#include <iterator>

namespace Container
{

template<typename T = int>
class ArrayIterator
{
public:
    using iterator_category = typename std::contiguous_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

private:
    pointer ptr_;
    
public:
    ArrayIterator(pointer ptr = nullptr)
    :ptr_ {ptr}
    {}

    reference operator*()  const noexcept {return *ptr_;}
    pointer   operator->() const noexcept {return ptr_;}

    ArrayIterator& operator++()
    {
        ptr_++;
        return *this;
    }

    ArrayIterator operator++(int)
    {
        ArrayIterator tmp {*this};
        ++(*this);
        return tmp;
    }
        
    ArrayIterator& operator--()
    {
        ptr_--;
        return *this;
    }

    ArrayIterator operator--(int)
    {
        ArrayIterator tmp {*this};
        --(*this);
        return tmp;
    }

    ArrayIterator& operator+=(const difference_type& rhs)
    {
        ptr_ += rhs;
        return *this;
    }

    ArrayIterator& operator-=(const difference_type& rhs)
    {
        ptr_ -= rhs;
        return *this;
    }

    reference operator[](const difference_type& diff) const
    {
        return *(ArrayIterator{*this} += diff);
    }

    std::strong_ordering operator<=>(const ArrayIterator& rhs) const = default;

    difference_type operator-(const ArrayIterator<T>& rhs) const
    {
        return ptr_ - rhs.ptr_;
    }

}; //class ArrayIterator

template<typename T = int>
ArrayIterator<T> operator+(const ArrayIterator<T>& itr, const typename ArrayIterator<T>::difference_type& diff)
{
    return (ArrayIterator{itr} += diff);
}

template<typename T = int>
ArrayIterator<T> operator+(const typename ArrayIterator<T>::difference_type& diff, const ArrayIterator<T>& itr)
{
    return (ArrayIterator{itr} += diff);
}

template<typename T = int>
ArrayIterator<T> operator-(const ArrayIterator<T>& itr, const typename ArrayIterator<T>::difference_type& diff)
{
    return (ArrayIterator{itr} -= diff);
}

template<typename T = int>
ArrayIterator<T> operator-(const typename ArrayIterator<T>::difference_type& diff, const ArrayIterator<T>& itr)
{
    return (ArrayIterator{itr} -= diff);
}

template<typename T = int>
class ArrayConstIterator
{
public:
    using iterator_category = typename std::contiguous_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = T;
    using const_pointer     = const T*;
    using const_reference   = const T&;

private:
    const_pointer ptr_;

public:
    ArrayConstIterator(const_pointer ptr = nullptr)
    :ptr_ {ptr}
    {}

    const_reference operator*()  const noexcept {return *ptr_;}
    const_pointer   operator->() const noexcept {return ptr_;}

    ArrayConstIterator& operator++()
    {
        ptr_++;
        return *this;
    }

    ArrayConstIterator operator++(int)
    {
        ArrayConstIterator tmp {*this};
        ++(*this);
        return tmp;
    }
    
    ArrayConstIterator& operator--()
    {
        ptr_--;
        return *this;
    }

    ArrayConstIterator operator--(int)
    {
        ArrayConstIterator tmp {*this};
        --(*this);
        return tmp;
    }

    ArrayConstIterator& operator+=(const difference_type& rhs)
    {
        ptr_ += rhs;
        return *this;
    }

    ArrayConstIterator& operator-=(const difference_type& rhs)
    {
        ptr_ -= rhs;
        return *this;
    }

    const_reference operator[](const difference_type& diff) const
    {
        return *(ArrayConstIterator{*this} += diff);
    }

    std::strong_ordering operator<=>(const ArrayConstIterator& rhs) const = default;

    difference_type operator-(const ArrayConstIterator& rhs) const
    {
        return ptr_ - rhs.ptr_;
    }
}; // class ArrayConstIterator

template<typename T = int>
ArrayConstIterator<T> operator+(const ArrayConstIterator<T>& itr, const typename ArrayConstIterator<T>::difference_type& diff)
{
    return (ArrayConstIterator{itr} += diff);
}

template<typename T = int>
ArrayConstIterator<T> operator+(const typename ArrayConstIterator<T>::difference_type& diff, const ArrayConstIterator<T>& itr)
{
    return (ArrayConstIterator{itr} += diff);
}

template<typename T = int>
ArrayConstIterator<T> operator-(const ArrayConstIterator<T>& itr, const typename ArrayConstIterator<T>::difference_type& diff)
{
    return (ArrayConstIterator{itr} -= diff);
}

template<typename T = int>
ArrayConstIterator<T> operator-(const typename ArrayConstIterator<T>::difference_type& diff, const ArrayConstIterator<T>& itr)
{
    return (ArrayConstIterator{itr} -= diff);
}

} // namespace Container