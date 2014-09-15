#include "main.h"
#include "log.h"
#include "rotate.h"

extern appBase app_base;
extern Log log;

Rotate::Rotate()
{

}

Rotate::~Rotate()
{

}

int Rotate::start()
{
    int retval;
    pthread_t tid;
    pthread_attr_t attr;
    
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

void Rotate::stop()
{
	string data = "QUIT";
	app_base.msg_queue.put_msg(data);
}

void* Rotate::task(void *args)
{
    // Rotate* self = (Rotate*)args;
    
    pthread_detach(pthread_self());
    while (true) {
		sleep(app_base.freq);
		pthread_mutex_lock(&app_base.mutex); 
		// rotate to file.
		if (app_base.gz_handle) {
			gzclose(app_base.gz_handle);
			app_base.gz_handle = NULL;
			rename(app_base.tmp_dir.c_str(), app_base.dst_dir.c_str());
		}

        if (app_base.gz_handle_2) {
            gzclose(app_base.gz_handle_2);
            app_base.gz_handle_2 = NULL;
            rename(app_base.tmp_dir_2.c_str(), app_base.dst_dir_2.c_str());
        }

        if (app_base.gz_handle_3) {
            gzclose(app_base.gz_handle_3);
            app_base.gz_handle_3 = NULL;
            rename(app_base.tmp_dir_3.c_str(), app_base.dst_dir_3.c_str());
        }

        pthread_mutex_unlock(&app_base.mutex);
    }
    pthread_exit((void*)0);
}
