#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    int       eat_count;        /* number of times the philosopher ate # */
    double    eat_time;         /* total time the philosopher spent eating*/
    int       think_count;      /* number of times the philospher thought */
    double    think_time;       /* total time the philosopher spent thinking */
};

/* Thread start function: display address near top of our stack,
   and return upper-cased copy of argv_string */

static void * thread_start(void *arg)
{
    struct thread_info *tinfo = (struct thread_info *)arg;

    tinfo->eat_count = 1;
    tinfo->eat_time = 2;
    tinfo->think_count = 3;
    tinfo->think_time = 4;

    return 0;
}

int main(int argc, char *argv[])
{
    thread_info tinfo[5];
    pthread_attr_t attr;
    int stack_size;
    void *res;

    int num_threads = 5;

    /* Initialize thread creation attributes */

    pthread_attr_init(&attr);

    /* Create one thread for each command-line argument */

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        tinfo[tnum].thread_num = tnum + 1;

        /* The pthread_create() call stores the thread ID into
           corresponding element of tinfo[] */

        pthread_create(&tinfo[tnum].thread_id, &attr,
                       &thread_start, &tinfo[tnum]);
    }

    /* Destroy the thread attributes object, since it is no
       longer needed */

    pthread_attr_destroy(&attr);

    /* Now join with each thread, and display its returned value */

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        pthread_join(tinfo[tnum].thread_id, &res);

        printf("PH%d\n", tinfo[tnum].thread_num);
        printf("Eat count: %d \n", tinfo[tnum].eat_count);
        printf("Eat time: %f \n", tinfo[tnum].eat_time);
        printf("Think count: %d \n", tinfo[tnum].think_count);
        printf("Think time: %f \n\n", tinfo[tnum].think_time);

        free(res); /* Free memory allocated by thread */
    }

    exit(EXIT_SUCCESS);
}