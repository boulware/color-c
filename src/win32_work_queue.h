#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <windows.h>
#include "ring_buffer.h"
#include "work_entry.h"

struct WorkQueue;

NoIntrospect
struct ThreadContext
{
    PoolId<Arena> arena_id;
    WorkQueue *queue;
};

struct WorkQueue
{
    bool init;
    HANDLE mutex_handle;

    int thread_count;
    ThreadContext thread_contexts[c::max_threads_per_work_queue];

    volatile size_t job_queued_count;     // # of jobs that have been added to the queue.
    volatile size_t job_started_count;    // # of jobs that have actually been sent off and run in a thread.
    volatile size_t job_completion_count; // # of jobs that have been delegated a thread and completely finished.

    volatile u32 next_entry_to_read;
    volatile u32 next_entry_to_write;
    HANDLE semaphore_handle;

    char name[32]; // Name that can be used to identify a work queue more easily.

    volatile RingBuffer entry_data_buffer;
    WorkEntry entries[1024];
};

NoIntrospect
struct WorkQueueSystem
{
    bool init;
    Array<WorkQueue> queues;
};


DWORD ThreadProc(LPVOID lpParameter);
void win32_QueueReleaseSemaphore(WorkQueue *queue);
void win32_AddWorkEntry(WorkQueue *queue, WorkEntry entry);
bool DoNextEntryOnWorkQueue(ThreadContext *context);
void win32_CreateWorkQueue(WorkQueue **queue_ptr, int thread_count, char *name);

#endif