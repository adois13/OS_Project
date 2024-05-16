#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size) {
    size_t blocks = (size + 2*8 + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    __asm__ volatile("mv a1, %[blocks]" : : [blocks] "r"(blocks));
    __asm__ volatile("li a0, %[code]" : : [code] "i"(MEM_ALLOC));
    void* volatile ret;
    __asm__ volatile("ecall");
    
    __asm__ volatile("mv %[ret], a0": [ret] "=r"(ret));
    return ret;

}

int mem_free(void* addr) {
    __asm__ volatile("mv a1, %[addr]" : : [addr] "r"(addr));
    __asm__ volatile("li a0, %[code]" : : [code] "i"(MEM_FREE));
    int volatile status;
    __asm__ volatile("ecall");

    __asm__ volatile("mv %[status], a0" : [status] "=r"(status));
    return status;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    char* stack = (char*)mem_alloc(DEFAULT_STACK_SIZE);
    if(stack == nullptr) {
        return -1;
    }
    __asm__ volatile("mv a4, %[stack]" : : [stack] "r"(stack));
    __asm__ volatile("mv a3, %[arg]" : : [arg] "r"(arg));
    __asm__ volatile("mv a2, %[start_routine]" : : [start_routine] "r"(start_routine));
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r"(handle));
    __asm__ volatile("li a0, %[code]": : [code] "i"(THREAD_CREATE));

    __asm__ volatile("ecall");
    int volatile status;
    
    __asm__ volatile("mv %[status], a0" : [status] "=r"(status));
    return status;
}

int thread_exit() {
    __asm__ volatile ("li a0, %[code]" : : [code] "i"(THREAD_EXIT));
    int volatile status;
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[status], a0": [status] "=r"(status));
    return status;
}

void thread_dispatch() {
    __asm__ volatile("li a0, %[code]" : : [code] "i"(THREAD_DISPATCH));
    __asm__ volatile ("ecall");
}

int time_sleep(time_t timeToSleep) {
    __asm__ volatile("mv a1, %[timeToSleep]" : : [timeToSleep] "r"(timeToSleep));
    __asm__ volatile("li a0, %[code]": : [code] "i"(TIME_SLEEP));
    int volatile status;
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_open(sem_t* myHandle, unsigned init) {
    __asm__ volatile ("mv a2, %[init]" : : [init] "r"(init));
    __asm__ volatile ("mv a1, %[myHandle]" : : [myHandle] "r"(myHandle));
    __asm__ volatile ("li a0, %[code]" : : [code] "i"(SEM_OPEN));
    int volatile status;

    __asm__ volatile("ecall");
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_close(sem_t myHandle) {
    __asm__ volatile("mv a1, %[myHandle]": : [myHandle] "r"(myHandle));
    __asm__ volatile("li a0, %[code]" : : [code] "i"(SEM_CLOSE));
    int volatile status;
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[status], a0" : [status] "=r"(status));
    return status;
}

int sem_signal(sem_t myHandle) {
    __asm__ volatile("mv a1, %[myHandle]": : [myHandle] "r"(myHandle));
    __asm__ volatile("li a0, %[code]" : : [code] "i"(SEM_SIGNAL));
    int volatile status;
    __asm__ volatile("ecall");

    __asm__ volatile("mv %[status], a0" : [status] "=r"(status));
    return status;
}

int sem_wait(sem_t myHandle) {
    __asm__ volatile("mv a1, %[myHandle]": : [myHandle] "r"(myHandle));
    __asm__ volatile("li a0, %[code]" : : [code] "i"(SEM_WAIT));
    int volatile status;
    __asm__ volatile("ecall");

    __asm__ volatile("mv %[status], a0" : [status] "=r"(status));
    return status;
}

int sem_timedwait(sem_t id, time_t timeout) {
    __asm__ volatile("mv a2, %[timeout]" : : [timeout] "r"(timeout));
    __asm__ volatile("mv a1, %[id]" : : [id] "r"(id));
    __asm__ volatile("mv a0, %[code]" : : [code] "r"(SEM_TIMEDWAIT));

    __asm__ volatile("ecall");
    return 0;
}

int sem_trywait(sem_t id) {
    __asm__ volatile("mv a1, %[id]" : : [id] "r"(id));
    __asm__ volatile("mv a0, %[code]" : : [code] "r"(SEM_TRYWAIT));

    __asm__ volatile("ecall");
    return 0;
}

char getc() {
    __asm__ volatile ("mv a0, %[code]" : : [code] "r"(GETC));
    char volatile ret;

    __asm__ volatile("ecall");
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r"(ret));
    return ret;
}

void putc(char c) {
    __asm__ volatile ("mv a1, %[c]" : : [c] "r"(c));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r"(PUTC));

    __asm__ volatile ("ecall");
}