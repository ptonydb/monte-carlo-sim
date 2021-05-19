#include "mpi.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cmath>

//number of iterations to be divided among the processes
#define ATTEMPTS 1000000

int main (int argc,  char *argv[]) {
    
    int myid, numprocs;
    int namelen;

    //for printing purposes
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    //MPI instance
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    
    MPI_Get_processor_name(processor_name, &namelen);

    //calculate the amount of iterations per proc
    //and its starting and ending index
    int size_per_proc = (int)floor(ATTEMPTS/numprocs);
    int s0 = size_per_proc + ATTEMPTS%numprocs;

    int startIndex = s0 + (myid-1) * size_per_proc;
    int endIndex = startIndex + size_per_proc;

    //makes sure each process has a different rng seed
    srand(time(NULL)+myid);

    //timing the program
    double startwtime;
    if (myid == 0) {
        startwtime = MPI_Wtime();
    }

    int count = 0;

    if (myid == 0) {
        double x, y, z;
        for (int i = 0; i < s0; i++) {
            x = (double)rand()/RAND_MAX;
            y = (double)rand()/RAND_MAX;
            z = (x*x)+(y*y);
            if (z <= 1)
                count++;
        }        
        printf("Process %d - startIndex 0 endIndex %d;count %d\n",myid,s0-1,count);
    }
    else {
        double x, y, z;
        for (int i = startIndex; i < endIndex;i++) {
            x = (double)rand()/RAND_MAX;
            y = (double)rand()/RAND_MAX;
            z = (x*x)+(y*y);
            if (z <= 1)
                count++;
        }
        printf("Process %d - startIndex %d endIndex %d;count %d\n",myid,startIndex,endIndex-1,count);
    }
    
    int sum_count = 0;
    MPI_Reduce(&count, &sum_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myid == 0)
    {
        double pi = (double)(sum_count*4)/ATTEMPTS;
        double runTime = MPI_Wtime() - startwtime;

        printf("Execution time in seconds = %f pi = %f\n", runTime, pi);
    }

    MPI_Finalize();
}