#include "../h/memoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/console.hpp"



void Riscv::newRunningSppSpie() {
    __asm__ volatile ("csrc sstatus, %0": : "r"(SSTATUS_SPP));
    __asm__ volatile ("csrw sepc, ra");
    __asm__ ("sret");
}

time_t Riscv::timerTickCounter = 0;

void Riscv::handleSupervisorTrap() {

    uint64 scause = Riscv::r_scause();
    volatile uint64 sepc = Riscv::r_sepc();
    volatile uint64 sstatus = Riscv::r_sstatus();

    if(scause == 0x8000000000000001UL) {
        // supervisor software interrupt (timer)
        // clear software interrupt pending
        // check if there is sleeping thread that is ready to continue
        mc_sip(SIP_SSIP);
        Riscv::timerTickCounter++;
        Scheduler::sleepingThreadsHandler();

        // timedWait: for every semaphore that has a thread that is waiting to be unblocked,
        //            we shuold check if time has expired or if somebody else called signal()
        //            set return value and put that thread in ready list in Scheduler

        TCB::running -> setTimeSliceCounter(TCB::running->getTimeSliceCounter() + 1);
        if(TCB::running -> getTimeSliceCounter() >= TCB::running->getTimeSlice()) 
        {
            TCB::running -> setTimeSliceCounter(0);
            TCB::dispatch();
        }

    } else if(scause == 0x8000000000000009UL) {
        // supervisor external interrupt (console)
        // clear external interrupt pending
        
        uint64 externalCause = plic_claim();
        if(externalCause == 0x0a) {
            _Console::setInterrupt(true);
        }

        mc_sip(SIP_SEIP);

    } else if(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        int sys_call = r_fp_register(10);
        sepc += 4;
        
        switch(sys_call) {
            case MEM_ALLOC:
            {
                uint64 size = r_fp_register(11);
                void* ret = MemoryAllocator::getMemoryAllocator()->_malloc(size);
                w_fp_register(10, (uint64)ret);
                break;
            }
            case MEM_FREE:
            {
                void* mem = (void*)r_fp_register(11);
                uint64 status = MemoryAllocator::getMemoryAllocator()->_free(mem);
                w_fp_register(10, status);
                break;
            }
            case THREAD_CREATE:
            {
                thread_t* handle = (thread_t*)r_fp_register(11);
                void(*body)(void*) = (void(*)(void*))r_fp_register(12);
                void* arg = (void*)r_fp_register(13);
                char* stack = (char*)r_fp_register(14);

                TCB* thread = new TCB(body, arg, stack);
                if(!thread) {
                    w_fp_register(10, -2);
                    break;
                }
                *(TCB**)handle = thread;
                Scheduler::put(thread);
                w_fp_register(10, 0);
                break;
            }
            case THREAD_EXIT:
            {
                TCB::running->setFinished(true);
                TCB::running->setReady(false);
                TCB::dispatch();
                break;
            }
            case THREAD_DISPATCH:
            {
                TCB::dispatch();
                break;
            }
            case TIME_SLEEP:
            {   
                time_t timeToSleep = (time_t)r_fp_register(11);
                TCB::running -> setTimeToSleep(Riscv::timerTickCounter + timeToSleep);
                TCB::running -> setReady(false);
                Scheduler::putToSleep(TCB::running);
                TCB::running -> dispatch();
                w_fp_register(10, 0);
                break;
            }
            case SEM_OPEN: {
                sem_t *handle = (sem_t*)r_fp_register(11);
                unsigned init = r_fp_register(12);
                _Semaphore *mySem = new _Semaphore(init);
                if(!mySem) {
                    w_fp_register(10, -2);
                    break;
                }
                *(_Semaphore**)handle = mySem;
                w_fp_register(10, 0);
                break;
            }
            case SEM_SIGNAL: {
                _Semaphore* id = (_Semaphore*)r_fp_register(11);
                if(!id) {
                    w_fp_register(10, -1);
                    break;    
                }
                id ->signal();
                w_fp_register(10, 0);
                break;
            }
            case SEM_WAIT: {
                _Semaphore* id = (_Semaphore*)r_fp_register(11);
                if(!id) {
                    w_fp_register(10, -1);
                    break;    
                }
                id -> wait();
                w_fp_register(10, 0);
                break;
            }
            case SEM_CLOSE: {
                _Semaphore* id = (_Semaphore*)r_fp_register(11);
                if(!id) {
                    w_fp_register(10, -1);
                    break;    
                }
                id -> close();
                w_fp_register(10, 0);
                break;
            }
            case SEM_TRYWAIT: {
                _Semaphore* id = (_Semaphore*)r_fp_register(11);
                if(!id){
                    w_fp_register(10, -1);
                    break;
                }
                w_fp_register(10, id -> tryWait());
                break;
            }
            case SEM_TIMEDWAIT: {
                _Semaphore* id = (_Semaphore*)r_fp_register(11);
                time_t timeout = r_fp_register(12);
                if(!id) {
                    w_fp_register(10, -1);
                    break;
                }
                w_fp_register(10, id -> timedWait(timeout));
                break;
            }
            case PUTC: {
                char c = (char)r_fp_register(11);
                _Console::getConsole() -> putc(c);
                break;
            }
            case GETC: {
                char c = _Console::getConsole() -> getc();
                w_fp_register(10, c);
                break;
            }
        }

    } else if( scause == 0x0000000000000005UL)  {
        while(1);
    }

    Riscv::w_sepc(sepc);
    Riscv::w_sstatus(sstatus);
}
