#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <chrono>
#include <thread>
#include <stdlib.h> /* srand, rand */

pthread_mutex_t forks[5];
pthread_mutex_t arbitrator;
bool continueRunning = true; //flag to stop threads

struct thread_info /* Used as argument to thread_start() */
{
    pthread_t thread_id; /* ID returned by pthread_create() */
    int thread_num;      /* Application-defined thread # */
    long eat_count;      /* number of times the philosopher ate # */
    double eat_time;     /* total time the philosopher spent eating*/
    long think_count;    /* number of times the philospher thought */
    double think_time;   /* total time the philosopher spent thinking */
};

/***************************************************************************
* static void gatherForks(int n)
* Author: Rigre R. Garciandia
* Date: 22 October 2021
* Description: assigns a pair of forks to a philosopher (a pair of mutexes to a thread)
*
* Parameters:
* n I/P int the number of the thread accessing the forks
**************************************************************************/
static void gatherForks(int tnum)
{
    pthread_mutex_lock(&arbitrator);

    //take two forks
    pthread_mutex_lock(&forks[tnum]);
    pthread_mutex_lock(&forks[(tnum + 1) % 5]);

    pthread_mutex_unlock(&arbitrator);
}

static int sleepForRandomTime()
{
    int milliseconds = (rand() % (49 - 25 + 1)) + 25;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    return milliseconds;
}

static void eat(thread_info *tinfo)
{
    tinfo->eat_count += 1;
    tinfo->eat_time += (double)sleepForRandomTime() / 1000; //convert to seconds before adding total
}

static void think(thread_info *tinfo)
{
    tinfo->think_count += 1;
    tinfo->think_time += (double)sleepForRandomTime() / 1000;
}

static void *thread_start(void *arg)
{
    struct thread_info *tinfo = (struct thread_info *)arg;

    while (continueRunning)
    {
        gatherForks(tinfo->thread_num);
        eat(tinfo);

        //release forks after done eating
        pthread_mutex_unlock(&forks[tinfo->thread_num]);
        pthread_mutex_unlock(&forks[(tinfo->thread_num + 1) % 5]);

        think(tinfo);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    for (int i = 0; i < 5; i++) //initialize fork mutexes
    {
        pthread_mutex_init(&forks[i], NULL);
    }

    pthread_mutex_init(&arbitrator, NULL); //initialize arbitrator mutex

    thread_info tinfo[5];
    pthread_attr_t attr;
    int stack_size;
    void *res;

    int num_threads = 5;

    /* Initialize thread creation attributes */

    pthread_attr_init(&attr);

    /* Create one thread for each philosopher */

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        tinfo[tnum].thread_num = tnum + 1;

        /* The pthread_create() call stores the thread ID into
           corresponding element of tinfo[] */

        int s = pthread_create(&tinfo[tnum].thread_id, &attr,
                               &thread_start, &tinfo[tnum]);
        if (s != 0)
            printf("error creating thread");
    }

    /* Destroy the thread attributes object, since it is no
       longer needed */

    pthread_attr_destroy(&attr);

    /* Wait 5 minutes */
    std::this_thread::sleep_for(std::chrono::minutes(5));

    /* Now join with each thread, and display its returned value */
    continueRunning = false; //flag that tells threads to stop

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        pthread_join(tinfo[tnum].thread_id, &res);

        printf("PH%d\n", tinfo[tnum].thread_num);
        printf("Eat count: %ld \n", tinfo[tnum].eat_count);
        printf("Eat time: %f \n", tinfo[tnum].eat_time);
        printf("Think count: %ld \n", tinfo[tnum].think_count);
        printf("Think time: %f \n\n", tinfo[tnum].think_time);

        free(res); /* Free memory allocated by thread */
    }

    exit(EXIT_SUCCESS);
}