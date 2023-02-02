#pragma once
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "container_iterator.hpp"

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
    explicit Array(size_type size = 0)
    :size_ {size}, data_ {new value_type[size_]{}}
    {}

private:
    // sub func
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

    Array(std::initializer_list<T> list)
    :size_ {list.size()}, data_ {new value_type[size_]}
    {
        std::copy(list.begin(), list.end(), data_);
    }
//-------------------------------=| Ctors end |=--------------------------------------

//-------------------------------=| Big five start |=---------------------------------
private:
    void swap(Array& arr) noexcept
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
using Iterator =      ArrayIterator<T>;
using ConstIterator = ArrayConstIterator<T>;

Iterator begin() & {return Iterator{data_};}
Iterator end()   & {return Iterator{data_ + size_};}

ConstIterator begin() const& {return ConstIterator{data_};}
ConstIterator end()   const& {return ConstIterator{data_ + size_};}

ConstIterator cbegin() const& {return ConstIterator{data_};}
ConstIterator cend()   const& {return ConstIterator{data_ + size_};}
//-------------------------------=| begin/end end|=-----------------------------------
}; // Array

} // Container
