#ifndef _semaphore_hpp_
#define _semaphore_hpp

#include "../h/syscall_c.hpp"
#include "../h/hw.h"
#include "../h/list.hpp"
#include "../h/TCB.hpp"
#include "../h/riscv.hpp"

class _Semaphore {

public:
    _Semaphore(unsigned init);
    ~_Semaphore();
    
    int close();
    int signal();
    int wait();

    int tryWait();
    int timedWait(time_t timeout);

private:
    sem_t myHandle;
    int value;

    List blockedQueue;

    enum retValue {
        SEMDEAD = -1,
        TIMEOUT = -2
    };
};

#endif