//
// Created by yue on 18-1-26.
//

#ifndef YUEWEBSERVER_POOL_H
#define YUEWEBSERVER_POOL_H

#include "lock.h"
#include "cerrno"
#include "cstdio"
#include "queue"
#include "exception"
#include "iostream"
#include "pthread.h"

template<class T>
class pool{
private:
    int threadnum;
    pthread_t *threads;
    mutexLocker queue_mutex;
    condLocker queue_cond;
    bool isend;
    std::queue<T *> LwpQueue;

public:
    pool(int threadnumber = 40);
    ~pool();
    bool append(T *lwp);
    void start();
    void stop();

private:
    static void *startPool(void *args);
    void run();
    T *getLwp();
};

template<class T>
pool<T>::pool(int threadnumber):
    threadnum(threadnumber),isend(false),threads(NULL){
    threads = new pthread_t[threadnum];
    if(threads==NULL) printf("线程池启动失败");
}

template<class T>
pool<T>::~pool() { delete[] threads; stop(); }

template<class T>
bool pool<T>::append(T *task) {
    queue_mutex.mutexLock();
    bool signal = LwpQueue.empty();
    LwpQueue.push(task);
    queue_mutex.mutexUnlock();
    if(signal) queue_cond.signal();
    return true;
}


template<class T>
void pool<T>::stop() {
    isend=true;
    queue_cond.broadcast(); //解除阻塞所有线程
}

template<class T>
void pool<T>::run() {
    while(!isend){
        T* lwp = getLwp();
        if(lwp == NULL){
            queue_cond.wait();
        } else{
            lwp->Read();
            delete(lwp);
        }
    }
}

template<class T>
void* pool<T>::startPool(void *args) {
    pool *pool1 = (pool*)args;
    pool1->run();
    return pool1;
}


template<class T>
void pool<T>::start() {
    for(int i=0;i<threadnum;i++){
        if(pthread_create(threads+i,NULL,startPool,this)!=0){
            delete[] threads;
            printf("创建线程失败");
            throw std::exception();
        }
        if(pthread_detach(threads[i])){
            delete[] threads;
            printf("创建线程失败!");
            throw std::exception();
        }
    }
}

template<class T>
T* pool<T>::getLwp() {
    T *lwp = NULL;
    queue_mutex.mutexLock();
    if(!LwpQueue.empty()){
        lwp = LwpQueue.front();
        LwpQueue.pop();
    }
    queue_mutex.mutexUnlock();
    return lwp;
}


#endif //YUEWEBSERVER_POOL_H
