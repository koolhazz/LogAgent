#ifndef _COLLECT_H_
#define _COLLECT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

typedef enum {
    ENORMAL,
    EEVENT,
    EPERF    
} MsgType_t;

/*enum MsgType {
    ENORMAL,
    EEVENT,
    EPERF    
};
typedef enum MsgType MsgType_t; enum的定义必须前置*/


class Collect {
public:
    Collect();
    ~Collect();
    
    int start();
    void stop();
    
    static void* task(void *args);

    typedef vector<string>  key_t;
    typedef key_t::iterator key_itr_t;
private:
    bool   _run;
    key_t  event_key;
    key_t  perf_key;
private:	
	int open_file(); //开启日志文件
	int uuid_format(char *uuid_out, char *uuid_in);
	int trim_rn(string &in, string &out);

    MsgType_t get_msg_type(string& msg);

    int init_key();
};

#endif
