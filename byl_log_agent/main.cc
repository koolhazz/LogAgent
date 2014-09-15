#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "conf.h"
#include "daemonize.h"
#include "udp.h"
#include "main.h"

using namespace std;

static int parse_arg(int argc, char **argv);
static int init_conf();
static void init_misc();
static void deinit_misc();
static void app_loop();
static void recv_udp_packet(int fd);
static int clear_output_history();

appBase app_base;
Log log;

int main(int argc, char **argv)
{
    int ret;
    ret = parse_arg(argc, argv);
    if (ret < 0) {
        log.fatal("File: %s Func: %s Line: %d => parse_arg.\n",
                            __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
    ret = init_conf();
    if (ret < 0) {
        log.fatal("File: %s Func: %s Line: %d => init_conf.\n",
                            __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
    
    if (app_base.is_daemonize == 1)
        daemonize();
    
    ret = single_instance_running(app_base.pid_file.c_str());
    if (ret < 0) {
        log.fatal("File: %s Func: %s Line: %d => single_instance_running.\n",
                            __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
	
	init_misc();
	
	log.start(app_base.log_file, app_base.level, app_base.consloe, app_base.rotate, app_base.log_max_size);
	
	clear_output_history();
	
	app_base.collect_thread.start(); // 开始采集线程
	app_base.rotate_thread.start();  // 开始分段线程
	
	app_loop();
	
	app_base.collect_thread.stop();
	app_base.rotate_thread.stop();
	
	deinit_misc();
    
    return 0;
}

static int parse_arg(int argc, char **argv)
{
    int flag = 0;
    int oc; /* option chacb. */
    char ic; /* invalid chacb. */
    
    app_base.is_daemonize = 0;
    while((oc = getopt(argc, argv, "Df:")) != -1) {
        switch(oc) {
            case 'D':
                app_base.is_daemonize = 1;
                break;
            case 'f':
                flag = 1;
                app_base.conf_file = string(optarg);
                break;
            case '?':
                ic = (char)optopt;
                printf("invalid \'%c\'\n", ic);
                break;
            case ':':
                printf("lack option arg\n");
                break;
        }
    }
    
    if (flag == 0)
        return -1;
        
    return 0;
}

static int init_conf()
{
    map<string, string> result;
    
    int ret = Conf::parse(app_base.conf_file, result);
    if (ret < 0)
        return -1;
    
    for (map<string, string>::iterator iter = result.begin(); iter != result.end(); ++iter) {
        cout << iter->first << "<==>" << iter->second << endl;
    }
    
    // app_base.is_daemonize;
    app_base.log_file = result["log_file"];
    app_base.level = atoi(result["level"].c_str());
    app_base.consloe = atoi(result["consloe"].c_str());
    app_base.rotate = atoi(result["rotate"].c_str());
    app_base.log_max_size = atol(result["log_max_size"].c_str());
    app_base.pid_file = result["pid_file"];
	
    app_base.unique = result["unique"];
    app_base.host = result["host"];
    app_base.port = atoi(result["port"].c_str());
    app_base.freq = atoi(result["freq"].c_str());
    app_base.output = result["output"];
	
	app_base.gz_handle = NULL;
    
    return 0;
}

static void init_misc()
{
	pthread_mutex_init(&app_base.mutex, NULL);
}

static void deinit_misc()
{
	pthread_mutex_destroy(&app_base.mutex);
}

static void app_loop()
{
	int fd;
	
	fd = bind_udp(app_base.host, app_base.port);
    if (fd < 0) {
        log.fatal("File: %s Func: %s Line: %d => bind_udp.\n",
                            __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
	
	recv_udp_packet(fd);
	
	close(fd);
}

static void recv_udp_packet(int fd)
{
	int n;
	char buf[2048];
	socklen_t len;
	struct sockaddr_in client_addr;
	string msg;
	
	while (true) {
		len = sizeof(client_addr);
		n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &len);
		//sendto(fd, buf, n, 0, (struct sockaddr*)&client_addr, len);
		if (n < 0) {
			log.error("File[%s] Line[%d]: recvfrom failed: %s\n",
							__FILE__, __LINE__, strerror(errno));
		}
		buf[n] = '\0';

        log.debug("MSG: %s\n", buf);

		msg.clear();
		msg.append(buf);
		app_base.msg_queue.put_msg(msg);
		//printf("[[[%s]]]\n", buf);
	}
}

static int clear_output_history()
{
    struct dirent *pentry;
    DIR *dir;
	struct stat64 st;
	string d_name;
    string src_name;
    string dst_name;
    
    dir = opendir(app_base.output.c_str());
    if (dir == NULL)
        return (-1);
    
    while ((pentry = readdir(dir)) != NULL) {
        src_name.clear();
        src_name.append(app_base.output);
        src_name.append("/");
        src_name.append(pentry->d_name);
		
        if (stat64(src_name.c_str(), &st) < 0) {
            log.error("File: %s Func: %s Line: %d => stat64 %s.\n",
                                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            closedir(dir);
            return -1;
        }
		
		// dir ignore
		if (!(st.st_mode & S_IFREG)) {
			continue;
		}
        if (strncmp(pentry->d_name, ".", 1) == 0) {
			dst_name.clear();
			dst_name.append(app_base.output);
			dst_name.append("/");
			dst_name.append(&pentry->d_name[1]);
			rename(src_name.c_str(), dst_name.c_str());
			log.info("clear old file src_name[%s] dst_name[%s]\n", src_name.c_str(), dst_name.c_str());
		}
    }
    
    closedir(dir);
    
    return 0;
}
