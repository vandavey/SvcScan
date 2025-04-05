/*
* @file
*     iterator_traits.h
* @brief
*     Header file for generic contiguous iterator type traits.
*/
#pragma once

#ifndef SCAN_ITERATOR_TRAITS_H
#define SCAN_ITERATOR_TRAITS_H

#include <iterator>
#include "../concepts/concepts.h"

namespace scan
{
    /**
    * @brief
    *     Generic contiguous iterator type traits.
    */
    template<NonIter T>
    struct IteratorTraits
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using pointer         = value_type*;
        using reference       = value_type&;
        using difference_type = ptrdiff_t;

        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept  = std::contiguous_iterator_tag;
    };
}

#endif // !SCAN_ITERATOR_TRAITS_H
