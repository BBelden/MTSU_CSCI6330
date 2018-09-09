// Name:       Ben Belden
// Class ID#:  bpb2v
// Section:    CSCI 6330-001
// Assignment: Homework #1
// Due:        23:59, August 31, 2015
// Purpose:    To make sure everything is working and you know how to do turnin, 
//             write a small pthreads program
// Input:      From terminal.
// Output:     Print to terminal.
//
// File:       hw1.cpp
//
// This program prompts the user for a number (1-10) and then creates that many 
// threads. Each thread prints a greeting and a local thread id.

#include <stdio.h>              // for printf
#include <iostream>             // for cin,cout,endl
#include <pthread.h>            // for pthread
#include <sys/types.h>          // for getpid() 
#include <unistd.h>             // for getpid()

using namespace std;

void *hello(void *threadid)
{
    printf("Hello from thread %llu!\n", *(int*)threadid);
    pthread_exit(NULL);
}

int main(){
    int numThrds = 0; // user input for number of threads
    // prompt user for number of threads
    printf("How many threads would you like (1-10)?\n");
    cin >> numThrds;
    // error checking for dummies
    while (numThrds < 1 || numThrds > 10){
        printf("I told you to choose a number between 1 and 10!\nTry again: ");
        cin >> numThrds; 
    }
    printf("Awesome! You are currently in process %i.\n", ::getpid());
    // pthreading!!
    pthread_t threads[numThrds];
    for(int i=0; i<numThrds; i++)
        pthread_create(&threads[i], NULL, hello, (void *)&i);
    for(int i=0; i<numThrds; i++)
        pthread_join(threads[i],NULL); // makes the parent process wait
    printf("Goodbye from process %i.\n", ::getpid());
    return 0;
}

