#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../h/hw.h"
#include "../h/riscv.hpp"

class TCB {
public:
    ~TCB() { delete stack; }

    using Body = void(*)(void*);

    TCB(Body body, void* arg, char* stack);

    // finished flags
    inline bool isFinished() const { return TCB::finished;}
    inline void setFinished(bool flag) { finished = flag; }

    // async context switch 
    inline uint64 getTimeSlice() const { return TCB::timeSlice; }
    inline uint64 getTimeSliceCounter() const { return TCB::timeSliceCounter; }
    inline void setTimeSliceCounter(uint64 timeSliceCounter) { TCB::timeSliceCounter = timeSliceCounter;}

    // sleeping thread 
    inline time_t getTimeToSleep() const { return TCB::timeToSleep; } 
    inline void setTimeToSleep(time_t time) { TCB::timeToSleep = time; } 

    // ready flags
    inline bool isReady() const { return TCB::ready; }
    inline void setReady(bool flag) { TCB::ready = flag; } 
    
    // unblocked after closing a semaphore
    inline bool getClosedSemFlag() const { return TCB::closedSemFlag; }
    inline void setClosedSemFlag(bool flag) { TCB::closedSemFlag = flag; }

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
    // flags to determine if thread continued regularly or because the semaphore is closed
    bool closedSemFlag;
    
    uint64 timeSliceCounter;
    time_t timeToSleep;

    static void threadWrapper();

    static void contextSwitch(Context*, Context*);
};

#endif