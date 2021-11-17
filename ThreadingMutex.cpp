// C++ STL: Thread
// Michael Yu
// 8/31/2021

//#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>

std::mutex lock;

class SharedResource
{
public:
    // Default ctor, dtor, copy, and move
    SharedResource() = default;
    ~SharedResource() = default;
    SharedResource(const SharedResource& other) = default;
    SharedResource(SharedResource&& other) = default;
    SharedResource& operator=(const SharedResource& other) = default;
    SharedResource& operator=(SharedResource&& other) = default;

    // Pre-fix increment
    SharedResource& operator++()
    {
        ++val;
        return *this;
    }

    // Post-fix increment
    SharedResource operator++(int)
    {
        SharedResource retObj = *this;
        ++* this;
        return retObj;
    }

    int get()
    {
        return val;
    }

private:
    int val = 0;
};

void executeFunction(SharedResource& src)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++src;
}

void executeFunctionMutex(SharedResource& src)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> guard(lock);
    ++src;
}

void print(std::string message, SharedResource& src, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end)
{
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "---------------------------" << std::endl;
    std::cout << message << std::endl;
    std::cout << "Elapsed time " << std::setprecision(3) << elapsed.count() << " seconds." << std::endl;
    std::cout << "SharedResource value is " << src.get() << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void ThreadingMutex()
{
    std::cout << "----------START: THREADING AND MUTEX EXAMPLE----------" << std::endl;
    // -------------------------------------------------------------------------------
    SharedResource srcSequential;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100; ++i)
    {
        executeFunction(srcSequential);
    }
    auto end = std::chrono::steady_clock::now();
    print("Sequential (single thread)", srcSequential, start, end);

    // -------------------------------------------------------------------------------
    std::vector<std::thread> threads;
    SharedResource srcThread;
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100; ++i)
    {
        threads.push_back(std::thread(executeFunction, std::ref(srcThread)));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
    end = std::chrono::steady_clock::now();
    print("Multi-threading without Mutex", srcThread, start, end);

    // -------------------------------------------------------------------------------
    threads.clear();
    SharedResource srcThreadMutex;
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100; ++i)
    {
        threads.push_back(std::thread(executeFunctionMutex, std::ref(srcThreadMutex)));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
    end = std::chrono::steady_clock::now();
    print("Multi-threading with Mutex", srcThreadMutex, start, end);
    std::cout << "----------END: THREADING AND MUTEX EXAMPLE----------" << std::endl;
}