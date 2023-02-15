#pragma once
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>

namespace Container
{

namespace detail
{

template<typename T>
void construct(T* p, const T& rhs) {new (p) T{rhs};}

template<typename T>
void construct(T* p, T&& rhs) {new (p) T{std::move(rhs)};}

template<class T>
void destroy(T* p) {p->~T();}

template<std::forward_iterator It>
void destroy(It first, It last)
{
    while (first != last)
        destroy(&*first++);
}

template<typename T>
class ArrayBuf
{
    using value_type = T;
    using pointer    = T*;
    using size_type  = typename std::size_t;
protected:
    size_type size_ = 0;
    pointer data_   = nullptr;

protected:
    ArrayBuf(size_type size)
    :size_ {size},
     data_ {(size_ == 0) ? nullptr : static_cast<pointer>(::operator new(sizeof(value_type) * size_))}
    {}

    ArrayBuf(const ArrayBuf&)            = delete;
    ArrayBuf& operator=(const ArrayBuf&) = delete;

    void swap(ArrayBuf& rhs) noexcept
    {
        std::swap(size_, rhs.size_);
        std::swap(data_, rhs.data_);
    }

    ArrayBuf(ArrayBuf&& rhs) noexcept
    {
        swap(rhs);
    }

    ArrayBuf& operator=(ArrayBuf&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    virtual ~ArrayBuf()
    {
        destroy(data_, data_ + size_);
        ::operator delete(data_);
    }
};

} // namespace detail

template<typename T = int>
class Array final : private detail::ArrayBuf<T>
{
    using size_type       = typename std::size_t;
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using base            = detail::ArrayBuf<T>;

    using base::data_;
    using base::size_;

//-------------------------------=| Ctors start |=------------------------------------
public:
    Array(size_type size = 0): base::ArrayBuf(size) {}

    template<std::input_iterator It>
    Array(It begin, It end): base::ArrayBuf(std::distance(begin, end))
    {
        for (size_type i = 0; i < size_; i++, begin++)
            detail::construct(data_ + i, *begin);
    }

    Array(std::initializer_list<T> initlist): base::ArrayBuf(initlist.size())
    {
        size_type i = 0;
        for (auto elem: initlist)
            detail::construct(data_ + i++, elem);
    }
//-------------------------------=| Ctors end |=--------------------------------------

//-------------------------------=| Big five start |=---------------------------------
public:
    Array(Array&&)            = default;
    Array& operator=(Array&&) = default;

    Array(const Array& rhs): base::ArrayBuf(rhs.size_)
    {
        for (size_type i = 0; i < size_; i++)
            detail::construct(data_ + i, rhs.data_[i]);
    }

    Array& operator=(const Array& rhs)
    {
        auto cpy {rhs};
        std::swap(*this, cpy);
        return *this;
    }

    ~Array() = default;
//-------------------------------=| Big five end |=-----------------------------------

//-------------------------------=| Acces op start |=---------------------------------
    size_type size() const noexcept {return size_;}

    reference       operator[](size_type index) &      noexcept {return data_[index];}
    const_reference operator[](size_type index) const& noexcept {return data_[index];}
    value_type      operator[](size_type index) &&     noexcept {return data_[index];}

    reference at(size_type index) &  
    {
        if(index >= size_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    const_reference at(size_type index) const&
    {
        if(index >= size_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    value_type at(size_type index) &&          
    {
        if(index >= size_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
//-------------------------------=| Acces op end |=-----------------------------------

//-------------------------------=| begin/end start|=---------------------------------
    class Iterator
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
        explicit Iterator(pointer ptr = nullptr)
        :ptr_ {ptr}
        {}

        reference operator*()  const noexcept {return *ptr_;}
        pointer   operator->() const noexcept {return ptr_;}

        Iterator& operator++()
        {
            ptr_++;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp {*this};
            ++(*this);
            return tmp;
        }
            
        Iterator& operator--()
        {
            ptr_--;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator tmp {*this};
            --(*this);
            return tmp;
        }

        Iterator& operator+=(const difference_type& rhs)
        {
            ptr_ += rhs;
            return *this;
        }

        Iterator& operator-=(const difference_type& rhs)
        {
            ptr_ -= rhs;
            return *this;
        }

        reference operator[](const difference_type& diff) const
        {
            return *(Iterator{*this} += diff);
        }

        std::strong_ordering operator<=>(const Iterator& rhs) const = default;

        difference_type operator-(const Iterator& rhs) const
        {
            return ptr_ - rhs.ptr_;
        }

        Iterator operator+(const difference_type& diff) const
        {
            return (Iterator{*this} += diff);
        }

        Iterator operator-(const difference_type& diff) const
        {
            return (Iterator{*this} -= diff);
        }

        friend Iterator operator+(const difference_type& diff, const Iterator& itr)
        {
            return itr + diff;
        }
    }; //class Iterator

    class ConstIterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using const_pointer     = const T*;
        using const_reference   = const T&;

    private:
        const_pointer ptr_;

    public:
        explicit ConstIterator(const_pointer ptr = nullptr)
        :ptr_ {ptr}
        {}

        const_reference operator*()  const noexcept {return *ptr_;}
        const_pointer   operator->() const noexcept {return ptr_;}

        ConstIterator& operator++()
        {
            ptr_++;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator tmp {*this};
            ++(*this);
            return tmp;
        }
        
        ConstIterator& operator--()
        {
            ptr_--;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator tmp {*this};
            --(*this);
            return tmp;
        }

        ConstIterator& operator+=(const difference_type& rhs)
        {
            ptr_ += rhs;
            return *this;
        }

        ConstIterator& operator-=(const difference_type& rhs)
        {
            ptr_ -= rhs;
            return *this;
        }

        const_reference operator[](const difference_type& diff) const
        {
            return *(ConstIterator{*this} += diff);
        }

        std::strong_ordering operator<=>(const ConstIterator& rhs) const = default;

        difference_type operator-(const ConstIterator& rhs) const
        {
            return ptr_ - rhs.ptr_;
        }

        ConstIterator operator+(const difference_type& diff) const
        {
            return (ConstIterator{*this} += diff);
        }

        ConstIterator operator-(const difference_type& diff) const
        {
            return (ConstIterator{*this} -= diff);
        }

        friend ConstIterator operator+(const difference_type& diff, const ConstIterator& itr)
        {
            return itr + diff;
        }
    }; // class ConstIterator

    Iterator begin() {return Iterator{data_};}
    Iterator end()   {return Iterator{data_ + size_};}

    ConstIterator begin() const {return ConstIterator{data_};}
    ConstIterator end()   const {return ConstIterator{data_ + size_};}

    ConstIterator cbegin() const {return ConstIterator{data_};}
    ConstIterator cend()   const {return ConstIterator{data_ + size_};}
//-------------------------------=| begin/end end|=-----------------------------------
}; // Array

} // Container
