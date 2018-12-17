#include<iostream>
#include<math.h>
#include<time.h>
using namespace std;
#define MAX 100000 // maximum number of objects to take.

void find_clusters(int n,int k,int *x,int *y,int *kc){
    int min=999999,max_it=50,it=0;
    int xcnt=0,cnt=0;
    int cenx[n],ceny[n],kc1[n];
    double d[n],euc_distance[k][n];
    
    srand(time(0));
    for(int i=0;i<n;i++){
        kc1[i]=rand()%k;
        kc[i]=rand()%k;
        cenx[i]=x[i];
        ceny[i]=y[i];
    }
        
    do
    {
        //"Calculating the euclidean distances"
        for(int m=0;m<k;m++) //m is for cluster number
        {
        for(int j=0;j<n;j++) //j is for object number
        {
            d[j]=(((cenx[m]-x[j])*(cenx[m]-x[j]))+((ceny[m]-y[j])*(ceny[m]-y[j])));

            euc_distance[m][j] = sqrt(d[j]);
        }
        }
        //Now,comparing if kc1 is equal to kc.
        int l=0;
        for(int j=0;j<n;j++)
        {
            if(kc1[j]==kc[j]) //checking each element
            {
                l++;
                //It checks if all the elements are equal.
            }
        }
        if(l==n)
        {
            //if yes then go outside this whole do-while loop
            return;

        }
        else //or just put the values of kc to kc1.
        {
            for(int j=0;j<n;j++)
                kc1[j]=kc[j];
        }

        //"Checking which data object is nearer to which cluster and creating cluster."

        for(int j=0;j<n;j++) // data objects
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

        }

        //Calculating the new centroid co-ordinates
        for(int m=0;m<k;m++) // cluster number
        {
            xcnt=0;
            for(int j=0;j<n;j++) //data objects
            {
                if(kc[j]==m)
                {
                    cenx[m]=x[j]+cenx[m];
                    ceny[m]=y[j]+ceny[m];
                    xcnt++;
                }
            }
            //cout<<"\nThe new centroids are: ";
            if(xcnt!=0){
            cenx[m]=cenx[m]/xcnt;
            ceny[m]=ceny[m]/xcnt;
            //cout<<cenx[m]<<","<<ceny[m]<<"\n";
            }
        }   
        it++;
    }while(it<max_it);
}
/*int main()
{
//initializing variables
int k,n;
int x[MAX],y[MAX],kc[MAX];
//double d[MAX],euc_distance[MAX][MAX];

//#Clusters
k=3;

//#points
n=100;
srand(time(0));
for(int i=0;i<n;i++)
{
    x[i]=rand()%10000;
    y[i]=rand()%10000;
    //x[i]=y[i]=i+1;
    //kc[i]=kc1[i]=0; //initializing as 0
}

find_clusters(n,k,x,y,kc);

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
}
cout<<"\n";
return 0;
}*/