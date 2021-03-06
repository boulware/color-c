#include "win32_work_queue.h"

#include "win32_platform.h"

void
win32_QueueReleaseSemaphore(WorkQueue *queue)
{
    ReleaseSemaphore(queue->semaphore_handle, 1, nullptr);
}

void
win32_AddWorkEntry(WorkQueue *queue, WorkEntry entry)
{
    if(!g_work_queue_system.init or !queue or !queue->init)
    {
        LogToFile("logs/critical.log", "Tried to add work entry before g_work_queue_system was init'd or with an uninit'd queue. Ignoring request.");
        return;
    }

    DWORD wait_code = WaitForSingleObjectEx(queue->mutex_handle, c::mutex_timeout_ms, FALSE);

    if(wait_code == WAIT_OBJECT_0)
    { // signaled
        if(RingBufferBytesRemaining(&queue->entry_data_buffer) >= entry.data_byte_count)
        {
            WriteBytesToRingBuffer(&queue->entry_data_buffer, entry.data, entry.data_byte_count);
            entry.data = nullptr; // Not strictly necessary, but maybe will catch some bugs if we try to access this later.

            queue->entries[queue->next_entry_to_write] = entry;
            u32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % ArrayCount(queue->entries);

            if(new_next_entry_to_write != queue->next_entry_to_read)
            { // Entry buffer isn't full.
                queue->next_entry_to_write = new_next_entry_to_write;
            }
            else
            { // Entry buffer is full. We won't move the write pointer, so subsequent calls will just continually overwrite
              // the current index until the read pointer moves forward.
                LogToFile("logs/critical.log", "Entry buffer was full.");
            }

            ++queue->job_queued_count;

            ReleaseSemaphore(queue->semaphore_handle, 1, nullptr);

        }
        else
        {
            // Not enough room left in ring buffer to store entry data.
            LogToFile("logs/critical.log", "WorkQueue ring buffer was full; not enough room to store entry data.");
        }

        _WriteBarrier();

        if(ReleaseMutex(queue->mutex_handle) == 0)
        {
            LogToFile("logs/critical.log", "Tried to release mutex without ownership");
        }
    }
    else if(wait_code == WAIT_TIMEOUT)
    {
        LogToFile("logs/critical.log", "WAIT_TIMEOUT returned for WaitForSingleObjectEx(). Skipping job.");
    }
    else
    {
        LogToFile("logs/critical.log", "wait_code wasn't WAIT_OBJECT_0 nor WAIT_TIMEOUT");
        // if(ReleaseMutex(queue->mutex_handle) == 0)
        // {
        //     LogToFile("logs/critical.log", "Tried to release mutex without ownership");
        // }
    }
}



bool
DoNextEntryOnWorkQueue(ThreadContext *context)
{
    WorkQueue *queue = context->queue;

    bool tried_to_do_something = false;

    DWORD wait_code = WaitForSingleObjectEx(queue->mutex_handle, INFINITE, FALSE);

    if(wait_code == WAIT_OBJECT_0)
    {

        Assert(queue->next_entry_to_read != queue->next_entry_to_write);
        tried_to_do_something = true;

        WorkEntry entry = queue->entries[queue->next_entry_to_read];
        queue->next_entry_to_read = (queue->next_entry_to_read + 1) % ArrayCount(queue->entries);

        size_t temp_data_size = 2048;
        Assert(entry.data_byte_count <= temp_data_size);
        void *temp_data = malloc(temp_data_size);

        ReadBytesFromRingBuffer(&queue->entry_data_buffer, temp_data, entry.data_byte_count);
        // entry.data_start_index ... I don't think we need this, since the entry data buffer should be in the same
        //                            order as the entry buffer

        ++queue->job_started_count;

        platform->ReadWriteBarrier();

        if(ReleaseMutex(queue->mutex_handle) == 0)
        {
            LogToFile("logs/critical.log", "Tried to release mutex without ownership");
        }

        entry.callback(temp_data, context->arena_id);

        InterlockedIncrement(&queue->job_completion_count);
        free(temp_data);
    }
    else
    {
        LogToFile("logs/critical.log", __FUNCTION__ "got wait_code that wasn't WAIT_OBJECT_0 [%u]", wait_code);
        ReleaseMutex(queue->mutex_handle);
    }

    return tried_to_do_something;
}

DWORD
ThreadProc(LPVOID lpParameter)
{
    ThreadContext *context = (ThreadContext *)lpParameter;
    WorkQueue *queue = context->queue; // alias

    WaitForSingleObjectEx(queue->semaphore_handle, INFINITE, FALSE);

    for(;;)
    {
        DoNextEntryOnWorkQueue(context);
        WaitForSingleObjectEx(queue->semaphore_handle, INFINITE, FALSE);
    }
}

void
win32_CreateWorkQueue(WorkQueue **queue_ptr, int thread_count, char *name)
{
    if(!g_work_queue_system.init)
    {
        LogToFile("logs/critical.log", "Tried to create work queue before g_work_queue_system was init'd. Ignoring request.");
        return;
    }

    if(thread_count > c::max_threads_per_work_queue)
    {
        LogToFile("logs/critical.log", "Tried to create work queue with higher thread_count than max_threads_per_work_queue. Ignoring request.");
        return;
    }

    *queue_ptr = Append(&g_work_queue_system.queues);
    WorkQueue *queue = *queue_ptr; // alias
    queue->semaphore_handle = CreateSemaphoreEx(NULL, 0, 1024, NULL, 0, SEMAPHORE_ALL_ACCESS);

    queue->thread_count = thread_count;

    queue->job_queued_count     = 0;
    queue->job_started_count    = 0;
    queue->job_completion_count = 0;

    //ZeroMemoryBlock(queue->name, ArrayCount(queue->name));
    if(name)
    {
        CopyCString(queue->name, name, ArrayCount(queue->name));
    }

    // Set up ring buffer to store entry data.
    queue->mutex_handle = CreateMutex(NULL, FALSE, NULL);
    queue->entry_data_buffer.next_index_to_write = 0;
    queue->entry_data_buffer.next_index_to_read = 0;
    queue->entry_data_buffer.size_in_bytes = Megabyte(1);
    queue->entry_data_buffer.data = win32_AllocateMemory(Megabyte(1));

    for(int i=0; i<thread_count; ++i)
    {
        char thread_arena_debug_name[64];
        snprintf(thread_arena_debug_name, 64, "WorkQueue thread %d/%d [%s]",
                                              i+1, thread_count, queue->name);
        thread_arena_debug_name[63] = '\0';

        PoolId<Arena> thread_arena_id = AllocArena(thread_arena_debug_name);
        Arena *arena = GetEntryFromId(game->arena_pool, thread_arena_id);
        Assert(arena);

        queue->thread_contexts[i] = {
            .arena_id = thread_arena_id,
            .queue = queue
        };

        CreateThread(0, 0, ThreadProc, (void *)&queue->thread_contexts, 0, nullptr);
    }

    queue->init = true;
}

// Returns the number of jobs that have been added to the queue but not completed
// ("Pending" in this context refers to any job that has been submitted but whose thread hasn't returned;
//  It *doesn't* mean jobs that have been posted, but have not yet been delegated a thread yet. Such jobs
//  are included in this count, but this query also counts jobs which have been delegated a thread but are
//  still currently executing)
size_t
win32_WorkQueuePendingJobCount(WorkQueue *queue)
{
    return queue->job_queued_count - queue->job_completion_count;
}