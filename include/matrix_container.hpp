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

    using size_type         = typename std::size_t;
    using iterator_category = typename std::random_access_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;

private:
    size_type height_ = 0, width_ = 0;
    pointer data_ = nullptr;

    size_type* init_row_order() const
    {
        try 
        {
            size_type* row_order = new size_type[height_];
            for (size_type i = 0; i < height_; i++)
                row_order[i] = i;
            return row_order;
        }
        catch (std::bad_alloc) {throw;}
    }
    size_type* init_col_order() const
    {
        try
        {
            size_type* col_order = new size_type[width_];
            for (size_type i = 0; i < width_; i++)
                col_order[i] = i;
            return col_order;
        }
        catch(std::bad_alloc) {throw;}
    }

    size_type* row_order_ = init_row_order();
    size_type* col_order_ = init_col_order();

public:
//--------------------------------=| Classic ctors start |=---------------------------------------------
    MatrixContainer(size_type h, size_type w, value_type val = value_type{})
    :height_ {h}, width_ {w}, data_ {new value_type[height_ * width_]}
    {
        for (size_type i = 0; i < height_ * width_; i++)
            data_[i] = val;
    }

    template<std::input_iterator it>
    MatrixContainer(size_type h, size_type w, it begin, it end)
    :height_ {h}, width_ {w}, data_ {new value_type[height_ * width_]}
    {   
        auto i = 0;
        for (auto itr = begin; i < height_ * width_ && itr != end; i++, ++itr)
            data_[i] = *itr;
        for (; i < height_ * width_; i++)
            data_[i] = value_type{};   
    }

    MatrixContainer(value_type val = value_type{})
    :height_ {1}, width_ {1}, data_ {new value_type[1]{val}} {}

    MatrixContainer(std::initializer_list<value_type> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ {new value_type[height_ * width_]}
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
    :height_ {twodim_list.size()}, width_ {calc_width(twodim_list)}, data_ {new value_type[height_ * width_]}
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

//--------------------------------=| Big five start |=--------------------------------------------------

    // sub method to swap
private:
    void swap(MatrixContainer& mat) noexcept
    {
        std::swap(height_, mat.height_);
        std::swap(width_, mat.width_);
        std::swap(data_, mat.data_);
        std::swap(row_order_, mat.row_order_);
        std::swap(col_order_, mat.col_order_);
    }

public:
    MatrixContainer(const MatrixContainer& rhs)
    :height_ {rhs.height_}, width_ {rhs.width_}, data_ {new value_type[height_ * width_]},
     row_order_ {new size_type[height_]}, col_order_ {new size_type[width_]}
    {
        std::copy(rhs.data_, rhs.data_ + height_ * width_, data_);
        std::copy(rhs.row_order_, rhs.row_order_ + height_, row_order_);
        std::copy(rhs.col_order_, rhs.col_order_ + width_, col_order_);
    }

    MatrixContainer(MatrixContainer&& rhs) noexcept {swap(rhs);}

    MatrixContainer& operator=(const MatrixContainer& rhs)
    {
        MatrixContainer rhs_cpy {rhs};
        swap(rhs_cpy);
        return *this;
    }

    MatrixContainer& operator=(MatrixContainer&& rhs)
    {
        swap(rhs);
        return *this;
    }

    virtual ~MatrixContainer()
    {
        delete[] data_;
        delete[] row_order_;
        delete[] col_order_;
    }
//--------------------------------=| Big five end |=----------------------------------------------------

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

private:
    class ProxyRow
    {
        pointer row_ = nullptr;
        const size_type* col_order_in_row_ = nullptr;
    public:
        ProxyRow(MatrixContainer& mat, size_type row_ind)
        :row_ {mat.data_ + mat.row_order_[row_ind] * mat.width_}, col_order_in_row_ {mat.col_order_}
        {}

        reference operator[](size_type ind)
        {
            return row_[col_order_in_row_[ind]];
        }
    };

    class ConstProxyRow
    {
        const_pointer row_ = nullptr;
        size_type* col_order_in_row_ = nullptr;
    public:
        ConstProxyRow(const MatrixContainer& mat, size_type row_ind)
        :row_ {mat.data_ + mat.row_order_[row_ind] * mat.width_}, col_order_in_row_ {mat.col_order_}
        {}

        const_reference operator[](size_type ind)
        {
            return row_[col_order_in_row_[ind]];
        }
    };

    class TmpProxyRow
    {
        const_pointer row_ = nullptr;
        size_type* col_order_in_row_ = nullptr;
    public:
        TmpProxyRow(const MatrixContainer& mat, size_type row_ind)
        :row_ {mat.data_ + mat.row_order_[row_ind] * mat.width_}, col_order_in_row_ {mat.col_order_}
        {}

        value_type operator[](size_type ind)
        {
            return row_[col_order_in_row_[ind]];
        }
    };

public:
    ProxyRow operator[](size_type ind) &
    {
        return ProxyRow {*this, ind};
    }

    ConstProxyRow operator[](size_type ind) const &
    {
        return ConstProxyRow {*this, ind};
    }

    TmpProxyRow operator[](size_type ind) &&
    {
        return TmpProxyRow {*this, ind};
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
        pointer data_ = nullptr;
        size_type i_, j_ = 0;
        size_type width_ = 0;
        size_type* row_order_ = nullptr;
        size_type* col_order_ = nullptr;

    public:
        Iterator(const MatrixContainer& mat, size_type i, size_type j)
        :data_ {mat.data_}, i_ {i}, j_ {j}, width_ {mat.width_}, row_order_ {mat.row_order_}, col_order_ {mat.col_order_} {}

        reference operator*() & noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        pointer operator->() & noexcept
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

        friend bool operator== (const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.data_ == rhs.data_ && lhs.i_ == rhs.i_ && lhs.j_ == rhs.j_;
        }

        friend bool operator!= (const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs == rhs);
        }   
    };

    Iterator begin() & {return Iterator {*this, 0, 0};}
    Iterator end()   & {return Iterator {*this, height_, 0};}

    class ConstIterator
    {
        const_pointer data_ = nullptr;
        size_type i_, j_ = 0;
        size_type width_ = 0;
        size_type* row_order_ = nullptr;
        size_type* col_order_ = nullptr;

    public:
        ConstIterator(const MatrixContainer& mat, size_type i, size_type j)
        :data_ {mat.data_}, i_ {i}, j_ {j}, width_ {mat.width_}, row_order_ {mat.row_order_}, col_order_ {mat.col_order_} {}

        const_reference operator*() & noexcept
        {
            return data_[row_order_[i_] * width_ + col_order_[j_]]; 
        }

        const_pointer operator->() & noexcept
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

        friend bool operator== (const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return lhs.data_ == rhs.data_ && lhs.i_ == rhs.i_ && lhs.j_ == rhs.j_;
        }

        friend bool operator!= (const ConstIterator& lhs, const ConstIterator& rhs)
        {
            return !(lhs == rhs);
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