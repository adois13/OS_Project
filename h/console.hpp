#ifndef _console_h_
#define _console_h_

#include "../h/semaphore.hpp"

#define BUFFER_SIZE 1024

class _Console {
    friend class Riscv;
    
public:
    static _Console* getConsole();

    char getc();
    void putc(char c);

    static void input_wrapper(void*);
    static void output_wrapper(void*);

private:
    _Console();

    static int inputHead, inputTail;
    static int outputHead, outputTail;

    static char inputBuffer[BUFFER_SIZE];
    static char outputBuffer[BUFFER_SIZE];

    static _Semaphore *waitToRead;
    static _Semaphore *waitToWrite;
    static _Semaphore *mutex;

    static void input();
    static void output();

    inline static bool checkForInput();
    inline static bool checkForOutput();

    static void setInterrupt(bool flag);
    static bool getInterrupt();

    static bool interruptFlag;
};


#endif