// Name:       Ben Belden
// Class ID#:  bpb2v
// Section:    CSCI 6330-001
// Assignment: Lab #3
// Due:        23:59, November 24, 2015
// Purpose:    Write an MPI program to compute an integral.
// Input:      From terminal.
// Output:     Print to terminal.
//
// File:       lab2.cpp
//

/*
	fnInt(sqrt(3(sin (x/2))^3),x,dx) from 0 to pi
	user gives number of intervals (no more than 100)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
	int myrank,nprocs;
	const float PI = 3.1415926535897;
	float x,y,w,myArea,areaSum;

	//MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

	w=x=y=myArea=areaSum=0;
	w = PI / (float)nprocs;
	x = ((((float)myrank+1.0)*w)+((float)myrank*w))/2.0; 
	y = sqrt(3.0 * (sin(x/2.0)) * (sin(x/2.0)) * (sin(x/2.0)));
	myArea=w*y;
	//printf("rank: %i\n",myrank);
	
	if (myrank != 0){
		//MPI_Recv(&areaSum,1,MPI_FLOAT,(myrank-1),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&areaSum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//printf("receive: %i\n",myrank);
	}
	areaSum+=myArea;

	//MPI_Send(&areaSum,1,MPI_FLOAT,((myrank+1)%nprocs),0,MPI_COMM_WORLD);
	MPI_Send(&areaSum,1,MPI_FLOAT,((myrank+1)%nprocs),0,MPI_COMM_WORLD);

	if(myrank==0){
		//MPI_Recv(&areaSum,1,MPI_FLOAT,(nprocs-1),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&areaSum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("answer: %f\n",areaSum);
	}

	return 0;
}
