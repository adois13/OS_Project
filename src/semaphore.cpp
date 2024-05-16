#include "../h/semaphore.hpp"
#include "../h/scheduler.hpp"

_Semaphore::_Semaphore(unsigned init) : myHandle(nullptr), value(init) 
    {}

_Semaphore::~_Semaphore() { };

int _Semaphore::signal() {
    if(value++ < 0) {
        TCB* newUnblocked = blockedQueue.removeFirst();
        if(newUnblocked) {
            newUnblocked->setReady(true);
            Scheduler::put(newUnblocked);
        }
    }

    return 0;
}

int _Semaphore::wait() {

    if(--value < 0) {
        TCB::running -> setReady(false);
        blockedQueue.addLast(TCB::running);
        TCB::running -> dispatch();
    }
    // return value is not 0 if semaphore is closed before sem.signal() (regular unblocking)
    if (TCB::running -> getClosedSemFlag()) {
       TCB::running -> setClosedSemFlag(false);
       return -1;
    }
    return 0;
}

int _Semaphore::close() {
    TCB* thread = blockedQueue.removeFirst();
    while(thread) {
        thread -> setReady(true);
        thread -> setClosedSemFlag(true);
        Scheduler::put(thread);
        thread = blockedQueue.removeFirst();
    }
    myHandle = nullptr;
    value = 0;
    return 0;
}

int _Semaphore::tryWait() {
    if(--value > 0) {
        return 1;
    }
    return 0;
}

int _Semaphore::timedWait(time_t timeout) {
    TCB::running -> setTimeToSleep(Riscv::timerTickCounter + timeout);
    int ret = wait();
    if(ret == -1) {
        return SEMDEAD;
    } else if(TCB::running -> getTimeToSleep() >= Riscv::timerTickCounter) {
        return TIMEOUT;
    }
    return 0;
}