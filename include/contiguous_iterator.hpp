#pragma once
#include <iterator>

namespace Container
{
namespace detail
{
template<typename T>
class ContiguousIterator
{
public:
    using iterator_category = typename std::random_access_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

private:
    pointer ptr_;
    
public:
    explicit ContiguousIterator(pointer ptr = nullptr)
    :ptr_ {ptr}
    {}

    reference operator*()  const noexcept {return *ptr_;}
    pointer   operator->() const noexcept {return ptr_;}

    ContiguousIterator& operator++()
    {
        ptr_++;
        return *this;
    }

    ContiguousIterator operator++(int)
    {
        ContiguousIterator tmp {*this};
        ++(*this);
        return tmp;
    }
        
    ContiguousIterator& operator--()
    {
        ptr_--;
        return *this;
    }

    ContiguousIterator operator--(int)
    {
        ContiguousIterator tmp {*this};
        --(*this);
        return tmp;
    }

    ContiguousIterator& operator+=(const difference_type& rhs)
    {
        ptr_ += rhs;
        return *this;
    }

    ContiguousIterator& operator-=(const difference_type& rhs)
    {
        ptr_ -= rhs;
        return *this;
    }

    reference operator[](const difference_type& diff) const
    {
        return *(ContiguousIterator{*this} += diff);
    }

    difference_type operator-(const ContiguousIterator& rhs) const
    {
        return ptr_ - rhs.ptr_;
    }

    std::strong_ordering operator<=>(const ContiguousIterator&) const = default;
}; // class ContiguousIterator

template<typename T>
ContiguousIterator<T> operator+(const ContiguousIterator<T>& itr, const typename ContiguousIterator<T>::difference_type& diff)
{
    return (ContiguousIterator{itr} += diff);
}

template<typename T>
ContiguousIterator<T> operator+(const typename ContiguousIterator<T>::difference_type& diff, const ContiguousIterator<T>& itr)
{
    return itr + diff;
}

template<typename T>
ContiguousIterator<T> operator-(const ContiguousIterator<T>& itr, const typename ContiguousIterator<T>::difference_type& diff)
{
    return (ContiguousIterator{itr} -= diff);
}
} // namespace detail
} // namespace Container