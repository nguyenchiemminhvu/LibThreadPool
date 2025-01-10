#include "pthreadpool.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

PThreadPool *pool = NULL;

void initialize_thread_pool()
{
    pool = create_thread_pool(4);
    if (!pool)
    {
        fprintf(stderr, "Failed to create thread pool.\n");
        exit(EXIT_FAILURE);
    }
}

void cleanup_thread_pool()
{
    if (pool)
    {
        destroy_thread_pool(pool);
        pool = NULL;
        printf("Thread pool destroyed. Exiting.\n");
    }
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\nSIGINT received. Cleaning up and exiting...\n");
        cleanup_thread_pool();
        exit(EXIT_SUCCESS);
    }
}

void sample_task(void *arg)
{
    int task_id = *(int *)arg;
    printf("Task %d is being processed by thread.\n", task_id);
    sleep(1);
    printf("Task %d is done processing.\n", task_id);
    free(arg);
}

int main()
{
    initialize_thread_pool();

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Failed to set up signal handler");
        cleanup_thread_pool();
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 10; i++)
    {
        int *task_id = (int *)malloc(sizeof(int));
        if (!task_id)
        {
            perror("Failed to allocate memory for task_id");
            cleanup_thread_pool();
            return EXIT_FAILURE;
        }
        *task_id = i + 1;
        add_task_to_pool(pool, sample_task, task_id);
    }

    while (1)
    {
        pause();
    }

    return EXIT_SUCCESS;
}