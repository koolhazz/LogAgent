#include <uuid/uuid.h>

#include "main.h"
#include "log.h"
#include "collect.h"

extern appBase app_base;
extern Log log;

#define UUID_STR_LEN 128

#define DLMTER_2 "ENT" /* 事件 */
#define DLMTER_3 "PER" /* 性能 */

Collect::Collect()
{

}

Collect::~Collect()
{

}

int Collect::start()
{
    int retval;
    pthread_t tid;
    pthread_attr_t attr;
    
    init_key();

    _run = true;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
    retval = pthread_create(&tid, &attr, task, (void*)this);
    if (retval != 0) {
		log.error("File[%s] Line[%d]: pthread_create failed: %s\n",
						__FILE__, __LINE__, strerror(errno));
        return -1;
    }
    // pthread_detach(tid);
    
    return 0;
}

void Collect::stop()
{
	string data = "QUIT";
	app_base.msg_queue.put_msg(data);
}

void* Collect::task(void *args)
{
    Collect* self = (Collect*)args;
	self->_run = true;
	
	string msg;
	string result;
	//string history;
	int ret;
    pthread_detach(pthread_self());
    while (true) {
		app_base.msg_queue.get_msg(msg);
		if (msg.compare("QUIT") == 0) {
			self->_run = false;
			break;
        }
		
		self->trim_rn(msg, result);

		// check duplicate log
		// if (result.compare(history) == 0) {
		// 	log.error("File[%s] Line[%d]: duplicate log: %s\n",
		// 					__FILE__, __LINE__, result.c_str());
		// }
		// history = result;
		
		result.append("\r\n");

		pthread_mutex_lock(&app_base.mutex); 
		
/*        self->open_file();
		ret = gzwrite(app_base.gz_handle, result.c_str(), result.length());
		if (ret <= 0 ) {
			log.error("File[%s] Line[%d]: gzwrite failed: %s\n",
							__FILE__, __LINE__, strerror(errno));
		}*/
        log.debug("Result: %s\n", result.c_str());

        MsgType_t mt = self->get_msg_type(result);

        log.debug("MsgType: %d\n", mt);

        switch (mt) {
            case ENORMAL:
                self->open_file();
                ret = gzwrite(app_base.gz_handle, result.c_str(), result.length());
                if (ret <= 0 ) {
                    log.error("File[%s] Line[%d]: gzwrite failed: %s\n",
                                    __FILE__, __LINE__, strerror(errno));
                } 

                log.debug("ret: %d\n", ret);
                break;
            case EEVENT: 
                self->open_file();
                ret = gzwrite(app_base.gz_handle_2, result.c_str(), result.length());
                if (ret <= 0 ) {
                    log.error("File[%s] Line[%d]: gzwrite failed: %s\n",
                                    __FILE__, __LINE__, strerror(errno));
                }
                log.debug("ret_2: %d\n", ret);  
                break;
            case EPERF:
                self->open_file();
                ret = gzwrite(app_base.gz_handle_3, result.c_str(), result.length());
                if (ret <= 0 ) {
                    log.error("File[%s] Line[%d]: gzwrite failed: %s\n",
                                    __FILE__, __LINE__, strerror(errno));
                }
                log.debug("ret_3: %d\n", ret);  
                break;
            default:
                break;
        }

        pthread_mutex_unlock(&app_base.mutex);
    }

    pthread_exit((void*)0);
}

int Collect::trim_rn(string &in, string &out)
{
	char c;
	const char *buf = in.c_str();
	
	out.clear();

	for (unsigned int i = 0; i < in.length(); i++) {
		c = *buf++;
		if (c == '\r' || c == '\n') {
			continue;
		}
		out.append(1, c);
	}

	return 0;
}

int Collect::open_file()
{
	uuid_t id;
	char tmp[UUID_STR_LEN];
    char buf[UUID_STR_LEN];
	
	if (app_base.gz_handle == NULL) {
		uuid_generate(id);
		uuid_unparse(id, tmp);
		uuid_format(buf, tmp);
		snprintf(tmp, sizeof(tmp), "%u", (unsigned int)time(NULL));
		
		app_base.tmp_dir.clear();
		app_base.tmp_dir.append(app_base.output);
		app_base.tmp_dir.append("/.");
		app_base.tmp_dir.append(app_base.unique);
		app_base.tmp_dir.append("_");
		app_base.tmp_dir.append(buf);
		app_base.tmp_dir.append("_");
		app_base.tmp_dir.append(tmp);
		app_base.tmp_dir.append(".gz");
		
		app_base.dst_dir.clear();
		app_base.dst_dir.append(app_base.output);
		app_base.dst_dir.append("/");
		app_base.dst_dir.append(app_base.unique);
		app_base.dst_dir.append("_");
		app_base.dst_dir.append(buf);
		app_base.dst_dir.append("_");
		app_base.dst_dir.append(tmp);
		app_base.dst_dir.append(".gz");

        app_base.gz_handle = gzopen(app_base.tmp_dir.c_str(), "ab");
        if (app_base.gz_handle == NULL) {
			log.error("File[%s] Line[%d]: gz_handle gzopen failed: %s\n",
							__FILE__, __LINE__, strerror(errno));
			exit(1);			
            // return -1;
        }
    }

    if (app_base.gz_handle_2 == NULL) {
        uuid_generate(id);
        uuid_unparse(id, tmp);
        uuid_format(buf, tmp);
        snprintf(tmp, sizeof(tmp), "%u", (unsigned int)time(NULL));

        app_base.tmp_dir_2.clear();
        app_base.tmp_dir_2.append(app_base.output);
        app_base.tmp_dir_2.append("/.");
        app_base.tmp_dir_2.append(app_base.unique);
        app_base.tmp_dir_2.append("_");
        app_base.tmp_dir_2.append(buf);
        app_base.tmp_dir_2.append("_");
        app_base.tmp_dir_2.append(tmp);
        app_base.tmp_dir_2.append("_");
        app_base.tmp_dir_2.append(DLMTER_2);
        app_base.tmp_dir_2.append(".gz");

        app_base.dst_dir_2.clear();
        app_base.dst_dir_2.append(app_base.output);
        app_base.dst_dir_2.append("/");
        app_base.dst_dir_2.append(app_base.unique);
        app_base.dst_dir_2.append("_");
        app_base.dst_dir_2.append(buf);
        app_base.dst_dir_2.append("_");
        app_base.dst_dir_2.append(tmp);
        app_base.dst_dir_2.append("_");
        app_base.dst_dir_2.append(DLMTER_2);
        app_base.dst_dir_2.append(".gz");

        app_base.gz_handle_2 = gzopen(app_base.tmp_dir_2.c_str(), "ab");
        if (app_base.gz_handle_2 == NULL) {
            log.error("File[%s] Line[%d]: gz_handle_2 gzopen failed: %s\n",
                            __FILE__, __LINE__, strerror(errno));
            exit(1);    
        }
    } 

    if (app_base.gz_handle_3 == NULL) {
        uuid_generate(id);
        uuid_unparse(id, tmp);
        uuid_format(buf, tmp);
        snprintf(tmp, sizeof(tmp), "%u", (unsigned int)time(NULL));

        app_base.tmp_dir_3.clear();
        app_base.tmp_dir_3.append(app_base.output);
        app_base.tmp_dir_3.append("/.");
        app_base.tmp_dir_3.append(app_base.unique);
        app_base.tmp_dir_3.append("_");
        app_base.tmp_dir_3.append(buf);
        app_base.tmp_dir_3.append("_");
        app_base.tmp_dir_3.append(tmp);
        app_base.tmp_dir_3.append("_");
        app_base.tmp_dir_3.append(DLMTER_3);
        app_base.tmp_dir_3.append(".gz");

        app_base.dst_dir_3.clear();
        app_base.dst_dir_3.append(app_base.output);
        app_base.dst_dir_3.append("/");
        app_base.dst_dir_3.append(app_base.unique);
        app_base.dst_dir_3.append("_");
        app_base.dst_dir_3.append(buf);
        app_base.dst_dir_3.append("_");
        app_base.dst_dir_3.append(tmp);
        app_base.dst_dir_3.append("_");
        app_base.dst_dir_3.append(DLMTER_3);
        app_base.dst_dir_3.append(".gz");

        app_base.gz_handle_3 = gzopen(app_base.tmp_dir_3.c_str(), "ab");
        if (app_base.gz_handle_3 == NULL) {
            log.error("File[%s] Line[%d]: gz_handle_3 gzopen failed: %s\n",
                            __FILE__, __LINE__, strerror(errno));
            exit(1);    
        }               
    }

	return 0;
}

int Collect::uuid_format(char *uuid_out, char *uuid_in)
{
    char *len;
    char *p;
    char *start = NULL;
    char ch;
    enum {
        sw_start = 0,
        sw_dash_first,
        sw_dash_second,
        sw_dash_third,
        sw_dash_fourth,
        sw_end,
    } state;
    
    state = sw_start;
    memset(uuid_out, 0, UUID_STR_LEN);
    len = uuid_in + UUID_STR_LEN;
    
    for (p = uuid_in; (p != NULL && p < len); p++) {
        ch = *p;
        switch (state) {
        case sw_start:
            start = p;
            state = sw_dash_first;
            break;
        case sw_dash_first:
            if (ch == '-') {
                *p = '\0';
                strcat(uuid_out, start);
                start = p + 1;
                state = sw_dash_second;
            }
            break;
        case sw_dash_second:
            if (ch == '-') {
                *p = '\0';
                strcat(uuid_out, start);
                start = p + 1;
                state = sw_dash_third;
            }
            break;
        case sw_dash_third:
            if (ch == '-') {
                *p = '\0';
                strcat(uuid_out, start);
                start = p + 1;
                state = sw_dash_fourth;
            }
            break;
        case sw_dash_fourth:
            if (ch == '-') {
                *p = '\0';
                strcat(uuid_out, start);
                start = p + 1;
                state = sw_end;
            }
            break;
        case sw_end:
            if (ch == '\0') {
                strcat(uuid_out, start);
                return 0;
            }
            break;
        default:
            return -1;
        }
    }
    return -1;
}

MsgType_t 
Collect::get_msg_type(string& msg)
{
    key_itr_t           it;
    string::size_type   ret = 0;

    for ( it = event_key.begin(); it != event_key.end(); it++) {
        log.debug("*it: %s\n", it->c_str());

        ret = msg.find(*it);
        log.debug("ret: %d\n", ret);

        if (ret != string::npos) {
            return EEVENT;
        }
    }

    for ( it = perf_key.begin(); it != perf_key.end(); it++) {
        log.debug("*it: %s\n", it->c_str());

        ret = msg.find(*it);
        log.debug("ret: %d\n", ret);
        
        if (ret != string::npos) {
            return EPERF;
        }
    }     

    return ENORMAL;
}

int
Collect::init_key()
{
    event_key.push_back("by_event");

    perf_key.push_back("pf_");
    perf_key.push_back("user_plat_auth_cb");
    perf_key.push_back("game_loading");
    perf_key.push_back("newbie_guide");
    perf_key.push_back("feed_loading");
    perf_key.push_back("room_list_loading");
    perf_key.push_back("room_enter_loading");
    perf_key.push_back("room_reloading");

    return 0;
}
