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
    using size_type       = typename std::size_t;
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;

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
        std::copy(onedim_list.begin(), onedim_list.end(), data_);
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
            std::copy(row.begin(), row.end(), data_ + act_row * width_);
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
    class Iterator 
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
    private:
        pointer data_ = nullptr;
        size_type i_ = 0, j_ = 0;
        size_type width_ = 0;
        size_type* row_order_ = nullptr;
        size_type* col_order_ = nullptr;

        difference_type diff_with_begin() const
        {
            return static_cast<difference_type>(i_ * width_ + j_);
        }

        void convert_from_diff_with_begin(const difference_type& diff)
        {
            if (diff < 0)
                return;
            i_ = diff / width_;
            j_ = diff % width_;
        }

    public:
        Iterator(const MatrixContainer& mat, size_type i, size_type j)
        :data_ {mat.data_}, i_ {i}, j_ {j}, width_ {mat.width_}, row_order_ {mat.row_order_}, col_order_ {mat.col_order_} {}

        Iterator() = default;

        reference operator*() const& noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        pointer operator->() const& noexcept
        {
            return &data_[row_order_[i_] * width_ + col_order_[j_]];
        }

        Iterator& operator++()
        {
            j_++;
            if (j_ == width_)
            {
                j_ = 0;
                i_++;
            }
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
            if (j_ == 0)
            {
                if (i_ == 0)
                    return *this;
                else
                {
                    i_--;
                    j_ = width_ - 1;
                }
            }
            j_--;
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
            convert_from_diff_with_begin(diff_with_begin() + rhs);
            return *this;
        }

        Iterator& operator-=(const difference_type& rhs)
        {
            convert_from_diff_with_begin(diff_with_begin() - rhs);
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
            return lhs.diff_with_begin() - rhs.diff_with_begin();
        }

        reference operator[](const difference_type& diff) const
        {
            Iterator new_itr {*this};
            return *(new_itr += diff);
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.data_ == rhs.data_ && lhs.i_ == rhs.i_ && lhs.j_ == rhs.j_;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const Iterator& lhs, const Iterator& rhs)
        {
            if (lhs.i_ < rhs.i_)
                return true;
            else if (lhs.i_ == rhs.i_)
                return lhs.j_ < rhs.j_;
            else
                return false;
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
    };

    Iterator begin() & {return Iterator {*this, 0, 0};}
    Iterator end()   & {return Iterator {*this, height_, 0};}

    class ConstIterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using const_pointer     = const T*;
        using const_reference   = const T&;
    private:
        const_pointer data_ = nullptr;
        size_type i_, j_ = 0;
        size_type width_ = 0;
        size_type* row_order_ = nullptr;
        size_type* col_order_ = nullptr;

        difference_type diff_with_begin() const
        {
            return static_cast<difference_type>(i_ * width_ + j_);
        }

        void convert_from_diff_with_begin(const difference_type& diff)
        {
            if (diff < 0)
                return;
            i_ = diff / width_;
            j_ = diff % width_;
        }
    public:
        ConstIterator(const MatrixContainer& mat, size_type i, size_type j)
        :data_ {mat.data_}, i_ {i}, j_ {j}, width_ {mat.width_}, row_order_ {mat.row_order_}, col_order_ {mat.col_order_} {}

        ConstIterator() = default;

        const_reference operator*() const& noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        const_pointer operator->() const& noexcept
        {
            return &data_[row_order_[i_] * width_ + col_order_[j_]];
        }

        ConstIterator& operator++()
        {
            j_++;
            if (j_ == width_)
            {
                j_ = 0;
                i_++;
            }
            return *this;
        }
    
        ConstIterator operator++(int)
        {
            Iterator tmp {*this};
            ++(*this);
            return tmp;
        }

        ConstIterator& operator--()
        {
            if (j_ == 0)
            {
                if (i_ == 0)
                    return *this;
                else
                {
                    i_--;
                    j_ = width_ - 1;
                }
            }
            j_--;
            return *this;
        }
    
        ConstIterator operator--(int)
        {
            Iterator tmp {*this};
            --(*this);
            return tmp;
        }

        ConstIterator& operator+=(const difference_type& rhs)
        {
            convert_from_diff_with_begin(diff_with_begin() + rhs);
            return *this;
        }

        ConstIterator& operator-=(const difference_type& rhs)
        {
            convert_from_diff_with_begin(diff_with_begin() - rhs);
            return *this;
        }

        friend ConstIterator operator+(const ConstIterator& itr, const difference_type& diff)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend ConstIterator operator+(const difference_type& diff, const ConstIterator& itr)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy += diff);
        }

        friend ConstIterator operator-(const ConstIterator& itr, const difference_type& diff)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend ConstIterator operator-(const difference_type& diff, const ConstIterator& itr)
        {
            Iterator itr_cpy {itr};
            return (itr_cpy -= diff);
        }

        friend difference_type operator-(const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return lhs.diff_with_begin() - rhs.diff_with_begin();
        }

        const_reference operator[](const difference_type& diff) const
        {
            Iterator new_itr {*this};
            return *(new_itr += diff);
        }

        bool operator==(const ConstIterator& rhs) const
        {
            return data_ == rhs.data_ && i_ == rhs.i_ && j_ == rhs.j_;
        }

        bool operator!=(const ConstIterator& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const ConstIterator& rhs) const
        {
            if (i_ < rhs.i_)
                return true;
            else if (i_ == rhs.i_)
                return j_ < rhs.j_;
            else
                return false;
        }

        bool operator<=(const ConstIterator& rhs) const
        {
            return (*this == rhs) || (*this < rhs);
        }

        bool operator>(const ConstIterator& rhs) const
        {
            return !(*this <= rhs);
        }

        bool operator>=(const ConstIterator& rhs) const
        {
            return !(this < rhs);
        }
    };

    ConstIterator begin() const& {return ConstIterator {*this, 0, 0};}
    ConstIterator end()   const& {return ConstIterator {*this, height_, 0};}
    ConstIterator cbegin() const& {return ConstIterator {*this, 0, 0};}
    ConstIterator cend()   const& {return ConstIterator {*this, height_, 0};}
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