#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <cstddef>
#include <compare>
#include "Vector/include/vector.hpp"

namespace Matrix
{
template<typename T = int>
class MatrixContainer
{
public:
    using size_type          = typename std::size_t;
    using value_type         = T;
    using reference          = T&;
    using const_reference    = const T&;
    using pointer            = T*;
    using const_pointer      = const T*;
    using VectorIterator      = typename Container::Vector<value_type>::Iterator;
    using VectorConstIterator = typename Container::Vector<value_type>::ConstIterator;
    using Iterator            = typename Container::Vector<Container::Vector<value_type>>::Iterator;
    using ConstIterator       = typename Container::Vector<Container::Vector<value_type>>::ConstIterator;

private:
    size_type height_ = 0, width_ = 0;
    Container::Vector<Container::Vector<value_type>> data_ = {};

public:
//--------------------------------=| Classic ctors start |=---------------------------------------------
    MatrixContainer(size_type h, size_type w, const_reference val)
    :height_ {h}, width_ {w}, data_ (height_, Container::Vector<value_type>(width_, val))
    {}

    MatrixContainer(size_type h, size_type w)
    :height_ {h}, width_ {w}, data_ (height_, Container::Vector<value_type>(width_))
    {}

    template<std::input_iterator it>
    MatrixContainer(size_type h, size_type w, it begin, it end)
    :height_ {h}, width_ {w}, data_ (height_, Container::Vector<value_type>(width_))
    {   
        for (auto& row: data_)
            for (auto& elem: row)
                if (begin != end)
                    elem = *begin++;
                else
                    elem = value_type{};        
    }

    MatrixContainer(value_type val = value_type{})
    :height_ {1}, width_ {1}, data_ (1, Container::Vector<value_type>{val})
    {}

    MatrixContainer(std::initializer_list<value_type> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ (height_, Container::Vector<value_type>(1))
    {
        size_type i = 0;
        for (const auto& elem: onedim_list)
            data_[i++][0] = elem;
    }

private:
    // sub function
    size_type calc_width(const std::initializer_list<std::initializer_list<value_type>>& twodim_list)
    {
        size_type max_width = 0;
        for (const auto& row: twodim_list)
            if (row.size() > max_width)
                max_width = row.size();
        return max_width;
    }

public:
    MatrixContainer(std::initializer_list<std::initializer_list<value_type>> twodim_list)
    :height_ {twodim_list.size()}, width_ {calc_width(twodim_list)},
     data_ (height_, Container::Vector<value_type>(width_))
    {
        size_type i = 0;
        for (auto& row: twodim_list)
            std::copy(row.begin(), row.end(), data[i++].begin());
    }
//--------------------------------=| Classic ctors end |=-----------------------------------------------

//--------------------------------=| Acces operators start |=-------------------------------------------
    size_type height() const {return height_;}
    size_type width()  const {return width_;}

    reference to(size_type i, size_type j) & noexcept
    {
        return data_[i][j];
    }

    const_reference to(size_type i, size_type j) const& noexcept
    {
        return data_[i][j];
    }

    value_type to(size_type i, size_type j) && noexcept
    {
        return data_[i][j];
    }

    reference at(size_type i, size_type j) &
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return to(i, j);
    }

    const_reference at(size_type i, size_type j) const&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return to(i, j);
    }

    value_type at(size_type i, size_type j) &&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return to(i, j);
    }
    
//--------------------------------=| Acces operators end |=---------------------------------------------

//--------------------------------=| Swap rows and columns start |=-------------------------------------
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                                                                         
 *----------------------------------------------------------------------------*
 *      ________________________________________________________________      *
 *---==| BE CAREFUL, THIS OPERATIONS INVALIDATE ITERATORS AND REFERENCES|==---*
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      *
 *----------------------------------------------------------------------------*
 */                              
    void swap_row(size_type ind1, size_type ind2)
    {
        if (ind1 >= height() || ind2 >= height())
            throw std::out_of_range{"try to swap rows with indexis out of range"};

        std::swap(data_[ind1], data_[ind2]);
    }

    void swap_col(size_type ind1, size_type ind2)
    {
        if (ind1 >= width() || ind2 >= width())
            throw std::out_of_range{"try to swap columns with indexis out of range"};

        for (auto& row: data_)
            std::swap(row[ind1], row[ind2]);
    }
//--------------------------------=| Swap rows and columns end |=---------------------------------------

//--------------------------------=| Iterators start |=-------------------------------------------------

    Iterator begin() noexcept {return Iterator {*this, 0, 0};}
    Iterator end()   noexcept {return Iterator {*this, height_, 0};}

    ConstIterator begin() const noexcept {return ConstIterator {*this, 0, 0};}
    ConstIterator end()   const noexcept {return ConstIterator {*this, height_, 0};}

    ConstIterator cbegin() const noexcept {return ConstIterator {*this, 0, 0};}
    ConstIterator cend()   const noexcept {return ConstIterator {*this, height_, 0};}
//--------------------------------=| Iterators end |=---------------------------------------------------
};

template<typename value_type = int>
std::ostream& dump(std::ostream& os, const MatrixContainer<value_type>& mat)
{
    os << '{';
    for (std::size_t i = 0; i < mat.height(); i++)
    {
        os << '{';
        for (std::size_t j = 0; j < mat.width() - 1; j++)
            os << mat.to(i, j) << ' ';
        os << mat.to(i, mat.width() - 1) << '}';
    }
    os << '}';
    return os;
}

template<typename value_type = int>
std::ostream& operator<<(std::ostream& os, const MatrixContainer<value_type>& mat)
{
    return dump(os, mat);
}

} // Matrix