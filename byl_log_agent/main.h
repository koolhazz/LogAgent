#ifndef _MAIN_H_
#define _MAIN_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
//#include <semaphore.h>
#include <sys/time.h>
#include <zlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "log.h"
#include "queue.h"
#include "collect.h"
#include "rotate.h"

using namespace std;

class appBase {
public:
    string 				conf_file;
    int 				is_daemonize;
    string 				log_file;
    int 				level;
    int 				consloe;
    int 				rotate;
    uint64_t 			log_max_size;
    string 				pid_file;
	
	string 				unique;
	string 				host;
	int 				port;
	int 				freq;
	string 				output;
	
	Collect 			collect_thread;
	Rotate 				rotate_thread;
	
	Message_Queue<string> msg_queue;
	gzFile 				gz_handle;
	string 				tmp_dir;
	string 				dst_dir;

	gzFile				gz_handle_2;
	string 				tmp_dir_2;
	string				dst_dir_2;

	gzFile				gz_handle_3;
	string				tmp_dir_3;
	string				dst_dir_3;

    pthread_mutex_t 	mutex;
};

#endif    /* _CPPBASE_H_ */
