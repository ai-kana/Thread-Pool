#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

struct Task {
    _Atomic(bool) isCompleted;
    void* result;
    void* (*function)();
};

static _Atomic(int) workptr = -1;
static struct Task* work[10];
static void* thread_worker(void* restrict arg) {
    while (true) {
        if (workptr == -1) {
            continue;
        }

        struct Task* task = work[workptr];
        task->result = task->function();
        task->isCompleted = true;
    }

    return NULL;
}

static struct {
    pthread_t thread;
    bool isInitialized;
} pool_data;

static void init_threadpool() {
    if (pool_data.isInitialized) {
        return;
    }

    pthread_create(&pool_data.thread, NULL, &thread_worker, NULL);
    pool_data.isInitialized = true;
}

static void run_async(register struct Task* restrict task) {
    if (task->isCompleted) {
        return;
    }

    while (workptr >= 9);

    workptr++;
    work[workptr] = task;

    while (!task->isCompleted);
}

static void run_sync(register struct Task* restrict task) {
    if (task->isCompleted) {
        return;
    }

    task->result = task->function();
}

static void* test() {
    int* x = malloc(sizeof(int));
    *x = 7;
    return x;
}

int main() {
    init_threadpool();

    struct Task task;
    task.function = &test;
    task.isCompleted = false;

    run_async(&task);

    int *p = task.result;
    printf("Result: %d\n", *p);
    free(p);
}
