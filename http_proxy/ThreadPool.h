#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

class ThreadPool {
private:

public:

    // Get the threadpool instance.
    static ThreadPool & getInstance();

    // Do the task;
    void doTheTask();

};

#endif