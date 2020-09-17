#ifndef LOG_H
#define LOG_H

#include "types.h"

struct LogState
{
	u8 log_start_index = 0;
	u8 log_cur_index = 0;
	u8 log_length = 0;
	char log_strings[10][1000];
	bool show_log;
};

void log(char *str, ...);

#endif