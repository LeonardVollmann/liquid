#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef float f32;
typedef double f64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define ANSI_BLACK		"\e[1;30m"
#define ANSI_RED		"\e[1;31m"
#define ANSI_GREEN		"\e[1;32m"
#define ANSI_YELLOW		"\e[1;33m"
#define ANSI_BLUE		"\e[1;34m"
#define ANSI_MAGENTA	"\e[1;35m"
#define ANSI_CYAN		"\e[1;36m"
#define ANSI_WHITE 		"\e[1;37m"
#define ANSI_RESET		"\e[0m"

#define COLOR_INFO ANSI_BLUE
#define COLOR_WARN ANSI_YELLOW
#define COLOR_ERROR ANSI_RED
#define COLOR_FATAL ANSI_RED
#define COLOR_FILE ANSI_CYAN
#define COLOR_LINE ANSI_WHITE
#define COLOR_FUNC ANSI_MAGENTA
#define COLOR_MSG ANSI_WHITE

#define DEBUG_LEVEL_INFO // Default

#ifdef DEBUG_LEVEL_INFO
	#define DEBUG_LEVEL_WARN
	#define DEBUG_LEVEL_ERROR
	#define DEBUG_LEVEL_FATAL
#endif

#ifdef DEBUG_LEVEL_WARN
	#define DEBUG_LEVEL_ERROR
	#define DEBUG_LEVEL_FATAL
#endif

#ifdef DEBUG_LEVEL_ERROR
	#define DEBUG_LEVEL_FATAL
#endif

#ifdef DEBUG_OFF
	#undef DEBUG_LEVEL_INFO
	#undef DEBUG_LEVEL_WARN
	#undef DEBUG_LEVEL_ERROR
	#undef DEBUG_LEVEL_FATAL
#endif

#ifndef DEBUG_OFF
	#define _CRASH()	\
	 	u32 *i = 0x0;	\
		*i = 42;

#define _LOGPRINTF(mode, color, pad, ...)																											\
		fprintf(stderr, "%s%s%s%s%s %s%d %s%s: %s", color, mode, pad, COLOR_FILE, __FILE__, COLOR_LINE, __LINE__, COLOR_FUNC, __func__, COLOR_MSG);	\
		fprintf(stderr, __VA_ARGS__);																												\
		fprintf(stderr, ANSI_RESET);																												\
		fprintf(stderr, "\n");
#endif

#ifdef DEBUG_LEVEL_INFO
	#define INFO(...) do { _LOGPRINTF("INFO", COLOR_INFO, "  ", __VA_ARGS__) } while (0)
#else
	#define INFO(...)
#endif

#ifdef DEBUG_LEVEL_WARN
	#define WARN(...) do { _LOGPRINTF("WARN", COLOR_WARN, "  ", __VA_ARGS__) } while (0)
#else
	#define WARN(...)
#endif

#ifdef DEBUG_LEVEL_ERROR
	#define ERROR(...) do { _LOGPRINTF("ERROR", COLOR_ERROR, " ", __VA_ARGS__) } while (0)
#else
	#define ERROR(...)
#endif

#ifdef DEBUG_LEVEL_FATAL
	#define FATAL(...) do { _LOGPRINTF("FATAL", COLOR_FATAL, " ", __VA_ARGS__) _CRASH() } while (0)
#else
	#define FATAL(...)
#endif

#ifndef DEBUG_OFF
	#define DEBUG(...) do { __LOGPRINTF("DEBUG", ANSI_WHITE, " ", __VA_ARGS__) } while (0)

	#define _ASSERT(x, msg)						\
		if (!(x))								\
		{										\
			FATAL("Assertion failed: %s", msg);	\
		}

	static char buffer[2048];
	#define ASSERT(x, ...) 					\
		do									\
		{									\
			sprintf(buffer, ##__VA_ARGS__);	\
			_ASSERT(x, buffer)				\
		} while (0)
#else
	#define DEBUG(...)
	#define ASSERT(x, ...)
#endif