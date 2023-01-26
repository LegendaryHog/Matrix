#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <cstddef>
#include "container.hpp"

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
    using ArrayIterator      = Container::Array<value_type>::Iterator;
    using ArrayConstIterator = Container::Array<value_type>::ConstIterator;

private:
    size_type height_ = 0, width_ = 0;
    Container::Array<value_type> data_;

    Container::Array<size_type> init_row_order() const
    {
        try 
        {
            Container::Array<size_type> row_order (height_);
            for (size_type i = 0; i < height_; i++)
                row_order[i] = i;
            return row_order;
        }
        catch (std::bad_alloc) {throw;}
    }
    Container::Array<size_type> init_col_order() const
    {
        try
        {
            Container::Array<size_type> col_order (width_);
            for (size_type i = 0; i < width_; i++)
                col_order[i] = i;
            return col_order;
        }
        catch(std::bad_alloc) {throw;}
    }

    Container::Array<size_type> row_order_ {init_row_order()};
    Container::Array<size_type> col_order_ {init_col_order()};

public:
//--------------------------------=| Classic ctors start |=---------------------------------------------
    MatrixContainer(size_type h, size_type w, value_type val = value_type{})
    :height_ {h}, width_ {w}, data_ (height_ * width_)
    {
        for (auto& elem: data_)
            elem = val;
    }

    template<std::input_iterator it>
    MatrixContainer(size_type h, size_type w, it begin, it end)
    :height_ {h}, width_ {w}, data_ (height_ * width_)
    {   
        auto itr_data = data_.begin();
        auto data_end = data_.end();
        for (auto itr_input = begin; itr_input != end && itr_data != data_end; ++itr_input, ++itr_data)
            *itr_data = *itr_input;
        for (; itr_data != data_end; ++itr_data)
            *itr_data = value_type{};
    }

    MatrixContainer(value_type val = value_type{})
    :height_ {1}, width_ {1}, data_ {val} {}

    MatrixContainer(std::initializer_list<value_type> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ (height_ * width_)
    {
        std::copy(onedim_list.begin(), onedim_list.end(), data_.begin());
    }

private:
    // sub function
    size_type calc_width(std::initializer_list<std::initializer_list<value_type>>& twodim_list)
    {
        size_type max_width = 0;
        for (auto row: twodim_list)
            if (row.size() > max_width)
                max_width = row.size();
        return max_width;
    }

public:
    MatrixContainer(std::initializer_list<std::initializer_list<value_type>> twodim_list)
    :height_ {twodim_list.size()}, width_ {calc_width(twodim_list)}, data_ (height_ * width_)
    {
        size_type act_row = 0;
        for (auto row: twodim_list)
        {
            std::copy(row.begin(), row.end(), data_.begin() + act_row * width_);
            if (row.size() < width_)
                for (auto i = row.size(); i < width_; i++)
                    data_[act_row * width_ + i] = value_type{};
            act_row++;
        }
    }
//--------------------------------=| Classic ctors end |=-----------------------------------------------

//--------------------------------=| Acces operators start |=-------------------------------------------
    size_type height() const {return height_;}
    size_type width()  const {return width_;}

    reference to(size_type i, size_type j) & noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    const_reference to(size_type i, size_type j) const& noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    value_type to(size_type i, size_type j) && noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    reference at(size_type i, size_type j) &
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    const_reference at(size_type i, size_type j) const&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    value_type at(size_type i, size_type j) &&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    class ProxyRow
    {
        Container::Array<size_type>& col_order_;
        ArrayIterator                row_itr_;
    public:
        reference operator[](size_type index)
        {
            return *(row_itr_ + col_order_[index]);
        }
    };

    class ConstProxyRow
    {
        Container::Array<size_type>& col_order_;
        ArrayConstIterator           row_itr_;
    public:
        const_reference operator[](size_type index)
        {
            return *(row_itr_ + col_order_[index]);
        }
    };

    ProxyRow operator[](size_type index) &
    {
        return ProxyRow{col_order_, data_.begin() + row_order_[index]};
    }

    ConstProxyRow operator[](size_type index) const&
    {
        return ConstProxyRow{col_order_, data_.cbegin() + row_order_[index]};
    }

    ConstProxyRow operator[](size_type index) &&
    {
        return ConstProxyRow{col_order_, data_.cbegin() + row_order_[index]};
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

        std::swap(row_order_[ind1], row_order_[ind2]);
    }

    void swap_col(size_type ind1, size_type ind2)
    {
        if (ind1 >= width() || ind2 >= width())
            throw std::out_of_range{"try to swap columns with indexis out of range"};

        std::swap(col_order_[ind1], col_order_[ind2]);
    }
//--------------------------------=| Swap rows and columns end |=---------------------------------------

//--------------------------------=| Iterators start |=-------------------------------------------------

//--------------------------------=| Iterators end |=---------------------------------------------------
};

template<typename value_type = int>
std::ostream& dump(std::ostream& os, const MatrixContainer<value_type>& mat)
{
    os << '{';
    for (auto i = 0; i < mat.height(); i++)
    {
        os << '{';
        for (auto j = 0; j < mat.width() - 1; j++)
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