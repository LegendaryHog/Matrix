#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace Matrix
{

template<typename T = int>
class MatrixContainer
{
    using std::size_t = size_t;

    size_t height_ = 0, width_ = 0;
    T* data_ = nullptr;

    size_t* init_row_order() const
    {
        size_t* row_order = new size_t[height_];
        for (size_t i = 0; i < height_; i++)
            row_order[i] = i;
        return row_order;
    }
    size_t* init_col_order() const
    {
        size_t* col_order = new size_t[width_];
        for (size_t i = 0; i < width_; i++)
            col_order[i] = i;
        return col_order;
    }

    size_t* row_order_ = init_row_order();
    size_t* col_order_ = init_col_order();

public:
//------------------------=| Classic ctors start |=------------------------
    MatrixContainer(size_t h, size_t w, T val = T{})
    :height_ {h}, width_ {w}, data_ {new T[height_ * width_]}
    {
        for (size_t i = 0; i < height_ * width_; i++)
            data_[i] = val;
    }

    template<std::input_iterator it>
    MatrixContainer(size_t h, size_t w, it begin, it end)
    :height_ {h}, width_ {w}, data_ {new T[height_ * width_]}
    {
        if (height_ < 0 || width_ < 0)
            throw std::length_error{"In constructor of matrix: height_ or width_ less null"};
        
        auto i = 0;
        for (auto itr = begin; i < height_ * width_ && itr != end; i++, ++itr)
            data_[i] = *itr;
        for (; i < height_ * width_; i++)
            data_[i] = T{};   
    }

    MatrixContainer(T val = T{})
    :height_ {1}, width_ {1}, data_ {new T[1]{val}} {}

    MatrixContainer(std::initializer_list<T> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ {new T[height_ * width_]}
    {
        std::copy(onedim_list.begin(), onedim_list.end(), data_);
    }

    MatrixContainer(std::initializer_list<std::initializer_list<T>> twodim_list)
    :height_ {twodim_list.size()}
    {
        if (height_ == 0)
            throw std::logic_error{"Never know why - Ozzy Osbourne"};
        
        size_t max_width_ = 0;
        for (auto row: twodim_list)
            if (row.size() > max_width_)
                max_width_ = row.size();

        width_ = max_width_;
        data_ = new T[height_ * width_];
        size_t act_row = 0;
        for (auto row: twodim_list)
        {
            std::copy(row.begin(), row.end(), data_ + act_row * width_);
            if (row.size() < width_)
                for (auto i = row.size(); i < width_; i++)
                    data_[act_row * width_ + i] = T{};
            act_row++;
        }
    }
//------------------------=| Classic ctors end |=--------------------------

//------------------------=| Big five start |=-----------------------------

    MatrixContainer(const MatrixContainer& rhs)
    :height_ {rhs.height_}, width_ {rhs.width_}, data_ {new T[height_ * width_]},
     row_order_ {new size_t[height_]}, col_order_ {new size_t[width_]}
    {
        std::copy(rhs.data_, rhs.data_ + height_ * width_, data_);
        std::copy(rhs.row_order_, rhs.row_order_ + height_, row_order_);
        std::copy(rhs.col_order_, rhs.col_order_ + width_, col_order_);
    }

    MatrixContainer(MatrixContainer&& rhs)
    :height_ {rhs.height_}, width_ {rhs.width_}, data_ {new T[height_ * width_]},
     row_order_ {rhs.row_order_}, col_order_ {rhs.col_order_}
    {
        height_ = 0;
        width_  = 0;
        data_ = nullptr;
        row_order_ = nullptr;
        col_order_ = nullptr;
    }

    MatrixContainer& operator=(const MatrixContainer& rhs)
    {
        MatrixContainer rhs_cpy {rhs};
        
        std::swap(rhs_cpy.height_, height_);
        std::swap(rhs_cpy.width_, width_);
        std::swap(rhs_cpy.data_, data_);
        std::swap(rhs_cpy.row_order_, row_order_);
        std::swap(rhs_cpy.col_order_, col_order_);

        return *this;
    }

    MatrixContainer& operator=(MatrixContainer&& rhs)
    {
        std::swap(rhs.height_, height_);
        std::swap(rhs.width_, width_);
        std::swap(rhs.data_, data_);
        std::swap(rhs.row_order_, row_order_);
        std::swap(rhs.col_order_, col_order_);

        return *this;
    }

    virtual ~MatrixContainer()
    {
        delete[] data_;
        delete[] row_order_;
        delete[] col_order_;
    }
//------------------------=| Big five end|=--------------------------------

//------------------------=| Acces operators start |=----------------------

    T& to(size_t i, size_t j) & noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    const T& to(size_t i, size_t j) const& noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    T to(size_t i, size_t j) && noexcept
    {
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    T& at(size_t i, size_t j) &
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    const T& at(size_t i, size_t j) const&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }

    T at(size_t i, size_t j) &&
    {
        if (i >= height_ || j >= width_)
            throw std::out_of_range{"try to get access to element out of matrix"};
        return data_[row_order_[i] * width_ + col_order_[j]];
    }
};

}