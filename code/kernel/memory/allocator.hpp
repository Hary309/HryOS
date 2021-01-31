#pragma once

#include <stddef.h>
#include <stdint.h>

#include "logger/logger.hpp"

constexpr uintptr_t align_to(uintptr_t value, uintptr_t align)
{
    return ((value + align - 1) & ~(align - 1));
}

class allocator
{
    struct block
    {
        enum class state
        {
            free,
            allocated
        };

        block* prev = nullptr;
        block* next = nullptr;
        size_t size = 0;
        state state;

        bool is_free()
        {
            return state == state::free;
        }

        uintptr_t get_data_address()
        {
            return reinterpret_cast<uintptr_t>(this) + sizeof(block);
        }

        void print()
        {
            logger::info(
                "{x} <-> {x} <-> {x} :: Size: {} :: State: {}", reinterpret_cast<uint32_t>(prev),
                reinterpret_cast<uint32_t>(this), reinterpret_cast<uint32_t>(next), size,
                state == state::allocated ? "ALLOCATED" : "FREE");
        }
    };

public:
    static constexpr uintptr_t ALIGNED = 8;

public:
    [[nodiscard]] static allocator* create(uintptr_t memory_start, uintptr_t memory_end)
    {
        if (memory_start > memory_end)
        {
            return nullptr;
        }

        size_t memory_length = memory_end - memory_start;

        if (memory_length < (sizeof(allocator) + sizeof(block)))
        {
            return nullptr;
        }

        allocator* heap = reinterpret_cast<allocator*>(memory_start);

        heap->memory_start_ = memory_start;
        heap->memory_length_ = memory_end - memory_start;

        auto* head = reinterpret_cast<block*>(memory_start + sizeof(allocator));
        auto* tail = reinterpret_cast<block*>(memory_end - sizeof(block));
        auto* free = head + 1;

        head->prev = nullptr;
        head->next = free;
        head->size = 0;
        head->state = block::state::allocated;

        tail->prev = free;
        tail->next = nullptr;
        tail->size = 0;
        tail->state = block::state::allocated;

        free->prev = head;
        free->next = tail;
        free->size = calc_size_between(tail, free);
        free->state = block::state::free;

        heap->head_ = head;
        heap->tail_ = tail;

        return heap;
    }

    uintptr_t allocate(size_t size)
    {
        size = align_to(size, ALIGNED);

        logger::info("Allocating {}...", size);

        block* block_to_allocate = find_fit(size);

        if (block_to_allocate == nullptr)
        {
            return 0;
        }

        if (block_to_allocate->size >= size + sizeof(block))
        {
            block* new_free_block = reinterpret_cast<block*>(
                reinterpret_cast<uintptr_t>(block_to_allocate) + sizeof(block) + size);
            block* next = block_to_allocate->next;

            new_free_block->next = next;
            new_free_block->prev = block_to_allocate;
            new_free_block->size = calc_size_between(next, new_free_block);
            new_free_block->state = block::state::free;

            block_to_allocate->next = new_free_block;
            block_to_allocate->size = size;

            next->prev = new_free_block;
        }

        block_to_allocate->state = block::state::allocated;

        return block_to_allocate->get_data_address();
    }

    void free(uintptr_t addr)
    {
        block* node = reinterpret_cast<block*>(addr - sizeof(block));

        logger::info("Freeing {}", node->size);

        bool found = false;

        for (auto* curr = head_; curr != nullptr; curr = curr->next)
        {
            if (node == curr)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return;
        }

        node->state = block::state::free;

        if (node->prev->is_free())
        {
            merge_with_next_block(node->prev);
        }
        else if (node->next->is_free())
        {
            merge_with_next_block(node);
        }
    }

    void print()
    {
        for (auto* node = head_; node != nullptr; node = node->next)
        {
            node->print();
        }

        logger::write_char('\n');
    }

    size_t get_size()
    {
        return memory_length_;
    }

private:
    block* find_fit(size_t size)
    {
        for (auto* node = head_->next; node != tail_; node = node->next)
        {
            if (!node->is_free())
            {
                continue;
            }

            if (node->size >= size)
            {
                return node;
            }
        }

        return nullptr;
    }

    static void merge_with_next_block(block* node)
    {
        auto* next = node->next->next;
        node->next = next;
        node->size = calc_size_between(node, next);

        next->prev = node;

        if (next->is_free())
        {
            merge_with_next_block(node);
        }
    }

    static uintptr_t calc_size_between(block* first, block* second)
    {
        if (second > first)
            return (second - first - 1) * sizeof(block);
        else
            return (first - second - 1) * sizeof(block);
    }

private:
    uintptr_t memory_start_;
    size_t memory_length_;

    block* head_;
    block* tail_;
};
