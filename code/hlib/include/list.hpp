#pragma once

namespace hlib
{
    template<typename T, bool StoreErased = false>
    class list
    {
    public:
        using reference = T&;
        using const_reference = const T&;

    public:
        struct node
        {
            explicit node(const T& v) : value(v)
            {
            }

            node* next = nullptr;
            node* prev = nullptr;
            T value{};
        };

        class iterator
        {
            friend list;

        public:
            using reference = T&;
            using const_reference = T&;
            using pointer = T*;
            using const_pointer = const T*;

            iterator() : node_(nullptr)
            {
            }

            iterator(node* node) : node_(node)
            {
            }

            iterator(const iterator& other) : node_(other.node_)
            {
            }

            iterator& operator++()
            {
                node_ = node_->next;
                return *this;
            }

            iterator operator++(int)
            {
                iterator temp(*this);
                node_ = node_->next;
                return temp;
            }

            iterator& operator--()
            {
                node_ = node_->previous;
                return *this;
            }

            iterator operator--(int)
            {
                iterator temp(*this);
                node_ = node_->previous;
                return temp;
            }

            iterator& operator=(const iterator& other)
            {
                node_ = other.node_;
                return *this;
            }

            reference operator*()
            {
                return node_->value;
            }

            const_reference operator*() const
            {
                return node_->value;
            }

            pointer operator&()
            {
                return &node_->value;
            }

            const_pointer operator&() const
            {
                return &node_->value;
            }

            pointer operator->()
            {
                return &node_->value;
            }

            const_pointer operator->() const
            {
                return &node_->value;
            }

            friend bool operator==(const iterator& lhs, const iterator& rhs)
            {
                return lhs.node_ == rhs.node_;
            }

            friend bool operator!=(const iterator& lhs, const iterator& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            node* node_;
        };

        class const_iterator
        {
            friend list;

        public:
            using reference = T&;
            using const_reference = T&;
            using pointer = T*;
            using const_pointer = const T*;

            const_iterator() : node_(nullptr)
            {
            }

            const_iterator(node* node) : node_(node)
            {
            }

            const_iterator(const node* node) : node_(node)
            {
            }

            const_iterator(const iterator& other) : node_(other.node_)
            {
            }

            const_iterator(const const_iterator& other) : node_(other.node_)
            {
            }

            const_iterator& operator++()
            {
                node_ = node_->next;
                return *this;
            }

            const_iterator operator++(int)
            {
                const_iterator temp(*this);
                node_ = node_->next;
                return temp;
            }

            const_iterator& operator--()
            {
                node_ = node_->previous;
                return *this;
            }

            const_iterator operator--(int)
            {
                const_iterator temp(*this);
                node_ = node_->previous;
                return temp;
            }

            const_iterator& operator=(const const_iterator& other)
            {
                node_ = other.node_;
                return *this;
            }

            const_reference operator*() const
            {
                return node_->valu;
            }

            const_pointer operator&() const
            {
                return &node_->value;
            }

            const_pointer operator->() const
            {
                return &node_->value;
            }

            friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
            {
                return lhs.node_ == rhs.node_;
            }

            friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            const node* node_;
        };

    public:
        ~list()
        {
            node* curr = head_;
            node* next = nullptr;

            while (curr)
            {
                next = curr->next;
                delete curr;
                curr = next;
            }

            head_ = nullptr;
            tail_ = nullptr;
        }

        T* push_back(const T& value)
        {
            auto* n = get_free_node(value);

            if (tail_ == nullptr)
            {
                head_ = n;
            }
            else
            {
                join(tail_, n);
            }

            tail_ = n;

            return &n->value;
        }

        T* push_front(const T& value)
        {
            auto* n = get_free_node(value);

            if (head_ == nullptr)
            {
                tail_ = n;
            }
            else
            {
                join(n, head_);
            }

            head_ = n;

            return &n->value;
        }

        void pop_front()
        {
            erase(head_);
        }

        void pop_back()
        {
            erase(tail_);
        }

        void erase(iterator it)
        {
            node* n = it.node_;

            auto* next = n->next;
            auto* prev = n->prev;

            if (next)
                next->prev = prev;

            if (prev)
                prev->next = next;

            if (n == head_)
                head_ = next;

            if (n == tail_)
                tail_ = prev;

            if constexpr (StoreErased)
            {
                n->next = free_;
                n->prev = nullptr;
                free_ = n;
            }
            else
            {
                delete n;
            }
        }

        template<typename Func>
        void erase_if(Func func)
        {
            auto it = begin();

            while (it != end())
            {
                if (func(it.node_->value))
                {
                    auto to_delete = it;
                    it++;
                    erase(to_delete);
                    continue;
                }

                it++;
            }
        }

        iterator begin()
        {
            return iterator(head_);
        }

        iterator end()
        {
            return tail_ == nullptr ? nullptr : iterator(tail_->next);
        }

        const_iterator begin() const
        {
            return const_iterator(head_);
        }

        const_iterator end() const
        {
            return const_iterator(tail_->next);
        }

        bool empty()
        {
            return head_ == nullptr || tail_ == nullptr;
        }

        reference front()
        {
            return head_->value;
        }

        const_reference front() const
        {
            return head_->value;
        }

        reference back()
        {
            return tail_->value;
        }

        const_reference back() const
        {
            return tail_->value;
        }

    private:
        node* get_free_node(const T& value)
        {
            if constexpr (StoreErased)
            {
                if (free_ != nullptr)
                {
                    node* result = free_;

                    free_ = free_->next;

                    result->value = hlib::move(value);
                    result->next = nullptr;
                    result->prev = nullptr;

                    return result;
                }
            }

            return new node(value);
        }
        void join(node* left, node* right)
        {
            left->next = right;
            right->prev = left;
        }

    private:
        node* head_ = nullptr;
        node* tail_ = nullptr;
        node* free_ = nullptr;
    };
} // namespace hlib
