// C++ STL: Thread
// Michael Yu
// 8/31/2021

//#pragma once
#include <thread>
#include <latch>
#include <barrier>
#include <iostream>

// Example from https://en.cppreference.com/w/cpp/thread/latch
// Modified example with additional commentary

struct Job {
    const std::string name;
    std::string product{ "not worked" };
    std::thread action{};
};

void Latch()
{
    std::cout << "----------START: LATCH EXAMPLE----------" << std::endl;
    Job jobs[] = { {"annika"}, {"buru"}, {"chuck"} };

    // Instantiate the latches
    std::latch work_done{ std::size(jobs) };
    std::latch start_clean_up{ 1 };

    // Lambda function that will be executed in each thread
    auto work = [&](Job& my_job) {
        my_job.product = my_job.name + " studied in morning";
        work_done.count_down();
        // Wait() method allows for threads to synchronize before continuing to clean up
        start_clean_up.wait();
        my_job.product = my_job.name + " cleaned in the afternoon";
    };

    // Instantiate threads and execute each thread
    std::cout << "MORNING: Work starting... ";
    for (auto& job : jobs) {
        job.action = std::thread{ work, std::ref(job) };
    }
    work_done.wait();
    std::cout << "done:\n";
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }

    std::cout << "AFTERNOON: Workers cleaning up... ";
    // Begin part 2 of job tasks for each job element (start_clean_up phase)
    // start_clean_up latch initialized to countdown of 1. Calling count_down() method will unblock the threads that were waiting for start_clean_up latch
    // Deadlock scenario if count_down() is not called, but other threads are waiting for count_down() to be called in order to finish executing
    start_clean_up.count_down();
    for (auto& job : jobs) {
        job.action.join();
    }
    std::cout << "done:\n";
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }
    std::cout << "----------END: LATCH EXAMPLE----------" << std::endl;
}

void NoLatch()
{
    std::cout << "----------START: NO LATCH EXAMPLE----------" << std::endl;
    Job jobs[] = { {"annika"}, {"buru"}, {"chuck"} };

    std::latch work_done{ std::size(jobs) };

    auto work = [&](Job& my_job) {
        my_job.product = my_job.name + " studied in morning";
        work_done.count_down();
        // Notice no wait() method for start_clean_up. Result in my_job.product being overwritten from "worked" to "cleaned" before synchronization
        my_job.product = my_job.name + " cleaned in the afternoon";
    };

    std::cout << "MORNING: Work starting... ";
    for (auto& job : jobs) {
        job.action = std::thread{ work, std::ref(job) };
    }
    work_done.wait();
    std::cout << "done:\n";
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }

    std::cout << "AFTERNOON: Workers cleaning up... ";
    for (auto& job : jobs) {
        job.action.join();
    }
    std::cout << "done:\n";
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }
    std::cout << "----------END: NO LATCH EXAMPLE----------" << std::endl;
}

void Barrier()
{
    std::cout << "----------START: BARRIER EXAMPLE----------" << std::endl;
    Job jobs[] = { {"annika"}, {"buru"}, {"chuck"} };

    // Completion function when barrier countdown hits 0
    auto on_completion = []() noexcept {
        // locking not needed here
        // static variables are initialized once. subsequent initialization is ignored
        static auto phase = "**Barrier first use completed**\nAFTERNOON: Workers cleaning up... done:";
        std::cout << phase << std::endl;

        // set phase value for next on_completion call
        phase = "**Barrier reuse completed**";
    };

    // std::barrier ctor parameters initialize the countdown and functor that will be executed when countdown hits 0
    // std::barrier auto-reset countdown after hitting 0
    std::barrier sync_point(std::size(jobs), on_completion);

    // Lambda function that will be executed in each thread
    // Similar to latch example, but notice the barrier reuse 
    auto work = [&](Job& my_job) {
        my_job.product = "  " + my_job.name + " studied in morning" + "\n";
        std::cout << my_job.product;
        sync_point.arrive_and_wait();

        my_job.product = "  " + my_job.name + " cleaned in the afternoon" + "\n";
        std::cout << my_job.product;
        sync_point.arrive_and_wait();
    };

    // Instantiate threads and execute code
    std::cout << "MORNING: Work starting... done:" << std::endl;
    for (auto& job : jobs) {
        job.action = std::thread{ work, std::ref(job) };
    }

    for (auto& job : jobs) {
        job.action.join();
    }
    std::cout << "----------END: BARRIER EXAMPLE----------" << std::endl;
}