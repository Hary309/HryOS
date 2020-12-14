#pragma once

class mutex
{
public:
    bool try_locK()
    {
        return __sync_bool_compare_and_swap(&blocked_, false, true);
    }

    void spinlock()
    {
        while (!try_locK())
        {
            asm("pause");
        }
    }

    void unlock()
    {
        blocked_ = false;
    }

private:
    volatile bool blocked_ = false;
};
