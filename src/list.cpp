#include "../h/list.hpp"
#include "../h/memoryAllocator.hpp"

void List::addFirst(TCB *data) {
    Node* node = (Node*) MemoryAllocator::getMemoryAllocator()->_malloc(1);
    node->data = data;
    node->next = head;
    //Node *node = new Node(data, head);
    head = node;
    if (!tail)
        tail = head;
}

void List::addLast(TCB *data) {
    //Node *node = new Node(data, nullptr);
    Node* node = (Node*) MemoryAllocator::getMemoryAllocator()->_malloc(1);
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
    if(!head) return;
    
    Node *curr = head, *prev = nullptr;

    while(curr && curr -> data != data) {
        prev = curr; curr = curr -> next;
    }

    if(prev && curr) {
        prev -> next = curr -> next;
    } else if(!prev && curr) {
        head = curr -> next;
    } else if (tail == curr) {
        prev -> next = nullptr;
        tail = prev;
    }
}

TCB *List::removeFirst() {
    if (!head)
        return 0;

    Node *node = head;
    head = head->next;

    if (!head) {
        tail = 0;
    }

    TCB *ret = node->data;
    //delete node;
    MemoryAllocator::getMemoryAllocator()->_free(node);
    return ret;
}

TCB *List::peekFirst() {
    if (head) {
        return head->data;
    }
    return 0;
}

TCB *List::removeLast() {
    if (!head)
        return 0;

    Node *prev = 0;
    for (Node *curr = head; curr && curr != tail; curr = curr->next) {
        prev = curr;
    }

    Node *node = tail;
    if (prev) {
        prev->next = 0;
    } else {
        head = 0;
    }
    tail = prev;

    TCB *ret = node->data;
    //delete node;
    MemoryAllocator::getMemoryAllocator()->_free(node);
    return ret;
}

TCB *List::peekLast() {
    if (tail) {
        return tail->data;
    }
    return 0;
}

void List::insertSorted(TCB *thread) {
    Node *node = new Node(thread, nullptr);
    if (!head) {
        head = tail = node;
        return;
    }

    Node *curr = head, *prev = nullptr;

    while (curr && curr->data->getTimeToSleep() < thread->getTimeToSleep()) {
        prev = curr;
        curr = curr->next;
    }
    if (curr && prev) {
        prev->next = node;
        node->next = curr;
        return;
    }
    if (curr == nullptr) {
        prev->next = node;
        tail = node;
    }
    else if (prev == nullptr) {
        node->next = head;
        head = node;
    }
}