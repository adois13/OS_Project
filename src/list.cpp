#include "../h/list.hpp"

void List::addFirst(TCB *data) {
    Node *node = new Node(data, head);
    head = node;
    if (!tail)
        tail = head;
}

void List::addLast(TCB *data) {
    Node *node = new Node(data, nullptr);
    if (tail) {
        tail->next = node;
        tail = node;
    } else {
        head = tail = node;
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
    delete node;
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
    delete node;
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