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
	int myrank,nprocs,i;
	const float PI = 3.1415926535897932384626433;
	float x,y,w,tmp,sum;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	/*
	if (myrank==0)
	{
		//printf("intervals: %i\n",nprocs);
		w=x=y=tmp=sum=0;
		w = PI / nprocs;
		for (i=0;i<nprocs;i++)
		{
			x = (((i+1)*w)+(i*w))/2; 
			y = sqrt(3 * (sin(x/2)) * (sin(x/2)) * (sin(x/2)));
			tmp=w*y;
			sum+=tmp;
		}
		printf("answer: %f\n",sum);
	}
*/
	w=x=y=tmp=sum=0;
	w = PI / nprocs;
	x = (((myrank+1)*w)+(myrank*w))/2; 
	y = sqrt(3 * (sin(x/2)) * (sin(x/2)) * (sin(x/2)));
	tmp=w*y;
	
	if (myrank != 0)
	{
		MPI_Recv(&sum,1,MPI_FLOAT,myrank-1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//printf("Rank %d received sum %f from rank %d\n",myrank,sum,myrank-1);
	}

	sum+=tmp;

	MPI_Send(&sum,1,MPI_FLOAT,((myrank+1)%nprocs),1,MPI_COMM_WORLD);

	if(myrank==0)
	{
		MPI_Recv(&sum,1,MPI_FLOAT,nprocs-1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//printf("Rank %d received sum %f from rank %d\n",myrank,sum,myrank-1);
		printf("answer: %f\n",sum);
	}

	return 0;
}
