#ifndef _list_hpp_
#define _list_hpp_

#include "../h/hw.h"
#include "../h/TCB.hpp"
#include "../h/memoryAllocator.hpp"

class List
{
private:
    struct Node {
        TCB *data;
        Node *next;

        Node(TCB *data, Node *next) : data(data), next(next) {}
    };

    Node *head;
    Node *tail;

public:
    List() : head(nullptr), tail(nullptr) {}

    List(const List &) = delete;
    List &operator=(const List &) = delete;

    void addFirst(TCB *data);
    void addLast(TCB *data);
    void remove(TCB* data);

    TCB *removeFirst();

    TCB *peekFirst();

    TCB *removeLast();

    TCB *peekLast();

    void insertSorted(TCB *thread);
};

#endif