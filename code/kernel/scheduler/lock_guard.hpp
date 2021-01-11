#pragma once

#include "mutex.hpp"

template<typename Mutex>
class lock_guard
{
public:
    lock_guard(Mutex& m) : mutex_(m)
    {
        mutex_.lock();
    }

    ~lock_guard()
    {
        mutex_.unlock();
    }

private:
    Mutex& mutex_;
};
