#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void worker(int id)
{
    for (int i = 0; i < 5; i++) {
        std::cout << "Thread " << id << ": " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 3; i++) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
