//
// Created by yue on 18-1-26.
//

#ifndef YUEWEBSERVER_LOCK_H
#define YUEWEBSERVER_LOCK_H

#include "pthread.h"
#include "cstdio"
#include "semaphore.h" //信息量的头文件

//信息量
class semlocker{
private:
    sem_t sem;

public:
    semlocker(){
        if(sem_init(&sem,0,0)){
            printf("sem error\n");
        }
    }

    ~semlocker(){sem_destroy(&sem); }

    bool add(){ return sem_post(&sem)==0; } //添加信号量

    bool wait(){ return sem_wait(&sem)==0; } //等待信号


};

//互斥锁
class mutexLocker{
private:
    pthread_mutex_t mutex;

public:
    mutexLocker(){
        if(pthread_mutex_init(&mutex,NULL)){
            printf("mutex init failed");
        }
    }

    ~mutexLocker(){ pthread_mutex_destroy(&mutex); }

    bool mutexLock(){ return pthread_mutex_lock(&mutex)==0; }

    bool mutexUnlock(){ return pthread_mutex_unlock(&mutex)==0; }

};

//条件变量
class condLocker{
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
public:
    condLocker(){
        if(pthread_mutex_init(&mutex, NULL)) printf("mutex init failed");
        if(pthread_cond_init(&cond,NULL)){  pthread_mutex_destroy(&mutex); printf("cond init failed");}
    }
    ~condLocker(){
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
    int wait(){
        int ans = 0;
        pthread_mutex_lock(&mutex);
        ans = pthread_cond_wait(&cond,&mutex);
        pthread_mutex_unlock(&mutex);
        return ans==0;
    }
    bool signal(){
        return pthread_cond_signal(&cond)==0;
    }

    bool broadcast(){
        return pthread_cond_broadcast(&cond)==0;
    }

};

#endif //YUEWEBSERVER_LOCK_H
