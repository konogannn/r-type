#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <vector>

void worker(int id)
{
    for (int i = 0; i < 5; i++) {
        std::cout << "Thread " << id << ": " << i << std::endl;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

int main()
{
    boost::thread_group threads;

    for (int i = 0; i < 3; i++) {
        threads.create_thread(boost::bind(worker, i));
    }

    threads.join_all();

    return 0;
}
