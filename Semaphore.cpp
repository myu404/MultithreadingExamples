// C++ STL: Thread
// Michael Yu
// 8/31/2021

//#pragma once
#include <thread>
#include <semaphore>
#include <iostream>

// Example from https://en.cppreference.com/w/cpp/thread/condition_variable
// Modified example with additional commentary

std::binary_semaphore bSemaphore(0);
std::string data1;

void worker_thread_semaphore()
{
    // 2 --------------------
    // Wait until main() sends data
    std::cout << "WORKER THREAD: thread is blocked until binary semaphore is acquired from another thread (other thread release semaphore, which increases counter to 1)" << std::endl;
    bSemaphore.acquire();

    // 4 --------------------
    // after the wait, we own the lock.
    std::cout << "WORKER THREAD: Acquired binary semaphore" << std::endl;
    std::cout << "WORKER THREAD: Worker thread is processing data\n";
    data1 += " after processing";

    // Send data back to main()
    std::cout << "WORKER THREAD: thread releases binary semaphore. Binary semaphore counter increases from 0 to 1 and allow for another thread to acquire it\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    bSemaphore.release();
}

void Semaphore()
{
    // 1 --------------------
    std::cout << "----------START: SEMAPHORE EXAMPLE----------" << std::endl;
    std::cout << "MAIN THREAD: instantiate and execute worker thread" << std::endl;
    std::thread worker(worker_thread_semaphore);

    // 3 --------------------
    // Simulate expensive operation on main thread, while worker thread is running
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "MAIN THREAD: executing expensive operation before releasing binary semaphore to worker thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    data1 = "Example data";
    std::cout << "MAIN THREAD: main() releases binary semaphore. Will be acquired by worker thread\n";
    bSemaphore.release();

    // wait for the worker
    {
        std::cout << "MAIN THREAD: Waiting to acquire binary semaphore back from worker thread" << std::endl;
        bSemaphore.acquire();
    }

    // 5 --------------------
    std::cout << "MAIN THREAD: Acquired binary semaphore from worker thread, data = " << data1 << '\n';

    worker.join();
    std::cout << "----------END: SEMAPHORE EXAMPLE----------" << std::endl;
}
