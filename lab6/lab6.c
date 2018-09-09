// Name:		Ben Belden
// Class ID#:	bpb2v
// Section:		CSCI 6330-001
// Assignment:	Lab #6
// Due:			23:59:59, December 10, 2015
// Purpose:		Write an MPI program to implement quicksort.
// Input:		From pre-formatted file (lab6.dat).
// Outut:		To terminal.
// 
// File:		lab6.cpp

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
	int myrank,nprocs,tmp[100000],pivot,sendTo,rcvFm,keepSize,rcvSize,sndSize,lclDist;
	int nums=0,sent=0,flag=1;
	int a,b,c,d,i,j,t;
	int *keepArr,*rcvArr,*sndArr,*tmpArr;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

	// distance to first child
	int dist[nprocs];
	for(i=0;i<nprocs;i++)
		if(i%2==1)
			dist[i]=0;
	d=1;
	while(d<nprocs)
	{
		for(i=0;i<nprocs;i+=(d*2))
			dist[i]=d;
		d*=2;	
	}
	lclDist=dist[myrank];

	if (myrank == 0)
	{
		// open file and get values to sort
		FILE *fp;
		fp = fopen("./lab6.dat","r");
		if (fp == NULL)
		{
			printf("Cannot open file for reading.\n");
			exit(EXIT_FAILURE);
		}

		while(fscanf(fp,"%d",&tmp[nums])!=EOF)
			++nums;
		fclose(fp);	

		// copy array into correct size
		rcvArr=(int*)calloc(nums,sizeof(int));
		for(i=0;i<nums;i++)
			rcvArr[i]=tmp[i];
		rcvSize=nums;
		
		if(nprocs == 1)
		{
			// sort array
			for(i=0;(i<rcvSize)&&flag;i++)
			{
				flag=0;
				for(j=0;j<rcvSize-1;j++)
				{
					if(rcvArr[j+1]<rcvArr[j])
					{
						t=rcvArr[j];
						rcvArr[j]=rcvArr[j+1];
						rcvArr[j+1]=t;
						flag=1;
					}
				}
			}

			if (myrank==0)
				for(i=0;i<rcvSize;i++)
					printf("%d\n",rcvArr[i]);
			MPI_Finalize();
			return 0;
		}
	}
	
	// if myrank != 0, receive size then arr
	if(myrank !=0)
	{
		MPI_Recv(&rcvSize,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		rcvArr = (int*)calloc(rcvSize,sizeof(int));
		MPI_Recv(rcvArr,rcvSize,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}

	// split
	if (rcvSize > 1 && myrank%2==0)
	{
		sndSize=0;
		a = rcvArr[0];
		b = rcvArr[rcvSize/2];
		c = rcvArr[rcvSize-1];
		if ((a-b)*(c-a)>=0)
			pivot = a;
		else if((b-a)*(c-b)>=0)
			pivot = b;
		else
			pivot = c;
		for(i=0;i<rcvSize;i++)
			if(rcvArr[i]>=pivot)
				sndSize++;
		sndArr = keepArr = NULL;
		sndArr=(int*)calloc(sndSize,sizeof(int));
		keepSize = rcvSize - sndSize;
		keepArr =(int*) calloc(keepSize,sizeof(int));
		keepSize=sndSize=0;
		for(i=0;i<rcvSize;i++)
		{
			if(rcvArr[i]>=pivot)
			{
				sndArr[sndSize]=rcvArr[i];
				sndSize++;
			}
			else
			{
				keepArr[keepSize]=rcvArr[i];
				keepSize++;
			}
		}
		rcvArr=keepArr;
		rcvSize=keepSize;
	}
	else
	{
		sndSize=0;
		sndArr=NULL;
	}
		
	// while local distance >= 1 
	while(lclDist >= 1)
	{
		// send to myrank + localDistance
		sendTo=myrank+lclDist;
		MPI_Send(&sndSize,1,MPI_INT,sendTo,1,MPI_COMM_WORLD);
		MPI_Send(sndArr,sndSize,MPI_INT,sendTo,2,MPI_COMM_WORLD);
		sent = 1;
		if (sent==0)
		{
			if (dist[myrank] > 0)
			{
				// receive from myrank- (localDistance *2)
				rcvFm = (myrank-(lclDist*2))%nprocs;
				MPI_Recv(&rcvSize,1,MPI_INT,rcvFm,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				rcvArr = (int*)calloc(rcvSize,sizeof(int));
				MPI_Recv(rcvArr,rcvSize,MPI_INT,rcvFm,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			} 
		}

		// split
		if(rcvSize > 1 && myrank%2==0 && lclDist > 1)
		{
			sndSize=0;
			pivot=rcvArr[0];
			for(i=0;i<rcvSize;i++)
				if(rcvArr[i]>=pivot)
					sndSize++;
			sndArr = keepArr = NULL;
			sndArr=(int*)calloc(sndSize,sizeof(int));
			keepSize = rcvSize - sndSize;
			keepArr =(int*) calloc(keepSize,sizeof(int));
			keepSize=sndSize=0;
			for(i=0;i<rcvSize;i++)
			{
				if(rcvArr[i]>=pivot)
				{
					sndArr[sndSize]=rcvArr[i];
					sndSize++;
				}
				else
				{
					keepArr[keepSize]=rcvArr[i];
					keepSize++;
				}
			}
			rcvArr=keepArr;
			rcvSize=keepSize;
		}
		else
		{
			sndSize=0;
			sndArr=NULL;
		}
		lclDist /= 2;
	}

	// if size != 0
	if(rcvSize > 1)
	{
		// sort array
		for(i=0;(i<rcvSize)&&flag;i++)
		{
			flag=0;
			for(j=0;j<rcvSize-1;j++)
			{
				if(rcvArr[j+1]<rcvArr[j])
				{
					t=rcvArr[j];
					rcvArr[j]=rcvArr[j+1];
					rcvArr[j+1]=t;
					flag=1;
				}
			}
		}
	}
		
	// all point to the same thing, same sizes
	keepSize=sndSize=rcvSize;
	keepArr=sndArr=rcvArr;

	// odd nodes send back to myrank -1
	if(myrank%2==1)
	{
		MPI_Send(&sndSize,1,MPI_INT,myrank-1,1,MPI_COMM_WORLD);
		MPI_Send(sndArr,sndSize,MPI_INT,myrank-1,2,MPI_COMM_WORLD);
	}
	// even nodes
	else
	{
		lclDist=1;
		// while local distance <= dist[myrank]
		while(lclDist <= dist[myrank])
		{
			// receive from myrank + localDistance
			MPI_Recv(&rcvSize,1,MPI_INT,myrank + lclDist,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			rcvArr = (int*)calloc(rcvSize,sizeof(int));
			MPI_Recv(rcvArr,rcvSize,MPI_INT,myrank + lclDist,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			// concat
			int tmpSize = rcvSize + keepSize;
			tmpArr = (int*)calloc(tmpSize,sizeof(int));
			for(i=0;i<keepSize;i++)
				tmpArr[i]=keepArr[i];
			for(i=0;i<rcvSize;i++)
				tmpArr[keepSize+i] = rcvArr[i];
			keepSize+=rcvSize;
			keepArr=tmpArr;
			// localDistance *=2
			lclDist*=2;
		}
		if(myrank != 0)
		{
			// send to parent (myrank - dist[myrank]*2)
			MPI_Send(&keepSize,1,MPI_INT,(myrank-(dist[myrank]*2)),1,MPI_COMM_WORLD);
			MPI_Send(keepArr,keepSize,MPI_INT,(myrank-(dist[myrank]*2)),2,MPI_COMM_WORLD);
		}
	}

	// output sorted array
	if (myrank==0)
		for(i=0;i<keepSize;i++)
			printf("%d\n",keepArr[i]);
			
	MPI_Finalize();
	return 0;
}

