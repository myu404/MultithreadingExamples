// C++ STL: Thread
// Michael Yu
// 8/31/2021

#include <iostream>
#include <vector>
#include <functional>

extern void ThreadingMutex();
extern void ConditionVariables();
extern void Semaphore();
extern void Latch();
extern void NoLatch();
extern void Barrier();


int main()
{
    std::cout << "-----------------START OF DEMO--------------------" << std::endl;
    std::vector < std::function<void()> > examples{ ThreadingMutex, ConditionVariables, Semaphore, Latch, NoLatch, Barrier };
    for (auto& example : examples)
    {
#ifndef NDEBUG
        // DEBUG MODE: pause between examples and wait for user to press enter to continue to next example
        std::cin.ignore();
#endif
        example();
    }
    std::cout << "-----------------END OF DEMO--------------------" << std::endl;
    return 0;
}
