#include "../h/scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/semaphore.hpp"

Scheduler* Scheduler::getScheduler() {
    static Scheduler scheduler;
    return &scheduler;
}

List Scheduler::readyQueue;
List Scheduler::sleepingQueue;

void Scheduler::put(TCB* tcb) {
    Scheduler::readyQueue.addLast(tcb);
    tcb->setReady(true);
}

TCB* Scheduler::get() {
    return readyQueue.removeFirst();
}

void Scheduler::putToSleep(TCB* thread) {
    sleepingQueue.insertSorted(thread);
}

void Scheduler::sleepingThreadsHandler() {
    TCB* sleepingThread = sleepingQueue.peekFirst();

    for(; sleepingThread && sleepingThread -> getTimeToSleep() <= Riscv::timerTickCounter; ) {
        sleepingThread -> setTimeToSleep(0);
        sleepingThread -> setReady(true);
        if(sleepingThread -> getStatus() == TIMED_WAIT) { 
            sleepingThread -> setStatus(TIMEOUT);
            sleepingThread -> getSemaphore() -> getBlocked() -> remove(sleepingThread);
            sleepingThread -> setSemaphore(nullptr);
        }
        else {
            sleepingThread->setStatus(REGULAR);
        }
        Scheduler::put(sleepingThread);
        sleepingThread = sleepingQueue.removeFirst();
        sleepingThread = sleepingQueue.peekFirst();
    }
}