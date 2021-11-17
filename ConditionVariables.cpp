// C++ STL: Thread
// Michael Yu
// 8/31/2021

//#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>

// Example from https://en.cppreference.com/w/cpp/thread/condition_variable
// Modified example with additional commentary

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
    // 2 --------------------
    // Wait until main() sends data
    std::cout << "WORKER THREAD: initiate mutex, unlock mutex, block worker thread until main thread signals or set predicate to true" << std::endl;
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] {return ready; });

    // 4 --------------------
    // after the wait, we own the lock.
    std::cout << "WORKER THREAD: Back in worker thread after signal" << std::endl;
    std::cout << "WORKER THREAD: Worker thread is processing data\n";
    data += " after processing";

    // Send data back to main()
    processed = true;
    std::cout << "WORKER THREAD: Worker thread signals data processing completed\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

void ConditionVariables()
{
    // 1 --------------------
    std::cout << "----------START: CONDITION VARIABLE EXAMPLE----------" << std::endl;
    std::cout << "MAIN THREAD: instantiate and execute worker thread" << std::endl;
    std::thread worker(worker_thread);

    // 3 --------------------
    // Simulate expensive operation on main thread, while worker thread is running
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "MAIN THREAD: executing expensive operation before signaling worker thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "MAIN THREAD: main() signals data ready for processing\n";
    }
    cv.notify_one();

    // wait for the worker
    {
        std::cout << "MAIN THREAD: waiting for worker thread to finish process and signal before continuing" << std::endl;
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return processed; });
    }

    // 5 --------------------
    std::cout << "MAIN THREAD: Back in main(), data = " << data << '\n';

    worker.join();
    std::cout << "----------END: CONDITION VARIABLE EXAMPLE----------" << std::endl;
}