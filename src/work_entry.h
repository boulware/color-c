#ifndef WORK_ENTRY_H
#define WORK_ENTRY_H

NoIntrospect
struct WorkEntry
{
    // Filled out by work requester
    void (*callback)(void *, PoolId<Arena>);
    void *data;
    size_t data_byte_count;
};

#endif