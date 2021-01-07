#pragma once

#include "mutex.hpp"

class lock_guard
{
public:
    lock_guard(mutex& m) : mutex_(m)
    {
        m.spinlock();
    }

    ~lock_guard()
    {
        mutex_.unlock();
    }

private:
    mutex& mutex_;
};
