

#include <stdio.h>              // printf,scanf
#include <unistd.h>             // sleep
#include <pthread.h>            // pthread
//#include <sys/wait.h>


int jobqueue;
int job_available;
pthread_mutex_t job_queue_lock=PTHREAD_MUTEX_INITIALIZER;

void *producer (void *ignored)
{
    int myjobs[3]={20,8,1};
    int i=0;
    while (i<3)
    {
        pthread_mutex_lock(&job_queue_lock);
            if(job_available == 0)
            {
                job_available++;
                jobqueue = myjobs[i];
                printf("producing job %i with value %i in queue.\n",i,jobqueue);
            pthread_mutex_unlock(&job_queue_lock);
                sleep(1);
                i++;
            }
            else
            {
                printf("queue empty can't consume\n");
            pthread_mutex_unlock(&job_queue_lock);
        }
    }
    pthread_exit(0);
}

void *consumer (void *ignored)
{
    int count=0;

    while (count <3)
    {
        pthread_mutex_lock(&job_queue_lock);
            if(job_available==1)
            {
                job_available--;
                printf("consuming job %i with value %i\n",count,jobqueue);
        pthread_mutex_unlock(&job_queue_lock);
                count++;
                sleep(1);
            }
            else
            {
                printf("queue empty can't consume\n");
        pthread_mutex_unlock(&job_queue_lock);
            }
    }
    pthread_exit(0);
}


int main()
{
    pthread_t produce, consume;

    job_available=0;
    pthread_create(&produce,NULL,producer,NULL);
    pthread_create(&consume,NULL,consumer,NULL);
    pthread_join(produce,NULL);
    pthread_join(consume,NULL);
    
    return 0;
}
