#include<iostream>
#include<math.h>
#include<time.h>
#include<mpi.h>

#include <cstdlib>
using namespace std;
#define MAX 100000 // maximum number of objects to take.

void find_clusters(int n,int k,int *x,int *y,int *kcen){
    struct { 
        double value; 
        int   rank; 
    } kc1[n], o[n],kc[n],d[n];
    int min=999999,max_it=5,it=0,i,j;
    int xcnt=0,cnt=0;
    int cenx[k],ceny[k];//,kc1[n],kc[n];
    double euc_distance[k][n];
    int rank,size;
    
    int root=0,flag=0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    srand(time(0));
    for(int i=0;i<n;i++){
        kc1[i].rank=rand()%k;
        kc[i].rank=rand()%k;
        if(i<k)
        {
        cenx[i]=rand()%100;
        ceny[i]=rand()%100;
        }
    }
    MPI_Bcast(cenx, k, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(cenx, k, MPI_INT, root, MPI_COMM_WORLD);
        
    do
    {
        //"Calculating the euclidean distances"
    #pragma omp parallel for shared(d)
        for(j=0;j<n;j++) //j is for object number
        {
            d[j].value=(((cenx[rank]-x[j])*(cenx[rank]-x[j]))+((ceny[rank]-y[j])*(ceny[rank]-y[j])));

            d[j].value = sqrt(d[j].value);
            d[j].rank=rank;
        }

        MPI_Barrier(MPI_COMM_WORLD);

        /*for(j=0;j<k;j++)
        {
            if(rank==j)
            {
                for(int i=0;i<n;i++)
                {
                    cout<<d[i].value<<" ";
                }
                cout<<endl;
            }
            MPI_Barrier(MPI_COMM_WORLD);
    
        }*/

        //new parallel code
        MPI_Reduce( d, kc, n, MPI_DOUBLE_INT, MPI_MINLOC, root,  MPI_COMM_WORLD);
        if(rank==root)
        {
            /*cout<<"cluster assignment:";
            for(i=0;i<n;i++)
                cout<<kc[i].rank<<" ";
        
        cout<<endl;*/
        //MPI_Barrier(MPI_COMM_WORLD);

        
        //Now,comparing if kc1 is equal to kc.
        //if(rank == root)
        
        int l=0;
    #pragma omp parallel for shared(l)
        for(j=0;j<n;j++)
        {
            if(kc1[j].rank==kc[j].rank) //checking each element
            {
                #pragma omp atomic
                    l++;
                //It checks if all the elements are equal.
            }
        }
        if(l==n)
        {
            //if yes then go outside this whole do-while loop
            
            //cout<<"l==n"<<endl;
            flag=1;
        #pragma omp parallel for
            for(i=0;i<n;i++)
                kcen[i]=kc[i].rank;
            //MPI_Bcast(&flag, 1, MPI_INT, root, MPI_COMM_WORLD);

            //break;
            
        }
        else //or just put the values of kc to kc1.
        {
        #pragma omp parallel for    
            for(j=0;j<n;j++)
                kc1[j].rank=kc[j].rank;
        }

        //"Checking which data object is nearer to which cluster and creating cluster."

        /*for(int j=0;j<n;j++) // data objects
        {
            for(int m=0;m<k;m++) //cluster number
            {
                if(euc_distance[m][j]<min)
                {
                    min=euc_distance[m][j];
                    kc[j]=m;
                }
            }
            min=999999;
            cenx[j]=ceny[j]=0; //For further calculation as it would be overwritten

        }*/

        //Calculating the new centroid co-ordinates
        for(int m=0;m<k;m++) // cluster number
        {
            xcnt=0;
            #pragma omp parallel for shared(kc,cenx,ceny,xcnt,m)
            for(int j=0;j<n;j++) //data objects
            {
                if(kc[j].rank==m)
                {
                    cenx[m]=x[j]+cenx[m];
                    ceny[m]=y[j]+ceny[m];
                    xcnt++;
                }
            }
            //cout<<"\nThe new centroids are: ";
            if(xcnt!=0)
            {
            cenx[m]=(int)(cenx[m]/xcnt);
            ceny[m]=(int)(ceny[m]/xcnt);
            }
            //cout<<cenx[m]<<","<<ceny[m]<<"\n";
        }
        //cout<<"braodcast"<<endl;
    }
        //cout<<"cenx[0]"<<cenx[1]<<endl;
        MPI_Bcast(cenx, k, MPI_INT, root, MPI_COMM_WORLD);
        //cout<<"cenx[0]"<<cenx[1]<<endl;
        MPI_Bcast(ceny, k, MPI_INT, root, MPI_COMM_WORLD);
        MPI_Bcast(&flag, 1, MPI_INT, root, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD); // wait for all processes
        it++;

    }while(it<max_it && flag==0);
    //MPI_Barrier(MPI_COMM_WORLD);

    //cout<<"it="<<it<<endl;
    if(rank==0)
    {
        #pragma omp parallel for
            for(i=0;i<n;i++)
                kcen[i]=kc[i].rank;
    }

}
/*int main(int argc,char** argv)
{
//initializing variables
int k,n=100;
int x[n],y[n],kc[n];
int rank, size;
//double d[MAX],euc_distance[MAX][MAX];

//#Clusters
k=3;

//#points
n=100;
srand(time(0));
for(int i=0;i<n;i++)
{
    //x[i]=rand()%100;
    //y[i]=rand()%100;
    x[i]=y[i]=i+1;
    //kc[i]=kc1[i]=0; //initializing as 0
}

MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


find_clusters(n,k,x,y,kc);
//cout<<"hi"<<endl;
if(rank==0)
{
for(int i=0;i<k;i++) //for cluster number
{
    //cout<<"\nCluster "<<i+1<<" centre is :("<<cenx[i]<<","<<ceny[i]<<")";

    cout<<"\nCluster:"<<i+1<<endl;
    int t=0;
    for(int j=0;j<n;j++)
    {
        if(kc[j]==i){ //if the cluster number is equal to i then print the elements i.e. present in the cluster
            cout<<"\n("<<x[j]<<","<<y[j]<<")";
            t+=1;
        }
    }
    cout<<"****"<<t<<endl;
    //MPI_Barrier(MPI_COMM_WORLD);
}

//for(int i=0;i<n;i++)
  //  cout<<x[i]<<" "<<y[i]<<endl;

cout<<"\n";
}
    MPI_Finalize();
return 0;
}*/