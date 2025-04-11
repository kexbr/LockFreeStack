#ifndef STACK_H 
#define STACK_H
#include "stack.h"

template <class T>
void Stack<T>::Push(const T& value) {
    Node* cur_head = head_.load();
    Node* new_node = new Node(value);
    new_node->next_ = cur_head;
    while (!head_.compare_exchange_weak(new_node->next_, new_node)) {
    }
}

template<class T>
std::shared_ptr<T> Stack<T>::Pop() {
    Node* cur = head_.load();
    if (cur == nullptr) return nullptr;
    while(cur != nullptr) {
        if (head_.compare_exchange_weak(cur, cur->next_)) {
            std::shared_ptr<T> res;
            res.swap(cur->data_);
            delete cur;
            return res;
        }
    }
    return nullptr;
}
#endif // STACK_H