#include "macros.h"
#include "game.h"
#include "platform.h"
#include "util.h"

#include "work_entry.h"

extern Game *game;
extern Platform *platform;

// void
// log_init(GameState *game_) {
// 	game = game_;
// }

struct Log_params
{
	char filename[256];
	char string[256];
};

void
THREAD_Log(void *data, Arena *thread_arena)
{
	Log_params params = *(Log_params *)data;

	platform->WriteLineToFile(params.filename, params.string);
}

void
_push_to_log_strings(char *str)
{
	strcpy(game->log_state.log_strings[(game->log_state.log_cur_index++)%MAX_LOG_COUNT], str);
	if(game->log_state.log_length == MAX_LOG_COUNT) {
		game->log_state.log_start_index += 1;
	}
	else game->log_state.log_length += 1;
}


void
InitLog(LogState *log_state) {
	platform->WriteLineToFile("logs/log.txt", "-------------------");
	//log_state->queue = (WorkQueue *)AllocPerma(sizeof(WorkQueue));
	platform->CreateWorkQueue(&log_state->queue, 1, "Logging");
}

void
PushLogEntry(char *filename, char *string, ...)
{
	Log_params params = {};

	va_list args;
	va_start(args, string);

	char formatted_str[ArrayCount(params.string)];
	// Apparently vsprintf() is re-entrant, but we may want to look into it a bit more.
	// and/or look into _beginthread()
	int formatted_length = vsprintf(formatted_str, string, args);
	if(formatted_length > ArrayCount(params.string))
	{
		// We exceeded the length of the formatted_str buffer, so we can only write a partial log
		formatted_str[ArrayCount(params.string)-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
		_push_to_log_strings(formatted_str);
		PushLogEntry("logs/log.txt", "%s", formatted_str);
		PushLogEntry("logs/log.txt", "ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}


	CopyMemoryBlock(params.filename, filename, ArrayCount(params.filename));
	params.filename[ArrayCount(params.filename)-1] = 0;
	CopyMemoryBlock(params.string, formatted_str, ArrayCount(params.string));
	params.filename[ArrayCount(params.string)-1] = 0;

	WorkEntry entry = {};
	entry.callback = THREAD_Log;
	entry.data = &params;
	entry.data_byte_count = sizeof(Log_params);

	platform->AddWorkEntry(game->log_state.queue, entry);
}

void Log(char *string, ...)
{
	va_list args;
	va_start(args, string);

	char formatted_str[MAX_LOG_LENGTH];
	// Apparently vsprintf() is re-entrant, but we may want to look into it a bit more.
	// and/or look into _beginthread()
	int formatted_length = vsprintf(formatted_str, string, args);
	if(formatted_length > MAX_LOG_LENGTH)
	{
		// We exceeded the length of the formatted_str buffer, so we can only write a partial log
		formatted_str[MAX_LOG_LENGTH-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
		_push_to_log_strings(formatted_str);
		PushLogEntry("logs/log.txt", "%s", formatted_str);
		PushLogEntry("logs/log.txt", "ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}

	PushLogEntry("logs/log.txt", formatted_str);
	// va_list args;
	// va_start(args, str);

	// char formatted_str[MAX_LOG_LENGTH];
	// int formatted_length = vsprintf(formatted_str, str, args);
	// if(formatted_length > MAX_LOG_LENGTH) {
	// 	// We exceeded the length of the formatted_str buffer, so we can only write a partial log
	// 	formatted_str[MAX_LOG_LENGTH-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
	// 	_push_to_log_strings(formatted_str);
	// 	Log("%s", formatted_str);
	// 	Log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
	// 	return;
	// }

	// platform->WriteLineToFile("logs/log.txt", formatted_str);
	//_push_to_log_strings(formatted_str);
}

u8 get_log_length() {
	return game->log_state.log_length;
}

char *get_nth_log_string(u8 n)
{
	u8 index = (game->log_state.log_start_index+n)%MAX_LOG_COUNT;
	return (char*)(game->log_state.log_strings[index]);
}

void LogToFile(char *filename, char *str, ...)
{
	va_list args;
	va_start(args, str);

	char formatted_str[MAX_LOG_LENGTH];
	int formatted_length = vsprintf(formatted_str, str, args);
	if(formatted_length > MAX_LOG_LENGTH)
	{
		// We exceeded the length of the formatted_str buffer, so we can only write a partial log
		formatted_str[MAX_LOG_LENGTH-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
		_push_to_log_strings(formatted_str);
		Log("%s", formatted_str);
		Log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}


	platform->WriteLineToFile(filename, formatted_str);
}

void VerboseError(const char *str, ...)
{
	if(!c::verbose_error_logging) return;

	va_list args;
	va_start(args, str);

	char formatted_str[MAX_LOG_LENGTH];
	int formatted_length = vsprintf(formatted_str, str, args);
	if(formatted_length > MAX_LOG_LENGTH) {
		// We exceeded the length of the formatted_str buffer, so we can only write a partial log
		formatted_str[MAX_LOG_LENGTH-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
		_push_to_log_strings(formatted_str);
		Log("%s", formatted_str);
		Log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}

	Log(formatted_str);
}

void TickLog()
{
	Log("tick");
}
