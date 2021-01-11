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

class weak_int_lock
{
public:
    weak_int_lock()
    {
        was_enabled_ = interrupts::enabled();
        lock_.lock();
    }

    ~weak_int_lock()
    {
        if (was_enabled_)
        {
            lock_.unlock();
        }
    }

private:
    bool was_enabled_ = false;
    int_lock lock_;
};
