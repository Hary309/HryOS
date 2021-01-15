#pragma once

class mutex
{
public:
    mutex() : flag_(0)
    {
        __sync_lock_release(&flag_);
    }

    void lock()
    {
        while (__sync_lock_test_and_set(&flag_, 1U))
        {
            asm("pause");
        }
    }

    bool try_lock()
    {
        return (__sync_lock_test_and_set(&flag_, 1U) == 1U);
    }

    void unlock()
    {
        __sync_lock_release(&flag_);
    }

private:
    char flag_;
};
