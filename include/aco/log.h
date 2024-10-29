/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <generated/autoconf.h>

#define LOG_VERSION "0.1.0"

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#ifdef  CONFIG_LOG_MODULE

/*  Name : log_module_init
 *  Desc : init log system
 *  Pram :
 *  Note : it should be called before many system in the system init,
 *         because many system depends on it
 *
 */
int log_module_init(void);

/*  Name : log_log
 *  Desc : log the 'fmt' information into log_system
 *  Pram :
 *  	   level : log-level (if level < built-in level in log system,
 *  	   the fmt will not be printed)
 *  	   file  : file name where we want to call log_log
 *  	   line  : line number where we want to call log_log
 *  	   fmt   : log message, it is formated
 *  	   ...   : 
 *  Usage :
 *  void a(void)
 *  {
 *       log_log(LOG_TRACE, __FILE__, __LINE__, "just a test ! %d %x\n",13,0x23);
 *  }
 *
 */
void log_log(int level, const char *file, int line, const char *fmt, ...);



/*
 * these functions are not thread-safe
 * be careful!
 */

/*  Name : log_level_string
 *  Desc : get the string format for the 'level'
 *  	   trans a enum to a string
 *  Pram :
 *         level : which level's string format want to get
 *  Usage :
 *  void a(void)
 *  {
 *       log_level_string(LOG_TRACE);
 *	 //this will return "TRACE" str
 *
 *  }
 */
const char* log_level_string(int level);

/*  Name : log_set_lock
 *  Desc : set log lock callback
 *  Pram :
 *         fn : lock callback func 
 *         udata : extended udata for callback
 *  Note : nerver use it unless you need it
 *         this will be called in the system init
 *         to make sure a thread-safety for log system
 */
void log_set_lock(log_LockFn fn, void *udata);

/*  Name : log_set_level
 *  Desc : set built-in log_level in the log system
 *  Pram :
 *         level : the level you want to set
 *  Note : this function is not thread-safety!
 */
void log_set_level(int level);

/*  Name : log_set_quiet
 *  Desc : disable log system stdoutput
 *    	   but all information will also logged
 *  Pram :
 *         enable : if log_system quiet
 *  Note : this function is not thread-safety!
 */
void log_set_quiet(bool enable);

/*  Name : log_add_callback
 *  Desc : add a callback function for log system
 *    	   this is helpful to extend log system
 *    	   means our log information will not only have
 *    	   one output-stream, we can add our way to log the information
 *  Pram :
 *         fn : the callback how to log information
 *         udata : user data for extended
 *         level : this callback gate limit, all information with level < 'level'
 *         will not call callback-func to log in
 *  Note : this function is not thread-safety! and never call this
 *         unless you need it, it will be called in the log_system_init
 *         to register all output-stram!
 */
int log_add_callback(log_LogFn fn, void *udata, int level);

#else  /* not CONFIG_LOG_MODULE */

static inline void log_log(int level, const char *file, int line, const char *fmt, ...)
{
	return;
}

static inline const char* log_level_string(int level)
{
	return NULL;
}

static inline void log_set_lock(log_LockFn fn, void *udata)
{
	return;
}

static inline void log_set_level(int level)
{
	return;
}

static inline void log_set_quiet(bool enable)
{
	return;
}

static inline int log_add_callback(log_LogFn fn, void *udata, int level)
{
	return 0;
}

static inline int log_module_init(void)
{
	return 0;
}

#endif /* CONFIG_LOG_MODULE END */


#endif
