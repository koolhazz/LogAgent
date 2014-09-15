#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Log {
public:
    Log();
    ~Log();
    void start(string log_file, int level, int flag, int rotate, int64_t max_size);
    void stop();
    void output(char *buf, int cnt);
    void panic(char *fmt, ...);
    void fatal(char *fmt, ...); 
    void error(char *fmt, ...);
    void warn(char *fmt, ...); 
    void info(char *fmt, ...);
    void debug(char *fmt, ...);    

private:
    int					_level;
    int					_fd;
    int					_flag;
    int					_rotate;
    int64_t				_max_size;
    int64_t				_current_size;
	pthread_mutex_t		_mutex;
};

#endif
