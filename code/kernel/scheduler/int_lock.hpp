#pragma once

#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"

struct int_lock
{
    void lock()
    {
        interrupts::disable();
    }

    void unlock()
    {
        interrupts::enable();
    }
};

class direct_int_lock
{
public:
    direct_int_lock()
    {
        lock_.lock();
    }

    ~direct_int_lock()
    {
        lock_.unlock();
    }

private:
    int_lock lock_;
};
