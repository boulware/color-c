#ifndef LOG_H
#define LOG_H

#include "types.h"
#include "platform.h"

#define MAX_LOG_LENGTH 1000
#define MAX_LOG_COUNT 10

struct LogState
{
	u8 log_start_index = 0;
	u8 log_cur_index = 0;
	u8 log_length = 0;
	char log_strings[MAX_LOG_COUNT][MAX_LOG_LENGTH];
    WorkQueue *queue;
	bool show_log;
};

void PushLogEntry(char *filename, char *string, ...);
void Log(char *string, ...);

#endif