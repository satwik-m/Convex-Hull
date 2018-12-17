#include <iostream>
#include <cstdlib>
#include <math.h>
#include<omp.h>

#include "kMeans.cpp"
#include "con_hull.cpp"

using namespace std;

int main()
{
    //Read the 2d points from a file
    int n=15000,k=3 ;
    double t1,t2;
    int x[n],y[n],convx[n],convy[n],kc[n];
    srand(time(0));
    cout<<"Generating Random Numbers...!!!"<<endl;
    t1=omp_get_wtime();
    #pragma omp parallel for
    for(int i=0;i<n;i++){
    	x[i]=rand()%100;
    	y[i]=rand()%100;
    	//x[i]=y[i]=i+1;
    	//scanf("%d%d",&x[i],&y[i]);
        //x[i]=x[i]%1000;
        //y[i]=y[i]%1000;
    }
    /*for(int i=0;i<200;i++){
    	cout<<x[i]<<","<<y[i]<<endl;
    }*/
    
    //call the clusters' function and get the clusters
    cout<<"Obtaining the clusters...!!"<<endl;
    find_clusters(n,k,x,y,kc);

    int yc[k][n],xc[k][n],clust_size[k];
    #pragma omp parallel for
    for(int p=0;p<k;p++){
    	clust_size[p]=0;
    }
    //#pragma omp parallel num_threads(k)
    {
    #pragma omp for
    for(int j=0;j<k;j++){
    	//printf("Cluster %d; tid=%d\n", j+1,omp_get_thread_num());
    	for(int i=0;i<n;i++){
       		if(kc[i]==j){
       			//printf("point %d: (%d,%d)\n",i,x[i],y[i]);
    			clust_size[j]+=1;
    			xc[j][clust_size[j]-1]=x[i];
    			yc[j][clust_size[j]-1]=y[i];
    			//cout<<x[i]<<","<<y[i]<<endl;
    		}
    	}
    }
    }
    cout<<"Finding the convex hulls for each cluster obtained..!!"<<endl;
    //for each cluster find the convex hull and store the points
    int con_x[n],con_y[n],con_n=0,fin_x[n],fin_y[n],fin_n=0;
    #pragma omp parallel for num_threads(k) private(con_x,con_y,con_n) shared(xc,yc,clust_size,fin_x,fin_y)
    for(int j=0;j<k;j++){
    	int tid=omp_get_thread_num();
    	findHull(xc[j],yc[j],clust_size[j],con_x,con_y,&con_n);
    	//printf("\n\nCluster %d; tid=%d\n", j+1,omp_get_thread_num());
    	for(int i=0;i<con_n;i++){
    		fin_x[fin_n+i]=con_x[i];
    		fin_y[fin_n+i]=con_y[i];
    		//printf("(%d,%d)\n",con_x[i],con_y[i]);
    	}
    	fin_n+=con_n;
    	//printf("l_n=%d,g_n=%d\n",con_n,fin_n );
    }

    //now find the convex hull for the points stored above
    printf("Finding the final Hull..!!\n");
    #pragma omp parallel for
    for(int i=0;i<fin_n;i++)
    	printf("(%d,%d)\n", fin_x[i],fin_y[i]);
    printf("%d\n",fin_n);
    findHull(fin_x,fin_y,fin_n,con_x,con_y,&con_n);
    printf("***\n");
    #pragma omp parallel for
    for(int i=0;i<con_n;i++){
        printf("(%d,%d)\n",con_x[i],con_y[i]);
    }
    t2=omp_get_wtime();
    printf("Time Taken:%lf\n", t2-t1);
    return 0;
}