#include "../h/console.hpp"
#include "../h/memoryAllocator.hpp"

_Console::_Console() {
    waitToRead = (_Semaphore*) new _Semaphore(0);
    waitToWrite = (_Semaphore*) new _Semaphore(BUFFER_SIZE);
    mutex = (_Semaphore*) new _Semaphore(1);
}

int _Console::inputHead = 0;
int _Console::inputTail = 0;
int _Console::outputHead = 0;
int _Console::outputTail = 0;

char _Console::inputBuffer[BUFFER_SIZE];
char _Console::outputBuffer[BUFFER_SIZE];
bool _Console::interruptFlag = false;

_Semaphore* _Console::waitToRead = nullptr;
_Semaphore* _Console::waitToWrite = nullptr;
_Semaphore *_Console::mutex = nullptr;

_Console* _Console::getConsole() {
    static _Console myConsole;
    return &myConsole;
}

void _Console::putc(char c) {
    waitToWrite -> wait();
    outputBuffer[outputTail++] = c;
    outputTail %= BUFFER_SIZE;
}

char _Console::getc() {
    waitToRead -> wait(); 
    char ret = inputBuffer[inputTail++];
    inputTail %= BUFFER_SIZE;
    return ret;
}

void _Console::input_wrapper(void* p) {
    static bool inputFlag = false;
    if(!inputFlag) {
        inputFlag = true;
        _Console::input();
    }
}

void _Console::output_wrapper(void* p) {
    static bool outputFlag = false;
    if(!outputFlag) {
        outputFlag = true;
        _Console::output();
    }
}

void _Console::input() {
    while(true) {
        while((_Console::inputHead + 1) % BUFFER_SIZE == _Console::inputTail) {
            thread_dispatch();
        }
        if(_Console::checkForInput() && _Console::getInterrupt()) {
            _Console::inputBuffer[_Console::inputHead++] = *((char*)CONSOLE_RX_DATA);
            _Console::inputHead %= BUFFER_SIZE;
            waitToRead -> signal();
        } else {
            if(!_Console::checkForInput() && !_Console::checkForOutput() && _Console::getInterrupt()) {
                    mutex -> wait();
                    _Console::setInterrupt(false);
                    plic_complete(0xa);
                    mutex -> signal();
            }
            thread_dispatch();
        }
    }
}

void _Console::output() {
    while(true) {
        while(_Console::outputHead == _Console::outputTail) {
            thread_dispatch();
        }
        if(_Console::checkForOutput() && _Console::getInterrupt()) {
            *((char*)CONSOLE_TX_DATA) = _Console::outputBuffer[_Console::outputHead++];
            _Console::outputHead %= BUFFER_SIZE;
            waitToWrite -> signal();
        } else {
            mutex -> wait();
            if(!_Console::checkForOutput() && !_Console::checkForInput() && _Console::getInterrupt()) {
                _Console::setInterrupt(false);
                plic_complete(0xa);
            }
            mutex -> signal();
            thread_dispatch();
        }
    }
}

inline bool _Console::checkForInput() {
    return (*(char*)CONSOLE_STATUS & CONSOLE_RX_STATUS_BIT) != 0;
}

inline bool _Console::checkForOutput() {
    return (*(char*)CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT) != 0;
}

void _Console::setInterrupt(bool flag) {
    _Console::interruptFlag = flag;
}

bool _Console::getInterrupt(){
    return _Console::interruptFlag;
}
