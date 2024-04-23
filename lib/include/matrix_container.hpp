#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <cstddef>
#include <compare>

#include "vector.hpp"

namespace Matrix
{
template<typename T = int>
class MatrixContainer
{
public:
    using size_type        = std::size_t;
    using value_type       = T;
    using reference        = T&;
    using const_reference  = const T&;
    using pointer          = T*;
    using const_pointer    = const T*;

    using Row = typename Container::Vector<value_type>;

    using row_iterator       = typename Row::iterator;
    using row_const_iterator = typename Row::const_iterator;
    using iterator       = typename Container::Vector<Row>::iterator;
    using const_iterator = typename Container::Vector<Row>::const_iterator;
    using reverse_iterator       = typename Container::Vector<Row>::reverse_iterator;
    using const_reverse_iterator = typename Container::Vector<Row>::const_reverse_iterator;

private:
    size_type height_ = 0, width_ = 0;
    Container::Vector<Container::Vector<value_type>> data_ = {};

public:
//--------------------------------=| Classic ctors start |=---------------------------------------------
    MatrixContainer() = default;
    
    MatrixContainer(size_type h, size_type w, const_reference val)
    :height_ {h}, width_ {w}, data_ (height_, Row(width_, val))
    {}

    MatrixContainer(size_type h, size_type w)
    :height_ {h}, width_ {w}, data_ (height_, Row(width_))
    {}

    template<std::input_iterator InpIt>
    MatrixContainer(size_type h, size_type w, InpIt begin, InpIt end)
    :height_ {h}, width_ {w}, data_ (height_, Row(width_))
    {   
        for (auto& row: data_)
            for (auto& elem: row)
                if (begin != end)
                    elem = *begin++;
                else
                    elem = value_type{};        
    }

    explicit MatrixContainer(const_reference val)
    :height_ {1}, width_ {1}, data_ (1, Row{val})
    {}

    MatrixContainer(std::initializer_list<value_type> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ (height_, Row(1))
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
            std::copy(row.begin(), row.end(), data_[i++].begin());
    }
//--------------------------------=| Classic ctors end |=-----------------------------------------------

//--------------------------------=| Acces operators start |=-------------------------------------------
    size_type height() const {return height_;}
    size_type width()  const {return width_;}

    reference to(size_type i, size_type j) noexcept
    {
        return data_[i][j];
    }

    const_reference to(size_type i, size_type j) const noexcept
    {
        return data_[i][j];
    }

    Row&       at(size_type ind)       {return data_.at(ind);}
    const Row& at(size_type ind) const {return data_.at(ind);}

    Row&       operator[](size_type ind)       {return data_[ind];}
    const Row& operator[](size_type ind) const {return data_[ind];} 
//--------------------------------=| Acces operators end |=---------------------------------------------

//--------------------------------=| Types start |=-----------------------------------------------------
    bool is_empty()  const {return height() == 0;}
    bool is_row()    const {return height() == 1;}
    bool is_column() const {return width() == 1;}
    bool is_scalar() const {return height() == 1 && width() == 1;}
    bool is_square() const {return height() == width();}
//--------------------------------=| Types end |=-------------------------------------------------------

//--------------------------------=| Swap rows and columns start |=-------------------------------------
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                                                                         
 *----------------------------------------------------------------------------*
 *      ________________________________________________________________      *
 *---==| BE CAREFUL, THIS OPERATIONS INVALIDATE ITERATORS AND REFERENCES|==---*
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      *
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

    iterator begin() {return data_.begin();}
    iterator end()   {return data_.end();}

    const_iterator begin() const {return data_.cbegin();}
    const_iterator end()   const {return data_.cend();}

    const_iterator cbegin() const {return data_.cbegin();}
    const_iterator cend()   const {return data_.cend();}

    reverse_iterator rbegin() {return data_.rbegin();}
    reverse_iterator rend()   {return data_.rend();}

    const_reverse_iterator rbegin() const {return data_.crbegin();}
    const_reverse_iterator rend()   const {return data_.crend();}

    const_reverse_iterator crbegin() const {return data_.crbegin();}
    const_reverse_iterator crend()   const {return data_.crend();}
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