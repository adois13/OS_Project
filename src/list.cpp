#include "../h/list.hpp"
#include "../h/memoryAllocator.hpp"

void List::addFirst(TCB *data) {
    Node* node = (Node*) MemoryAllocator::getMemoryAllocator()->_malloc(1);
    if(!node) return;

    node->data = data;
    node->next = head;
    head = node;
    if (!tail)
        tail = head;
}

void List::addLast(TCB *data) {
    Node* node = (Node*) MemoryAllocator::getMemoryAllocator()->_malloc(1);
    if(!node) return;

    node->data = data;
    node->next = nullptr;

    if (tail) {
        tail->next = node;
        tail = node;
    } else {
        head = tail = node;   
    }
}

void List::remove(TCB* data) {
    if (!head) return;

    Node *curr = head, *prev = nullptr;

    while (curr && curr->data != data) {
        prev = curr; 
        curr = curr->next; 
    }

    if (!curr) return;

    if (curr == head) {
        head = curr->next;
        if (!head) {
            tail = nullptr;
        }
    } else {
        prev->next = curr->next;
        if (!curr->next) {
            tail = prev;
        }
    }

    MemoryAllocator::getMemoryAllocator()->_free(prev);
    MemoryAllocator::getMemoryAllocator()->_free(curr);
}

TCB *List::removeFirst() {
    if (!head)
        return nullptr;

    Node *node = head;
    head = head->next;

    if (!head) {
        tail = nullptr;
    }

    TCB *ret = node->data;
    MemoryAllocator::getMemoryAllocator()->_free(node);
    return ret;
}

TCB *List::peekFirst() {
    if (head) {
        return head->data;
    }
    return nullptr;
}

TCB *List::removeLast() {
    if (!head)
        return nullptr;

    Node *prev = nullptr;
    Node *curr = head;

    while (curr && curr != tail) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) return nullptr;

    Node *node = tail;
    if (prev) {
        prev->next = nullptr;
        tail = prev;
    } else {
        head = tail = nullptr;
    }

    TCB *ret = node->data;
    MemoryAllocator::getMemoryAllocator()->_free(node);
    return ret;
}

TCB *List::peekLast() {
    if (tail) {
        return tail->data;
    }
    return nullptr;
}

void List::insertSorted(TCB *thread) {
    Node* node = (Node*) MemoryAllocator::getMemoryAllocator()->_malloc(1);
    if(!node) return;

    node->data = thread;
    node->next = nullptr;

    if (!head) {
        head = tail = node;
        return;
    }

    Node *curr = head, *prev = nullptr;

    while (curr && curr->data->getTimeToSleep() < thread->getTimeToSleep()) {
        prev = curr;
        curr = curr->next;
    }

    if (!prev) {
        node->next = head;
        head = node;
    } else if (!curr) {
        prev->next = node;
        tail = node;
    } else {
        prev->next = node;
        node->next = curr;
    }
}