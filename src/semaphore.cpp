#include "../h/semaphore.hpp"
#include "../h/scheduler.hpp"

_Semaphore::_Semaphore(unsigned init) : value(init) 
    {}

_Semaphore::~_Semaphore() { close(); };

List* _Semaphore::getBlocked() { return &blockedQueue; }

int _Semaphore::signal() {
    if(value++ < 0) {
        TCB* newUnblocked = blockedQueue.removeFirst();
        if(newUnblocked) {
            newUnblocked->setReady(true);
            if(newUnblocked->getStatus() == TIMED_WAIT) {
                newUnblocked->setStatus(UNBLOCKED_ON_SIGNAL);
                newUnblocked->setTimeToSleep(0);
                Scheduler::sleepingQueue.remove(newUnblocked);
            }
            else {
                newUnblocked->setStatus(REGULAR);
            }
            Scheduler::put(newUnblocked);
        }
    }
    return 0;
}

int _Semaphore::wait() {

    if(--value < 0) {
        TCB::running -> setReady(false);
        blockedQueue.addLast(TCB::running);
        TCB::running -> setStatus(BLOCKED_ON_SEM);
        TCB::running -> dispatch();
    }
    
    if (TCB::running -> getStatus() == SEM_CLOSED) {
       TCB::running -> setStatus(REGULAR);
       return -1;
    }
    return 0;
}

int _Semaphore::close() {
    TCB* thread = blockedQueue.removeFirst();
    while(thread) {
        thread -> setReady(true);
        thread -> setStatus(SEM_CLOSED);
        thread -> setTimeToSleep(0);
        Scheduler::put(thread);
        thread = blockedQueue.removeFirst();
    }
    value = 0;
    return 0;
}

int _Semaphore::tryWait() {
    if(value <= 0) {
        return 1;
    }
    value--;
    return 0;
}

int _Semaphore::timedWait(time_t timeout) {

    if (--value < 0)
    {
        TCB::running -> setSemaphore(this);
        TCB::running -> setStatus(TIMED_WAIT);
        TCB::running -> setTimeToSleep(Riscv::timerTickCounter + timeout);
        Scheduler::putToSleep(TCB::running);
        TCB::running -> setReady(false);
        blockedQueue.addLast(TCB::running);
        TCB::running -> dispatch();

        if(TCB::running -> getStatus() == TIMEOUT) {
            TCB::running->setStatus(REGULAR);
            return -2;
        }
        if (TCB::running -> getStatus() == SEM_CLOSED) {
            TCB::running -> setStatus(REGULAR);
            return -1;
        }
        if(TCB::running -> getStatus() == UNBLOCKED_ON_SIGNAL) {
            TCB::running->setStatus(REGULAR);
            return 0;
        }
    }
    return 0;
}