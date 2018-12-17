#include <algorithm>
#include<stdlib.h>
#include<omp.h>
#include<stdio.h>
using namespace std;
int main(int argc, char const *argv[])
{
	int A[10]={4,3,1,8,9,5,0,6,7,2};
	int N=10;
	for( int i = 0; i < N; i++ )
	{
		int first = i % 2; // 0 if i is 0, 2, 4, ...
						   // 1 if i is 1, 3, 5, ...
		#pragma omp parallel for default(none),shared(A,first)
		for( int j = first; j < N-1; j += 2 )
		{
			if( A[ j ] > A[ j+1 ] )
			{
				std::swap( A[ j ], A[ j+1 ] );
			}
		}
	}
	for (int i = 0; i < N; ++i)
	{
		printf("%d\n",A[i] );
	}
	return 0;
}
