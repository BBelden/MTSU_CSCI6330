// Name:       Ben Belden
// Class ID#:  bpb2v
// Section:    CSCI 6330-001
// Assignment: Lab #2
// Due:        23:59, October 7, 2015
// Purpose:     
// Input:      From pre-formatted file.
// Output:     Print to terminal.
//
// File:       lab2.cpp
//

#include <stdio.h>              // for printf
#include <iostream>             // for cin,cout,endl
#include <pthread.h>            // for pthread
#include <fstream>              // for ifstream
#include <deque>                // for deque
#include <unistd.h>				// for usleep
#include <sys/wait.h>			// for wait

using namespace std;

int done=0,epochs=0;
const int NUM_WORLDS = 4;
const int NUM_PLAYERS = 5;
struct player {char type;int id;player():type('T'),id(-1){}};
struct world
{
    player players[NUM_PLAYERS];
	player transport[2];
	bool dead[NUM_PLAYERS];
    bool dragon;
    int numAs;
    int numHs;
    world():dragon(0),numAs(0),numHs(0){}};
world worlds[NUM_WORLDS];
deque<player> q;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv;

void printQueue()
{
	deque<player>::iterator it = q.begin();
	while(it != q.end())
	{
		printf("%i\t%c\tqueue\n",it->id,it->type);
		it++;
	}
	printf("\n");
}
void printWorld(int i)
{
	for (int j=0;j<NUM_PLAYERS;j++)
	{
		if (worlds[i].dead[j] == 1)
			printf("%i\t%c X\t%i\n",worlds[i].players[j].id,worlds[i].players[j].type,i);
		else
			printf("%i\t%c\t%i\n",worlds[i].players[j].id,worlds[i].players[j].type,i);
	}
	if (worlds[i].dragon == 1)
		printf("Dragon!\t\t%i\n",i);
	printf("\n");
}

void printWorlds()
{
	printf("Player\tType\tWorld\n");
	for (int i=0;i<NUM_WORLDS;i++)
	{
		for (int j=0;j<NUM_PLAYERS;j++)
		{
			if (worlds[i].dead[j] == 1)
				printf("%i\t%c X\t%i\n",worlds[i].players[j].id,worlds[i].players[j].type,i);
			else
				printf("%i\t%c\t%i\n",worlds[i].players[j].id,worlds[i].players[j].type,i);
		}
		if (worlds[i].dragon == 1)
			printf("Dragon!\t\t%i\n",i);
	}
	printQueue();
}

void shiftWorld(int i)
{
	player temp[NUM_PLAYERS];
	int j,ind=0;
	for (j=0;j<NUM_PLAYERS;j++)
	{
		if (worlds[i].dead[j] == 0)
		{
			temp[ind] = worlds[i].players[j];
			ind++;
		}
	}
	for (j=0;j<NUM_PLAYERS;j++)
	{
		worlds[i].players[j] = temp[j];
		worlds[i].dead[j] = 0;
	}
}

void fight(int i)
{
	int val = worlds[i].numAs - worlds[i].numHs;
	if (val == 5 || val == -5)
		return;
	switch (val){
		case  3 : ;
		case  1 : 
			for (int j=0;j<NUM_PLAYERS;j++)
			{
				if (worlds[i].players[j].type == 'H')
				{
					worlds[i].dead[j]=1;			
					worlds[i].numHs--;
					break;
				}
			}
			break;
		case -1 : ;  
		case -3 : 
			for (int j=0;j<NUM_PLAYERS;j++)
			{
				if (worlds[i].players[j].type == 'A')
				{
					worlds[i].dead[j]=1;			
					worlds[i].numAs--;
					break;
				}
			}
			break;
	}
}

void dragon(int i)
{
	if (worlds[i].dragon == 0)
		return;
	if (worlds[i].numAs > 0)
	{
		for (int j=0;j<NUM_PLAYERS;j++)
		{
			if (worlds[i].players[j].type == 'A' && worlds[i].dead[j] == 0)
			{
				worlds[i].dead[j] = 1;
				worlds[i].numAs--;
				break;
			}
		}
	}
	else
	{
		for (int j=0;j<NUM_PLAYERS;j++)
		{
			if (worlds[i].players[j].type == 'H' && worlds[i].dead[j] == 0)
			{
				worlds[i].dead[j] = 1;
				worlds[i].numHs--;
				break;
			}
		}
	}
}

void returnDead(int i)
{
	for (int j=0;j<NUM_PLAYERS;j++)
		if (worlds[i].dead[j] == 1)
			q.push_back(worlds[i].players[j]);
	shiftWorld(i);
}

void sendPlayers(int i)
{
	int put=0,j=0,next = i+1;
	if (next == NUM_WORLDS) next=0;
	worlds[next].transport[0] = worlds[i].players[0];
	worlds[next].transport[1] = worlds[i].players[1];
	worlds[i].dead[0]=worlds[i].dead[1]=1;
	if (worlds[i].players[0].type =='A')
		worlds[i].numAs--;
	else
		worlds[i].numHs--;
	if (worlds[i].players[1].type =='A')
		worlds[i].numAs--;
	else
		worlds[i].numHs--;
	shiftWorld(i);
	while (worlds[i].transport[1].type == 'T')
		;
	while (j<5 && put<2)
	{
		if (worlds[i].players[j].id != -1)
		{
			j++;
			continue;
		}
		else
		{
			worlds[i].players[j] = worlds[i].transport[put];
			put++;
		}
		j++;
	}
	if (worlds[i].transport[0].type == 'A')
		worlds[i].numAs++;
	else
		worlds[i].numHs++;
	if (worlds[i].transport[1].type == 'A')
		worlds[i].numAs++;
	else
		worlds[i].numHs++;
		
}

void moveDragon(int i)
{
	if (worlds[i].dragon == 0)
		return;
	usleep(5000);
	int j=i-1;
	if (j == -1) j=4;
	worlds[i].dragon=0;
	worlds[j].dragon=1;
}

void getPlayers(int i)
{
	player temp;
	int total = worlds[i].numAs + worlds[i].numHs;
	while (total < NUM_PLAYERS)
	{
		temp = q.front();
		q.pop_front();
		worlds[i].players[total] = temp;
		if (temp.type == 'A')
			worlds[i].numAs++;
		else
			worlds[i].numHs++;
		total = worlds[i].numAs + worlds[i].numHs;
	}
}

void *play(void *threadid)
{
	while (epochs < 5)
	{
		int thrd = *((int*)threadid);
		fight(thrd);
		dragon(thrd);
		returnDead(thrd);
		sendPlayers(thrd);
		/****** wait for worlds to catch up  ******/
		/****** do this one at a time, not necessarily sequentially ******/
		pthread_mutex_lock(&lock);
		getPlayers(thrd);
		pthread_mutex_unlock(&lock);
		/****** do this once only ******/
		//pthread_mutex_lock(&lock);
		moveDragon(thrd);
		//pthread_mutex_unlock(&lock);
		done++;
		pthread_mutex_lock(&lock);
		if (done < NUM_WORLDS)
		{
			pthread_mutex_unlock(&lock);
			sleep(1);
		}
		else
		{
			done = 0;
			pthread_mutex_unlock(&lock);
		}
	}
	pthread_exit(NULL);
}

int main( ){
    ifstream myin;
    myin.open("players.dat");
    int d,i,j,k,id=1;
    player temp;
	myin >> temp.type;
	temp.id = id;
    while (myin)
    {
        if (temp.type == 'A' || temp.type == 'H')
            q.push_back(temp);
        else if (temp.type == 'D')
        {
            myin >> d;
            worlds[d].dragon = 1;
        }
		id++;
        myin >> temp.type;
		temp.id = id;
    }
	printQueue();
	for (i=0;i<NUM_WORLDS;i++)
	{
		for (j=0;j<NUM_PLAYERS;j++)
		{
			temp = q.front();
			q.pop_front();
			worlds[i].players[j] = temp;
			if (temp.type == 'A')
				worlds[i].numAs++;
			else
				worlds[i].numHs++;
			worlds[i].dead[j]=0;
		}
	}
	printf("Game at setup.\n");
	printWorlds();
    pthread_t threads[NUM_WORLDS];
    //pthread_mutex_init(&mv,NULL);
    pthread_cond_init(&cv,NULL);
	int worldNum[NUM_WORLDS]={0,1,2,3};

    for (i=0;i<NUM_WORLDS;i++)
        pthread_create(&threads[i],NULL,play,(void*)&worldNum[i]);
	k=0;
	while(epochs<5)
	{
		pthread_mutex_lock(&lock);
		if (done == NUM_WORLDS)
		{
			printf("Epoch %i\n",epochs+1);
			printWorlds();
		pthread_mutex_unlock(&lock);
			epochs++;	
		}
		else
			pthread_mutex_unlock(&lock);
	}

    for (i=0;i<NUM_WORLDS;i++)
        pthread_join(threads[i],NULL);

    return 0;
}

