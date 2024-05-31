#include "../h/TCB.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"
#include "../h/syscall_c.hpp"
#include "../h/memoryAllocator.hpp"

TCB* TCB::running;

TCB::TCB(Body body, void* arg, char* stack) : 
            body(body),
            arg(arg),
            stack(stack), 
            context({
                    (uint64) &threadWrapper,
                    stack != nullptr ? (uint64) &((char*)stack)[DEFAULT_STACK_SIZE] : 0
                    }), 
            timeSlice(DEFAULT_TIME_SLICE),
            finished(false),
            ready(true),
            timeSliceCounter(0),
            handle(nullptr)
    {}

void TCB::dispatch()
{
    TCB* oldRunning = running;
    MemoryAllocator *allocator = MemoryAllocator::getMemoryAllocator();

    if(oldRunning -> isFinished()){
       allocator -> _free(oldRunning->stack);
       allocator -> _free(oldRunning);
    }
    else if(oldRunning -> isReady()) { 
        Scheduler::put(oldRunning); 
    }

    running = Scheduler::get();
    if(oldRunning != running) {
        TCB::contextSwitch(&oldRunning->context, &running->context);
    }
}

void TCB::threadWrapper() {
    Riscv::newRunningSppSpie();
    running -> body(running->arg);
    thread_exit();
}