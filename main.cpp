#include "stack.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

bool start_flag;
std::mutex start_mutex;
std::condition_variable cv;
Stack<int> my_super_puper_stack;
std::mt19937_64 rnd;

void wait() {
    std::unique_lock<std::mutex> lk(start_mutex);
    cv.wait(lk, [] { return start_flag == true; });
    return;
}

void PushThread() {
    wait();
    int cnt = 10'0000;
    while (cnt--) {
        my_super_puper_stack.Push(rnd() % (1'000'000'000 + 7));
    }
}

void PopThread() {
    wait();
    int cnt = 10'000;
    while (cnt--) {
    //    my_super_puper_stack.Pop();
    }
}

int main() {
    start_flag = false;
    std::cout << "Preparing threads..." << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < 50; i++) {
        threads.emplace_back(PushThread);
    }
    for (int i = 0; i < 20; i++) {
        threads.emplace_back(PopThread);
    }
    std::cout << "Starting threads..." << std::endl;
    start_flag = true;
    cv.notify_all();
    for (auto &i : threads) {
        i.join();
    }
    std::cout << "Success!" << std::endl;
}