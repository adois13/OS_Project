#ifndef _syscall_cpp_hpp_
#define _syscall_cpp_hpp_

#include "../h/syscall_c.hpp"
#include "../h/list.hpp"
#include "../h/TCB.hpp"

void* operator new (size_t);
void operator delete (void*);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);

protected:
    Thread ();
    virtual void run () {}

private:
    static void runWrapper(void* obj) {
        ((Thread*)obj) -> run();
    }

    thread_t myHandle;
    void (*body)(void*); 
    void* arg;
};


class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    
    int wait ();
    int signal ();
    int timedWait (time_t);
    int tryWait();

private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
public:
    void terminate ();

protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    virtual void run() override;

private:
    time_t period;
};


class Console {   
public:
    static char getc ();
    static void putc (char);
};


#endif