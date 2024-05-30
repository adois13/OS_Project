#include "../h/syscall_cpp.hpp"
#include "../h/console.h"
#include "../h/console.hpp"
#include "../test/printing.hpp"

extern void userMain();


void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

void userMainWrapper(void* arg) {
    Semaphore* sem = (Semaphore*)arg;
    userMain();

    // for(int i = 0; i < 5; i++) {
    //     int ret = sem->timedWait(10);
    //     printInt(ret, 10, 1);
    //     _Console::getConsole()->putc('\n');
    // }    
    sem->signal();
}

int main() {

    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB::running = new TCB(nullptr, nullptr, nullptr);
    
    Thread* idleThread = new Thread(idle, nullptr);
    idleThread->start();

    Semaphore* sem = new Semaphore(0);

    Thread* inputThread = new Thread(_Console::input_wrapper, nullptr);
    inputThread->start();

    Thread* outputThread = new Thread(_Console::output_wrapper, nullptr);
    outputThread->start();

    Thread* userThread = new Thread(userMainWrapper, sem);
    userThread->start();
    
    sem->wait();

    delete sem;
    return 0;
}
