#ifndef PTHREAD_POOL_H
#define PTHREAD_POOL_H

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Task
{
    void (*function)(void *);
    void *arg;
    struct Task *next;
} Task;

typedef struct TaskQueue
{
    Task *front;
    Task *rear;
    size_t size;
} TaskQueue;

typedef struct PThreadPool
{
    pthread_t *threads;
    size_t thread_count;
    TaskQueue *task_queue;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;
    bool stop;
} PThreadPool;

TaskQueue *create_task_queue();
void enqueue_task(TaskQueue *queue, void (*function)(void *), void *arg);
Task *dequeue_task(TaskQueue *queue);
void destroy_task_queue(TaskQueue *queue);
void *worker_thread(void *arg);
PThreadPool *create_thread_pool(size_t thread_count);
void add_task_to_pool(PThreadPool *pool, void (*function)(void *), void *arg);
void destroy_thread_pool(PThreadPool *pool);

TaskQueue *create_task_queue()
{
    TaskQueue *queue = (TaskQueue *)malloc(sizeof(TaskQueue));
    if (!queue)
    {
        perror("Failed to create task queue");
        return NULL;
    }
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue_task(TaskQueue *queue, void (*function)(void *), void *arg)
{
    Task *task = (Task *)malloc(sizeof(Task));
    if (!task)
    {
        perror("Failed to allocate task");
        return;
    }
    task->function = function;
    task->arg = arg;
    task->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = task;
    }
    else
    {
        queue->rear->next = task;
        queue->rear = task;
    }
    queue->size++;
}

Task *dequeue_task(TaskQueue *queue)
{
    if (queue->front == NULL)
    {
        return NULL;
    }

    Task *task = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    queue->size--;
    return task;
}

void destroy_task_queue(TaskQueue *queue)
{
    while (queue->front != NULL)
    {
        Task *temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    free(queue);
}

void *worker_thread(void *arg)
{
    PThreadPool *pool = (PThreadPool *)arg;

    while (true)
    {
        pthread_mutex_lock(&pool->queue_lock);

        while (pool->task_queue->size == 0 && !pool->stop)
        {
            pthread_cond_wait(&pool->queue_cond, &pool->queue_lock);
        }

        if (pool->stop)
        {
            pthread_mutex_unlock(&pool->queue_lock);
            break;
        }

        Task *task = dequeue_task(pool->task_queue);
        pthread_mutex_unlock(&pool->queue_lock);

        if (task)
        {
            task->function(task->arg);
            free(task);
        }
    }

    return NULL;
}

PThreadPool *create_thread_pool(size_t thread_count)
{
    PThreadPool *pool = (PThreadPool *)malloc(sizeof(PThreadPool));
    if (!pool)
    {
        perror("Failed to create thread pool");
        return NULL;
    }

    pool->thread_count = thread_count;
    pool->threads = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    pool->task_queue = create_task_queue();
    pool->stop = false;

    pthread_mutex_init(&pool->queue_lock, NULL);
    pthread_cond_init(&pool->queue_cond, NULL);

    for (size_t i = 0; i < thread_count; i++)
    {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0)
        {
            perror("Failed to create worker thread");
            destroy_thread_pool(pool);
            return NULL;
        }
    }

    return pool;
}

void add_task_to_pool(PThreadPool *pool, void (*function)(void *), void *arg)
{
    pthread_mutex_lock(&pool->queue_lock);

    enqueue_task(pool->task_queue, function, arg);

    pthread_cond_signal(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_lock);
}

void destroy_thread_pool(PThreadPool *pool)
{
    if (!pool)
    {
        return;
    }

    pthread_mutex_lock(&pool->queue_lock);
    pool->stop = true;
    pthread_cond_broadcast(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_lock);

    for (size_t i = 0; i < pool->thread_count; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);
    destroy_task_queue(pool->task_queue);
    pthread_mutex_destroy(&pool->queue_lock);
    pthread_cond_destroy(&pool->queue_cond);
    free(pool);
}

#endif // PTHREAD_POOL_H