/*
* @file
*     iterator_traits.h
* @brief
*     Header file for generic bidirectional iterator type traits.
*/
#pragma once

#ifndef SCAN_ITERATOR_TRAITS_H
#define SCAN_ITERATOR_TRAITS_H

#include <iterator>

namespace scan
{
    /**
    * @brief
    *     Generic bidirectional iterator type traits.
    */
    template<class T>
    struct IteratorTraits
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using pointer         = value_type*;
        using reference       = value_type&;
        using difference_type = ptrdiff_t;

        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept  = std::bidirectional_iterator_tag;
    };
}

#endif // !SCAN_ITERATOR_TRAITS_H
