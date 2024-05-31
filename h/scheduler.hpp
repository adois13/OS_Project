#ifndef _scheduler_hpp_
#define _scheduler_hpp_

#include "../h/list.hpp"
#include "../h/TCB.hpp"

class Scheduler {

friend class _Semaphore;

private:
    Scheduler(){}

    static List readyQueue;
    static List sleepingQueue;

public:
    Scheduler(const Scheduler&) = delete;
    
    static Scheduler* getScheduler();

    static void put(TCB*);
    static TCB* get();

    static void putToSleep(TCB*);

    static void sleepingThreadsHandler();

};

#endif