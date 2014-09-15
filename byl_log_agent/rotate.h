#ifndef _ROTATE_H_
#define _ROTATE_H_

#include <iostream>
#include <fstream>
#include <string>

#include <pthread.h>

using namespace std;

class Rotate {
public:
    Rotate();
    ~Rotate();
    
    int start();
    void stop();
    
    static void* task(void *args);
    
    
private:
    bool    _run;
};

#endif
