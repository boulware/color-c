#include "macros.h"
#include "game.h"
#include "platform.h"
#include "util.h"

#define MAX_LOG_LENGTH 1000
#define MAX_LOG_COUNT 10

extern Game *game;
extern Platform *platform;

// void
// log_init(GameState *game_) {
// 	game = game_;
// }

void
_push_to_log_strings(char *str)
{
	strcpy(game->log_state.log_strings[(game->log_state.log_cur_index++)%MAX_LOG_COUNT], str);
	if(game->log_state.log_length == MAX_LOG_COUNT) {
		game->log_state.log_start_index += 1;
	}
	else game->log_state.log_length += 1;
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
		log("%s", formatted_str);
		log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}

	platform->WriteLineToFile(filename, formatted_str);
}

void log(char *str, ...) {
	va_list args;
	va_start(args, str);

	char formatted_str[MAX_LOG_LENGTH];
	int formatted_length = vsprintf(formatted_str, str, args);
	if(formatted_length > MAX_LOG_LENGTH) {
		// We exceeded the length of the formatted_str buffer, so we can only write a partial log
		formatted_str[MAX_LOG_LENGTH-1] = '\0'; // vsprintf wouldn't have null-appended if the buffer was too small.
		_push_to_log_strings(formatted_str);
		log("%s", formatted_str);
		log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}

	platform->WriteLineToFile("logs/log.txt", formatted_str);
	_push_to_log_strings(formatted_str);
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
		log("%s", formatted_str);
		log("ERROR: Maximum log length exceeded (formatted_length=%d). Only partial log was written.", formatted_length);
		return;
	}

	log(formatted_str);
}

void TickLog()
{
	log("tick");
}

void start_log() {
	platform->WriteLineToFile("logs/log.txt", "-------------------");
}