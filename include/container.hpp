#pragma once
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <cstddef>

namespace Container
{

template<typename T = int>
class Array
{
    using size_type       = typename std::size_t;
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    size_type size_ = 0;
    pointer   data_ = nullptr; 


//-------------------------------=| Ctors start |=------------------------------------
public:
    Array(size_type size = 0)
    :size_ {size}, data_ {new value_type[size_]{}}
    {}

private:
    template<std::input_iterator it>
    size_type calc_size(it begin, it end)
    {
        size_type size = 0;
        for (it itr = begin; itr != end; ++itr, size++) {}
        return size;
    }

public:
    template<std::input_iterator it>
    Array(it begin, it end)
    :size_ {calc_size(begin, end)}, data_ {new value_type[size_]}
    {
        std::copy(begin, end, data_);
    }

    Array(std::initializer_list<value_type>& list)
    :size_ {list.szie()}, data_ {new value_type[size_]}
    {
        std::copy(list.begin(), list.end(), data_);
    }
//-------------------------------=| Ctors end |=--------------------------------------

//-------------------------------=| Big five start |=---------------------------------
private:
    void swap(const Array& arr) noexcept
    {
        std::swap(data_, arr.data_);
        std::swap(size_, arr.size_);
    }

public:
    Array(const Array& rhs)
    :size_ {rhs.size_}, data_ {new value_type[size_]}
    {
        std::copy(rhs.data_, rhs.data_ + rhs.size_, data_);
    }

    Array(Array&& rhs) noexcept
    {
        swap(rhs);
    }

    Array& operator=(const Array& rhs)
    {
        Array rhs_cpy {rhs};
        swap(rhs_cpy);
        return *this;
    }

    Array& operator=(Array&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    virtual ~Array()
    {
        delete[] data_;
    }
//-------------------------------=| Big five end |=-----------------------------------

//-------------------------------=| Acces op start |=---------------------------------
    size_type size() const noexcept {return size_;}

    reference       operator[](size_type index) & noexcept      {return data_[index];}
    const_reference operator[](size_type index) const& noexcept {return data_[index];}
    value_type      operator[](size_type index) && noexcept     {return data_[index];}

    reference at(size_type index) & noexcept      
    {
        if(index >= size_)
            std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    const_reference at(size_type index) const& noexcept 
    {
        if(index >= size_)
            std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    value_type at(size_type index) && noexcept            
    {
        if(index >= size_)
            std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
//-------------------------------=| Acces op end |=-----------------------------------

//-------------------------------=| Iterator start |=---------------------------------
    class Iterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

    private:
        pointer ptr_ = nullptr;
    
    public:
        reference operator*()  const& noexcept {return *ptr_;}
        pointer   operator->() const& noexcept {return ptr_;}

        Iterator& operator++()
        {
            ptr_++;
            return ptr_;
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
            return ptr_;
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

        friend Iterator operator+(const Iterator& itr, const difference_type& diff)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend Iterator operator+(const difference_type& diff, const Iterator& itr)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend Iterator operator-(const Iterator& itr, const difference_type& diff)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend Iterator operator-(const difference_type& diff, const Iterator& itr)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend difference_type operator-(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.ptr_ - rhs.ptr_;
        }

        reference operator[](const difference_type& diff) const
        {
            Iterator new_itr {*this};
            return *(new_itr += diff);
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.ptr < rhs.ptr;
        }

        friend bool operator<=(const Iterator& lhs, const Iterator& rhs)
        {
            return (lhs == rhs) || (lhs < rhs);
        }

        friend bool operator>(const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs <= rhs);
        }

        friend bool operator>=(const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs < rhs);
        }
    }; // Iterator

    class ConstIterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using const_pointer     = const T*;
        using const_reference   = const T&;

    private:
        const_pointer ptr_ = nullptr;
    
    public:
        const_reference operator*()  const& noexcept {return *ptr_;}
        const_pointer   operator->() const& noexcept {return ptr_;}

        ConstIterator& operator++()
        {
            ptr_++;
            return ptr_;
        }

        CosntIterator operator++(int)
        {
            Iterator tmp {*this};
            ++(*this);
            return tmp;
        }
        
        ConstIterator& operator--()
        {
            ptr_--;
            return ptr_;
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

        friend ConstIterator operator+(const ConstIterator& itr, const difference_type& diff)
        {
            ConstIterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend ConstIterator operator+(const difference_type& diff, const ConstIterator& itr)
        {
            ConstIterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend ConstIterator operator-(const ConstIterator& itr, const difference_type& diff)
        {
            ConstIterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend ConstIterator operator-(const difference_type& diff, const ConstIterator& itr)
        {
            ConstIterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend difference_type operator-(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return lhs.ptr_ - rhs.ptr_;
        }

        const_reference operator[](const difference_type& diff) const
        {
            ConstIterator new_itr {*this};
            return *(new_itr += diff);
        }

        friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return lhs.ptr < rhs.ptr;
        }

        friend bool operator<=(const CosntIterator& lhs, const ConstIterator& rhs)
        {
            return (lhs == rhs) || (lhs < rhs);
        }

        friend bool operator>(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return !(lhs <= rhs);
        }

        friend bool operator>=(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return !(lhs < rhs);
        }
    }; // ConstIterator
//-------------------------------=| Iterator end |=-----------------------------------

//-------------------------------=| begin/end start|=---------------------------------
Iterator begin() & {return Iterator{data_};}
Iterator end()   & {return Iterator{data_ + size_};}

ConstIterator begin() const& {return ConstIterator{data_};}
ConstIterator end()   const& {return ConstIterator{data_ + size_};}

ConstIterator cbegin() const& {return ConstIterator{data_};}
ConstIterator cend()   const& {return ConstIterator{data_ + size_};}
//-------------------------------=| begin/end end|=-----------------------------------
}; // Array

} // Container
