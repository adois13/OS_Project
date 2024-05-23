#ifndef _riscv_hpp_
#define _riscv_hpp_

#include "hw.h"

class Riscv {

public:

    static time_t timerTickCounter;
    //read register scause
    static uint64 r_scause();
    //write register scause
    static void w_scause(uint64 scause);

    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);
    
    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);

    static uint64 r_stval();
    static void w_stval(uint64 stval);

    enum BitMaskSip 
    {
        // supervisor software interrupt pending
        SIP_SSIP = (1 << 1),
        // supervisor external interrupt pending
        SIP_SEIP = (1 << 9)
    };
    // mask set register sip
    static void ms_sip(uint64 mask);
    // mask clear register sip
    static void mc_sip(uint64 mask);
    // read register sip
    static uint64 r_sip();
    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8)
    }; 
    // mask set register sstatus
    static void ms_sstatus(uint64 mask);
    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);
    // read register sstatus
    static uint64 r_sstatus();
    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // save register on stack
    static void pushRegisters();
    // pop register from stack
    static void popRegisters();

    static uint64 r_fp_register(uint64 index); 
    static void w_fp_register(uint64 index, int value); 
    
    // setting new Spp and Spie for thread that has no context
    static void newRunningSppSpie();

    //supervisor trap -> called by processor when there is an exception or interrupt 
    static void supervisorTrap();

private:
    // supervisor trap handler
    static void handleSupervisorTrap();

};

inline uint64 Riscv::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause){
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec] stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
} 

inline uint64 Riscv::r_stval() {
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval) {
    __asm__ volatile ("csrw stval %[stval]" : : [stval] "r"(stval));
}

// sip register 
inline uint64 Riscv::r_sip() {
    uint64 volatile sip;
    __asm__ volatile ("csrr sip %[sip]" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip) {
    __asm__ volatile ("csrw %[sip] sip" : : [sip] "r"(sip));
}

inline void Riscv::ms_sip(uint64 mask) {
    __asm__ volatile ("csrs sip, %[mask]": : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

//sstatus register
inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus": [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::ms_sstatus(uint64 mask) {
    __asm__  volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

// accessing i-th register - setting and reading it's value
inline uint64 Riscv::r_fp_register(uint64 idx) {
    uint64 volatile regValue = 0;
    __asm__ volatile("ld %[regValue], 8*%[idx](fp)" : [regValue] "=r"(regValue) : [idx] "i"(idx));
    return regValue;
}

inline void Riscv::w_fp_register(uint64 index, int value){
    __asm__ volatile("sd %[value], 8*%[index](fp)" :: [index] "i"(index), [value] "r"(value) : "memory");
}

#endif