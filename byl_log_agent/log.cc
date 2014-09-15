#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "log.h"

enum LOG_TYPE {
    PANIC_TYPE = 0,
    FATAL_TYPE,
    ERROR_TYPE,
    WARN_TYPE,
    INFO_TYPE,
    DEBUG_TYPE,
};

char *log_type_str[] = {"panic", "fatal", "error", "warn ", "info ", "debug"};

#define COMMON_LOG(log_type) \
	char buf[8192]; \
	time_t t;\
    struct tm gt;\
    struct tm* p;\
    int len;\
    int cnt;\
	time(&t);\
    p = localtime_r(&t, &gt);\
    if(!p)\
        p= &gt;\
	len = sprintf(buf, "[%s][%.4d-%.2d-%.2d %.2d:%.2d:%.2d] ",\
                log_type_str[log_type],\
                p->tm_year + 1900,\
                p->tm_mon + 1,\
                p->tm_mday,\
                p->tm_hour,\
                p->tm_min,\
                p->tm_sec);\
	va_list argptr;\
	va_start(argptr, fmt);\
    cnt = vsprintf(buf + len, fmt, argptr);\
	va_end(argptr);\
    cnt += len;\

Log::Log()
{
    _level = 1;
    _fd = -1;
    _flag = 0;
    // default 1G
    _max_size = 1073741824;
    _current_size = 0;
    _rotate = 0;
	pthread_mutex_init(&_mutex, NULL);
}

Log::~Log()
{
	pthread_mutex_destroy(&_mutex);
}

void Log::start(string log_file, int level, int flag, int rotate, int64_t max_size)
{
    struct stat64 st;
    
    _fd = open(log_file.c_str(), O_RDWR | O_APPEND | O_CREAT | O_LARGEFILE, 0644);
    if (_fd < 0) {
        perror("LOG~open");
        exit(1);
    }
    
    if (fstat64(_fd, &st) < 0) {
        perror("LOG~stat64");
        exit(1);
    }
    
    _level = level;
    _flag = flag;
    _rotate = rotate;
    if (max_size > 0) 
        _max_size = max_size;
    _current_size = st.st_size;
}

void Log::stop()
{
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}

void Log::output(char *buf, int cnt)
{
    if (_flag == 0) {
        printf("%s", buf);
		return;
	}
	
	pthread_mutex_lock(&_mutex); 
    if (_fd > 0) {
        if (_rotate == 1) {
            if (_current_size > _max_size) {
                ftruncate(_fd, 0);
                _current_size = 0;
            }
        }
        int ret = write(_fd, buf, cnt);
        if (ret < 0) {
            perror("Log~write");
        }
        _current_size += cnt;
    }
	pthread_mutex_unlock(&_mutex); 
}

void Log::panic(char *fmt, ...)
{
    int log_type = PANIC_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
    
    exit(1);
}

void Log::fatal(char *fmt, ...)
{
    int log_type = FATAL_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
}

void Log::error(char *fmt, ...)
{
    int log_type = ERROR_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
}

void Log::warn(char *fmt, ...)
{
    int log_type = WARN_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
}

void Log::info(char *fmt, ...)
{
    int log_type = INFO_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
}

void Log::debug(char *fmt, ...)
{
    if (_level == 0)
        return;
    int log_type = DEBUG_TYPE;
    COMMON_LOG(log_type)
    
	output(buf, cnt);
}

#ifdef LOG_DEBUG
    int main()
    {
        Log log;
        log.warn("%s\n", "if is good.");
        log.start("a.log", 0, 1, 1, 500);
        log.fatal("%s\n", "if is good.");
        log.error("%s\n", "if is good.");
        log.warn("%s\n", "if is good.");
        log.info("%s\n", "if is good.");
        log.debug("%s\n", "if is good.");
        log.panic("%s\n", "if is good.");
        log.stop();
        
        return 0;
    }
#endif
