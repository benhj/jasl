/// Ben Jones 2019

#pragma once

#include <functional>
#include <thread>
#include <vector>

namespace jasl {

    /// A hacky global thread manager for handling threads
    /// within the JASL language.
    class ThreadManager
    {
      public:
        static void create(std::function<void()> f);
        static void joinAll();
        static void clear();
      private:
        static std::vector<std::thread> m_threads;
    };
}