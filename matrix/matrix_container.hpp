#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <cstddef>

namespace Matrix
{

template<typename T = int>
class MatrixContainer
{
    using size_t = std::size_t;

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
//------------------------=| Classic ctors start |=--------------------------
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
//------------------------=| Classic ctors end |=----------------------------

//------------------------=| Big five start |=-------------------------------

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
//------------------------=| Big five end |=---------------------------------

//------------------------=| Acces operators start |=------------------------
    size_t height() const {return height_;}
    size_t width()  const {return width_;}

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

private:
    class ProxyRow
    {
        T* row_ = nullptr;
        size_t* col_order_in_row_ = nullptr;
    public:
        ProxyRow(const MatrixContainer& mat, size_t row_ind)
        :row_ {mat.data_ + mat.row_order_[row_ind]}, col_order_in_row_ {mat.col_order_}
        {}

        T& operator[](size_t ind) & noexcept
        {
            return row_[col_order_in_row_[ind]];
        }

        const T& operator[](size_t ind) const & noexcept
        {
            return row_[col_order_in_row_[ind]];
        }

        T operator[](size_t ind) && noexcept
        {
            return row_[col_order_in_row_[ind]];
        }
    };

public:
    ProxyRow operator[](size_t ind) & noexcept
    {
        return ProxyRow {*this, ind};
    }

    const ProxyRow operator[](size_t ind) const& noexcept
    {
        return ProxyRow {*this, ind};
    }

    ProxyRow operator[](size_t ind) && noexcept
    {
        return ProxyRow {*this, ind};
    }
//------------------------=| Acces operators end |=--------------------------

//------------------------=| Iterators start |=------------------------------

    struct Iterator 
    {
        using iterator_category = std::random_access_iterator;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

    private:
        pointer data_ = nullptr;
        size_t i_, j_ = 0;
        size_t width_ = 0;
        size_t* row_order_ = nullptr;
        size_t* col_order_ = nullptr;

    public:
        Iterator(const MatrixContainer& mat, size_t i, size_t j)
        :data_ {mat.data_}, i_ {i}, j_ {j}, width_ {mat.width_}, row_order_ {mat.row_order_}, col_order_ {mat.col_order_}

        reference operator*() & noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        reference operator*() & noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        pointer operator->() & noexcept
        {
            return &data_[row_order_[i_] * width_ + col_order_[j_]];
        }

        const pointer operator->() const& noexcept
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
                if (i_ == 0)
                    return *this;
                else
                {
                    i_--;
                    j_ = width_ - 1;
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

        friend bool operator== (const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.data_ == rhs.data_ && lhs.i_ == rhs.i_ && lhs.j_ && rhs.j_;
        }

        friend bool operator!= (const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs == rhs);
        }   
    };

    Iterator begin() & {return Iterator {*this, 0, 0};}
    Iterator end()   & {return Iterator {*this, height_, 0};}
    const Iterator cbegin() const& {return Iterator {*this, 0, 0};}
    const Iterator cend()   const& {return Iterator {*this, height_, 0};}
//------------------------=| Iterators end |=--------------------------------
};


template<typename T = int>
std::ostream& dump(std::ostream& os, const MatrixContainer<T>& mat)
{
    os << '{';
    for (auto i = 0; i < height(); i++)
    {
        os << '{';
        for (auto j = 0; j < width() - 1; j++)
            os << mat.to(i, j) << ' ';
        os << mat.to(i, width() - 1) << '}';
    }
    os << '}';
    return os;
}

template<typename T = int>
std::ostream& operator<<(std::ostream& os, const MatrixContainer<T>& mat)
{
    return dump(os, mat);
}

} // Matrix