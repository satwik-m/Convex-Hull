#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <iostream>
using namespace std;
int findSide(int p1x,int p1y,int p2x,int p2y,int px,int py)
{
    int val = (py - p1y) * (p2x - p1x) -
            (p2y - p1y) * (px - p1x);

    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

// returns a value proportional to the distance
// between the point p and the line joining the
// points p1 and p2
int lineDist(int px,int py,int qx,int qy,int rx,int ry)
{
    return abs ((ry - py) * (qx - px) -
            (qy - py) * (rx - px));
}

int setfunction(int hullx[],int hully[],int index,int convx[],int convy[]){
    //printf("set function1\n");
    int ind=0,temp;
    int rank, size,i;


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for(int i=0;i<index;i++){
        int first=i%2;
        #pragma omp parallel for shared(first,hullx,hully)
        for(int j=first;j<index-1;j+=2){
            if (hullx[j]>hullx[j+1]){
                temp=hullx[j];
                hullx[j]=hullx[j+1];
                hullx[j+1]=temp;

                temp=hully[j];
                hully[j]=hully[j+1];
                hully[j+1]=temp;
            }
            else if(hullx[j+1]==hullx[j]){
                if(hully[j]>hully[j+1]){
                    temp=hully[j];
                    hully[j]=hully[j+1];
                    hully[j+1]=temp;
                }
            }
        }
    }
    //printf("set function2:\n");
    i=0;
    while (i<index){
        //printf("set function3:\n");
        convx[ind]=hullx[i];
        convy[ind]=hully[i];
        ind++;
        if(hullx[i]==hullx[i+1] && hully[i]==hully[i+1])
            i=i+2;
        else
            i=i+1;
    }
    //printf("#\n");
    //MPI_Barrier(MPI_COMM_WORLD);
    return ind;
}

// End points of line L are p1 and p2.  side can have value
// 1 or -1 specifying each of the parts made by the line L
int quickHull(int x[],int y[], int n, int px,int py,int qx,int qy, int side,int hullx[],int hully[],int index)
{
    int ind = -1;
    int max_dist = 0;
    //int rank, size;

    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MPI_Comm_size(MPI_COMM_WORLD, &size);

    // finding the point with maximum distance
    // from L and also on the specified side of L.
    #pragma omp parallel for shared(px,py,qx,qy,x,y,max_dist,ind)
    for (int i=0; i<n; i++)
    {
        int temp = lineDist(px,py, qx,qy, x[i],y[i]);
        if (findSide(px,py,qx,qy,x[i],y[i]) == side && temp > max_dist)
        {
            ind = i;
            max_dist = temp;
        }
    }

    // If no point is found, add the end points
    // of L to the convex hull.
    if (ind == -1)
    {
        hullx[index]=px;
        hully[index]=py;
        index=index+1;
        hullx[index]=qx;
        hully[index]=qy;
        index=index+1;
        return index;
    }

    // Recur for the two parts divided by a[ind]
    int i1=0,i2=0;
    int tx1[n],tx2[n],ty1[n],ty2[n];
    #pragma omp task shared(i1,tx1,ty1)
    {
        i1=quickHull(x,y,n, x[ind],y[ind],px,py, -findSide(x[ind],y[ind],px,py, qx,qy),tx1,ty1,i1);
    }
    #pragma omp task shared(i2,tx2,ty2)
    {
        i2=quickHull(x,y, n, x[ind],y[ind],qx,qy, -findSide(x[ind],y[ind], qx,qy, px,py),tx2,ty2,i2);
    }
    #pragma omp taskwait
        for(int i=0;i<i1;i++){
            hullx[index+i]=tx1[i];
            hully[index+i]=ty1[i];
        }
        index+=i1;
        for(int i=0;i<i2;i++){
            hullx[index+i]=tx2[i];
            hully[index+i]=ty2[i];
        }
        index+=i2;
        return index;
}

void findHull(int x[],int y[],int n,int *convx,int *convy,int *n_c)
{
    int hullx[n],hully[n],index=0;
    int rank, size;


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // a[i].second -> y-coordinate of the ith point
    if (n < 3)
    {
        printf("Convex hull not possible\n");
        return;
    }

    // Finding the point with minimum and
    // maximum x-coordinate
    int min_x = 0, max_x = 0;
    #pragma omp parallel for shared(x,min_x,max_x)
    for (int i=1; i<n; i++)
    {
        if (x[i] < x[min_x])
            min_x = i;
        if (x[i] > x[max_x])
            max_x = i;
    }

    int i1=0,i2=0;
    int tx1[n],tx2[n],ty1[n],ty2[n];
    #pragma omp task shared(i1,tx1,ty1)
    {
        i1=quickHull(x,y,n, x[min_x],y[min_x], x[max_x],y[max_x], 1,tx1,ty1,i1);
    }
    #pragma omp task shared(i2,tx2,ty2)
    {
        i2=quickHull(x,y, n, x[min_x],y[min_x], x[max_x],y[max_x], -1,tx2,ty2,i2);
    }
    #pragma omp taskwait
        for(int i=0;i<i1;i++){
            hullx[index+i]=tx1[i];
            hully[index+i]=ty1[i];
        }
        index+=i1;
        for(int i=0;i<i2;i++){
            hullx[index+i]=tx2[i];
            hully[index+i]=ty2[i];
        }
        index+=i2;
    int ind = setfunction(hullx,hully,index,convx,convy);
    //MPI_Barrier(MPI_COMM_WORLD);

    *n_c=ind;
    //printf("@@@%d convx[0]:%d\n",ind,convx[0]);

    //printf("%d\n",ind);

}


// Driver code
/*int main(int argc,char** argv)
{
    int n=50;
    int rank, size,k=4;

    int x[k][50],y[k][50],convx[k][50],convy[k][50];
    int n_c=0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(0));
    //for(int j=0;j<k;j++)
    if(rank==0)
    for(int i=0;i<50;i++){
        x[rank][i]=rand()%100;
        y[rank][i]=rand()%100;
    }
    else
        for(int i=0;i<50;i++){
        x[rank][i]=rand()%1000;
        y[rank][i]=rand()%1000;
    }

    
    cout<<"rank:"<<rank<<endl;

    findHull(&x[rank][0],&y[rank][0],n,&convx[rank][0],&convy[rank][0],&n_c);

    MPI_Barrier(MPI_COMM_WORLD);
    cout<<"hello"<<endl;

    for(int j=0;j<k;j++)
    {
        if(rank==j)
        {
        printf("hull %d:\n",j);
    for(int i=0;i<n_c;i++){
        printf("(%d,%d)\n",convx[rank][i],convy[rank][i] );
    }
    }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}*/
