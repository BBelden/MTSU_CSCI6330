/* 
Author: 	H. Stephen Morse
Reference:	Adapted from Sample Problem found in Appendix A of the book
				Practical Parallel Computing. AP Professional. 1994
Modified by: 	Chrisila Pettey
Modification date:	February 2012
Purpose: 	This program will calulate the max, the min, the average,
				a histogram, and a percentile array for a large number
				of SAT scores. The program reads the file name from 
				the user. The file contains the number of scores on 
				the first line followed by a score per line.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main()
{
	char file_name[128];
	FILE * file1;
	int * scores;
	int num_scores, max_scores, min_scores;
	int average_int;
	float temp1,temp2;
	double sum_scores, sumsq_scores;
	double average, standard_dev;
	int histogram[1601];
	double percentile[1601];

	register int i, curr_score;

	// prompt user for file name.

	printf("Enter the name of the file containing the scores\n");
	scanf("%s",file_name);

	// open file for update

	if ( (file1 = fopen( file_name,"rw")) == NULL)
	{
		printf("Cannot open file.\n");
		exit(-1);
	}

	// read in total number of scores in the file

	double time1 = omp_get_wtime();
	fscanf(file1, "%d", &num_scores);

	// allocate memory to hold the array

	scores = (int *)malloc(num_scores * sizeof(int));

	// read in the array

	for (i=0;i<num_scores;i++)
		fscanf(file1,"%d",&scores[i]);

	// initialize variables

	max_scores = -1;
	min_scores = 1601;
	sum_scores = 0.;
	sumsq_scores = 0.;

	for (i=0;i<1601;i++) histogram[i]=0;

	// loop over all the scores, updating max, min, sum, sum of squares, and histogram

	for (i=0;i<num_scores;i++)
	{
		curr_score = scores[i];
		if (curr_score < min_scores) min_scores = curr_score;
		if (curr_score > max_scores) max_scores = curr_score;
		sum_scores += (double)curr_score;
		sumsq_scores += (double)curr_score * (double)curr_score;
		histogram[curr_score] += 1;
	}

	// calculate the average and standard deviation

	average = sum_scores/num_scores;
	standard_dev = sqrt(sumsq_scores/num_scores - average*average);

	// round average to an integer

	average_int = (int)(average + .5);

	// loop over the scores array, subtracting the average from each entry

	for (i=0;i<num_scores;i++) scores[i] -= average_int;

	// print out all values and arrays

	printf("min score is %d\n",min_scores);
	printf("max score is %d\n",max_scores);
	printf("average scores is %d\n",average_int);
	printf("standard deviation is %f\n",standard_dev);

	// close file and exit

	double time2 = omp_get_wtime();
	printf("time: %f\n",time2-time1);
	fclose(file1);
	exit(0);

}



