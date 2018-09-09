#include <stdio.h>
#include <mpi.h>

int main(int argc, char * argv[])
{
	int nproc, myrank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	printf("hello world from rank %d of the %d processes.\n",myrank,nproc);

	MPI_Finalize();

	return 0;
}
