// Name:		Ben Belden
// Class ID#:	bpb2v
// Section:		CSCI 6330-001
// Assignment:	Lab #5
// Due:			23:59:59, November 24.2015
// Purpose:		Write an MPI program to compute an integral.
// Input:		None.
// Output:		To terminal.
// 
// File:		lab2.cpp

//	f(x) = sqrt(3(sin(x/2))^3)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
	int myrank,nprocs;
	float x,y,width,myArea,areaSum;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

	x=y=width=myArea=areaSum=0.0;
	width=3.1415926535/(float)nprocs;
	x=(((float)myrank*width)+(((float)myrank+1.0)*width))/2.0;
	y=sqrt(3.0*(sin(x/2.0))*(sin(x/2.0))*(sin(x/2.0)));
	myArea=width*y;

	if (myrank!=0)
		MPI_Recv(&areaSum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	areaSum+=myArea;

	MPI_Send(&areaSum,1,MPI_FLOAT,((myrank+1)%nprocs),0,MPI_COMM_WORLD);

	if (myrank==0){
		MPI_Recv(&areaSum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("answer: %f\n",areaSum);
	}
	MPI_Finalize();
	return 0;
}

