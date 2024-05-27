/*
* @file
*     index_pair.h
* @brief
*     Header file for a generic collection index-value pair.
*/
#pragma once

#ifndef INDEX_PAIR_H
#define INDEX_PAIR_H

namespace scan
{
    /**
    * @brief  Generic collection index-value pair.
    */
    template<class T>
    class IndexPair
    {
    private:  /* Type Aliases */
        using value_type = T;

    public:  /* Fields */
        size_t index;      // Collection element index
        value_type value;  // Collection element value

    public:  /* Constructors & Destructor */
        IndexPair(const IndexPair &t_pair) noexcept;
        IndexPair(IndexPair &&) = default;
        IndexPair(const size_t &t_idx, const value_type &t_value) noexcept;

        virtual ~IndexPair() = default;

    public:  /* Operators */
        IndexPair &operator=(const IndexPair &) = default;
        IndexPair &operator=(IndexPair &&) = default;
    };
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::IndexPair<T>::IndexPair(const IndexPair &t_pair) noexcept
{
    index = t_pair.index;
    value = t_pair.value;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::IndexPair<T>::IndexPair(const size_t &t_idx,
                                     const value_type &t_value) noexcept
{
    index = t_idx;
    value = t_value;
}

#endif // !INDEX_PAIR_H
