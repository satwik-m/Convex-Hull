# Convex-Hull
1. A convex hull is a convex polygon that encloses all the points in it
1. Quick Hull Algorithm is used to find the convex hull of points in 2-D space in this project
1. The Basic idea is to improve runtime of the algorithm using K-means clustering technique
1. The points are divided into clusters and their respective convex hulls are found and combined
1. One more time the algorithm is applied to obtain the final Convex Hull
1. The parallelization is done in 3 ways using the following:
* OpenMP
* MPI
* Hybrid ( OpenMP + MPI )

