#ifndef _semaphore_hpp_
#define _semaphore_hpp

#include "../h/syscall_c.hpp"
#include "../h/list.hpp"

class _Semaphore {

public:
    _Semaphore(unsigned init);
    ~_Semaphore();
    
    int close();
    int signal();
    int wait();

    int tryWait();
    int timedWait(time_t timeout);

    List* getBlocked();

private:
    int value;
    List blockedQueue;
};

#endif