#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../h/hw.h"
#include "../h/riscv.hpp"


enum status {
        REGULAR = 0,
        BLOCKED_ON_SEM = 1,
        SEM_CLOSED = -1, 
        SIGNAL = 2, 
        TIMED_WAIT = -2, 
        UNBLOCKED_ON_SIGNAL = 3,
        TIMEOUT = -3
    };

class _Semaphore;

class TCB {
public:
    ~TCB() { delete stack; }

    using Body = void(*)(void*);
    
    TCB(Body body, void* arg, char* stack);

    inline bool isFinished() const { return TCB::finished;}
    inline void setFinished(bool flag) { finished = flag; }

    inline uint64 getTimeSlice() const { return TCB::timeSlice; }
    inline uint64 getTimeSliceCounter() const { return TCB::timeSliceCounter; }
    inline void setTimeSliceCounter(uint64 timeSliceCounter) { TCB::timeSliceCounter = timeSliceCounter;}

    inline time_t getTimeToSleep() const { return TCB::timeToSleep; } 
    inline void setTimeToSleep(time_t time) { TCB::timeToSleep = time; } 

    inline bool isReady() const { return TCB::ready; }
    inline void setReady(bool flag) { TCB::ready = flag; } 

    inline int getStatus() const { return StatusTCB; } 
    inline void setStatus(status Status) { StatusTCB = Status; }

    inline _Semaphore* getSemaphore() const { return TCB::handle; }
    inline void setSemaphore(_Semaphore* sem) { TCB::handle = sem; } 

    static void dispatch();

    static TCB *running;

private: 
    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    char *stack;
    Context context;
    uint64 timeSlice;

    bool finished;
    bool ready;
    
    uint64 timeSliceCounter;
    time_t timeToSleep;

    _Semaphore* handle;

    static void threadWrapper();

    static void contextSwitch(Context*, Context*);

    status StatusTCB = REGULAR;

};

#endif