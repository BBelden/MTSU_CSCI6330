// Name:       Ben Belden
// Class ID#:  bpb2v
// Section:    CSCI 6330-001
// Assignment: Lab #1
// Due:        23:59, September 14, 2015
// Purpose:     
// Input:      From pre-formatted file.
// Output:     Print to terminal.
//
// File:       lab1.cpp
//

#include <stdio.h>              // for printf
#include <pthread.h>            // for pthread
#include <unistd.h>             // for usleep

const int MAX_ACTV_PLYRS = 3;
int currentPlayer = 0;
int playLength;

pthread_mutex_t mv;

void *play(void *threadid)
{
    pthread_mutex_lock(&mv);
        //int player = currentPlayer;
        //int length = playLength;
        int thrd = *((int*)threadid);
    pthread_mutex_unlock(&mv);
    while  (currentPlayer <= 10)
    {
        printf("Thread %i, Player %i, play length %i\n",thrd, currentPlayer,playLength);
        sleep(playLength);
        pthread_mutex_lock(&mv);
            currentPlayer++;
            scanf("%i",&playLength);
            //player = currentPlayer;
            //length = playLength;
        pthread_mutex_unlock(&mv);
    }
    pthread_exit(NULL);
}

int main( ){
    pthread_t threads[MAX_ACTV_PLYRS];
    pthread_mutex_init(&mv,NULL);
    int ind[MAX_ACTV_PLYRS];

    for (int i=0;i<MAX_ACTV_PLYRS;i++)
    {
        ind[i] = i;
        scanf("%i",&playLength);
        currentPlayer++;
        pthread_create(&threads[i],NULL,play,(void*)&ind[i]);
        usleep(1); //HAAAAACK!!!
    }
    for (int i=0;i<MAX_ACTV_PLYRS;i++)
        pthread_join(threads[i],NULL);
    return 0;
}

