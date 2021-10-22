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
    char     *argv_string;      /* From command-line argument */
};

/* Thread start function: display address near top of our stack,
   and return upper-cased copy of argv_string */

static void * thread_start(void *arg)
{
    printf("Hello from inside a thread");
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

        printf("Joined with thread %d; returned value was %s\n",
               tinfo[tnum].thread_num, (char *)res);
        free(res); /* Free memory allocated by thread */
    }

    exit(EXIT_SUCCESS);
}