#include <atomic>
#include <memory>

template <class T>
class Stack {
   public:
    struct Node {
        std::shared_ptr<T> data_;
        Node* next_;

        Node(const T& data) {
            data_ = std::make_shared<T>(data);
            next_ = nullptr;
        }

        Node(T&& data) {
            data_ = std::make_shared<T>(std::move(data));
            next_ = nullptr;
        }

        Node() {
            data_ = nullptr;
            next_ = nullptr;
        }
    };

    void Push(const T& value);
    std::shared_ptr<T> Pop();
    ~Stack();

   private:
    std::atomic<Node*> head_{nullptr};
};

template <class T>
void Stack<T>::Push(const T& value) {
    Node* cur_head = head_.load(std::memory_order_relaxed);
    Node* new_node = new Node(value);
    new_node->next_ = cur_head;
    while (!head_.compare_exchange_weak(
        new_node->next_, new_node, std::memory_order_release, std::memory_order_relaxed)) {
    }
}

template <class T>
std::shared_ptr<T> Stack<T>::Pop() {
    Node* cur = head_.load(std::memory_order_acquire);
    while (cur != nullptr) {
        if (head_.compare_exchange_weak(
                cur, cur->next_, std::memory_order_release, std::memory_order_relaxed)) {
            std::shared_ptr<T> res = std::move(cur->data_);
            cur->data_ = nullptr;
            delete cur;
            return res;
        }
    }
    return nullptr;
}

template <class T>
Stack<T>::~Stack() {
    Node* cur = head_.load(std::memory_order_relaxed);
    while (cur != nullptr) {
        Node* next = cur->next_;
        delete cur;
        cur = next;
    }
}