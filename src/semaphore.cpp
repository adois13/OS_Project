#include "../h/semaphore.hpp"
#include "../h/scheduler.hpp"


_Semaphore::_Semaphore(unsigned init) : myHandle(nullptr), value(init) 
    {}

_Semaphore::~_Semaphore() { };

List _Semaphore::getBlocked() { return blockedQueue; }

int _Semaphore::signal() {
    if(value++ < 0) {
        TCB* newUnblocked = blockedQueue.removeFirst();
        if(newUnblocked) {
            newUnblocked->setReady(true);
            if(newUnblocked -> getTimeToSleep() > 0) {
                newUnblocked -> setTimeToSleep(0);
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
        thread -> setTimeToSleep(0);
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
    TCB::running -> setSemaphore(this);
    TCB::running -> setTimedWait(true);
    TCB::running -> setTimeToSleep(Riscv::timerTickCounter + timeout);
    Scheduler::putToSleep(TCB::running);

    int ret = wait();
    
    if(!TCB::running -> getTimedWait()) {
        return TIMEOUT;
    } else if(TCB::running -> getClosedSemFlag()) {
        return SEMDEAD;
    } return ret;
}