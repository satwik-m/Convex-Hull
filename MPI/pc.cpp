#include <iostream>
#include <cstdlib>
#include <math.h>
#include <mpi.h>
#include<omp.h>

#include "kMeans1.cpp"
#include "con_hull.cpp"

using namespace std;

int main(int argc,char** argv)
{
    //Read the 2d points from a file
    double t1,t2;
    
    int n=15000,k=3;
    int x[n],y[n],convx[n],convy[n],kc[n];
    int rank, size;

    t1=omp_get_wtime();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(0));
    if(rank==0)
    {
    cout<<"Generating Random Numbers...!!!"<<endl;
    for(int i=0;i<n;i++){
    	x[i]=rand()%100;
    	y[i]=rand()%100;
    	//x[i]=y[i]=i+1;
    }
    }

    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(y, n, MPI_INT, 0, MPI_COMM_WORLD);

    /*for(int i=0;i<200;i++){
    	cout<<x[i]<<","<<y[i]<<endl;
    }*/
    
    //call the clusters' function and get the clusters
    
    cout<<"Obtaining the clusters...!!"<<endl;
    find_clusters(n,k,x,y,kc);

    /*if(rank==0)
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
        //cout<<"****"<<t<<endl;
        //MPI_Barrier(MPI_COMM_WORLD);
        }
    }*/
    MPI_Bcast(kc, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int yc[n],xc[n],clust_size=0;
    /*for(int p=0;p<k;p++){
    	clust_size[p]=0;
    }*/
    //for(int j=0;j<k;j++){
    	//printf("Cluster %d\n", rank);
    	for(int i=0;i<n;i++){
       		if(kc[i]==rank){
       			//printf("point %d: (%d,%d)\n",i,x[i],y[i]);
    			clust_size+=1;
    			xc[clust_size-1]=x[i];
    			yc[clust_size-1]=y[i];
    			//cout<<x[i]<<","<<y[i]<<endl;
    		}
    	}
    
        MPI_Barrier(MPI_COMM_WORLD);
    
    cout<<"Finding the convex hulls for each cluster obtained..!!"<<endl;
    //for each cluster find the convex hull and store the points
    int con_x[n],con_y[n],con_n=0,*fin_x,*fin_y,fin_n=0;
    //printf("cluster size:\n",clust_size);

    findHull(xc,yc,clust_size,&con_x[0],&con_y[0],&con_n);
    /*for(int j=0;j<k;j++){
    	findHull(xc[j],yc[j],clust_size[j],con_x,con_y,&con_n);
    	printf("\n\nCluster %d\n", j+1);
    	for(int i=0;i<con_n;i++){
    		fin_x[fin_n+i]=con_x[i];
    		fin_y[fin_n+i]=con_y[i];
    		printf("(%d,%d)\n",con_x[i],con_y[i]);
    	}
    	fin_n+=con_n;
    	printf("l_n=%d,g_n=%d\n",con_n,fin_n );
    }*/
    //printf("con_n:%d convx[0]:%d\n",con_n,con_x[0]);

    /*for(int i=0;i<k;i++)
    {
        printf("abcdefgh:%d\n",rank);
        if(rank==i)
        {
            printf("hull:%d",rank);
            for(int j=0;j<con_n;j++)
                printf("(%d %d)\n",con_x[j],con_y[j]);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }*/
    int recv_counts[k];

    MPI_Gather(&con_n, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce( &con_n, &fin_n, 1, MPI_INT, MPI_SUM, 0,  MPI_COMM_WORLD);
    int recvx[fin_n],recvy[fin_n];

    int displ[k];
        if(rank==0)
        {
            //displ=(int*) malloc(3*sizeof(int));
            displ[0]=0;
            int sum=recv_counts[0];
            for(int i=1;i<3;i++){
                //printf("%d\n",recv_counts[i-1]);
                displ[i]=displ[i-1]+recv_counts[i-1];
                sum+=recv_counts[i];}
            //recv=(int*) malloc(sum*sizeof(int));

        }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gatherv(con_x, con_n, MPI_INT,recvx, recv_counts, displ, MPI_INT,0, MPI_COMM_WORLD);
    MPI_Gatherv(con_y, con_n, MPI_INT,recvy, recv_counts, displ, MPI_INT,0, MPI_COMM_WORLD);


    if(rank==0)
        {
            printf("total:%d\n",fin_n);
            /*for(int i=0;i<fin_n;i++)
                printf("(%d %d)\n",recvx[i],recvy[i]);*/

            findHull(recvx,recvy,fin_n,&con_x[0],&con_y[0],&con_n);

            printf("final hull:\n");
            for(int i=0;i<con_n;i++)
                printf(" (%d %d)\n",con_x[i],con_y[i]);
        }
    /*MPI_Gather( con_x, con_n, MPI_INT, fin_x, 100, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather( con_y, con_n, MPI_INT, fin_y, 100, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce( &con_n, &fin_n, 1, MPI_INT, MPI_SUM, 0,  MPI_COMM_WORLD);

    if(rank==0)
    {
        printf("fin_n:%d\n",fin_n);
        for(int j=0;j<fin_n;j++)
            printf("(%d %d)\n",fin_x[j],fin_y[j]);
    }*/
    //now find the convex hull for the points stored above
   /* printf("Finding the final Hull..!!\n");
    for(int i=0;i<fin_n;i++)
    	printf("(%d,%d)\n", fin_x[i],fin_y[i]);
    printf("%d\n",fin_n);
    findHull(fin_x,fin_y,fin_n,con_x,con_y,&con_n);
    printf("***\n");
    for(int i=0;i<con_n;i++){
        printf("(%d,%d)\n",con_x[i],con_y[i]);
    }*/
        t2=omp_get_wtime();
        if(rank==0)
            printf("time :%lf",t2-t1);
    MPI_Finalize();
    return 0;
}
