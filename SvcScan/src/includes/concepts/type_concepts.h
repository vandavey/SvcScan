/*
*  type_concepts.h
*  ---------------
*  Header file containing data type concept constraints
*/
#pragma once

#ifndef TYPE_CONCEPTS_H
#define TYPE_CONCEPTS_H

#include <concepts>
#include <iostream>

namespace scan
{
    /// ***
    /// Require that the given type is the same as all the other specified types
    /// ***
    template<class T, class ...Args>
    concept AllSameAs = (std::same_as<T, Args> && ...);

    /// ***
    /// Require that the given type is the same as any of the other specified types
    /// ***
    template<class T, class ...Args>
    concept AnySameAs = (std::same_as<T, Args> || ...);

    /// ***
    /// Require that the range and value types can be used in a binary predicate
    /// ***
    template<class R, class T>
    concept BinaryPredicate = std::indirect_binary_predicate<
        std::ranges::equal_to,
        std::projected<std::ranges::iterator_t<R>, std::identity>,
        const T *
    >;

    /// ***
    /// Require that the given type is a clearable range type
    /// ***
    template<class R>
    concept ClearableRange = std::ranges::forward_range<R> && requires(R t_range)
    {
        { t_range.clear() } -> std::same_as<void>;
        { t_range.shrink_to_fit() } -> std::same_as<void>;
    };

    /// ***
    /// Require that the given type has a bitwise left-shift operator overload
    /// ***
    template<class T>
    concept LShift = requires(std::ostream &t_os, const T &t_obj)
    {
        { t_os << t_obj } -> std::same_as<std::ostream &>;
    };

    /// ***
    /// Require that the given type is an iterable range type
    /// ***
    template<class R>
    concept Range = std::ranges::forward_range<R>;

    /// ***
    /// Require that the given type is a forward range iterator type
    /// ***
    template<class T>
    concept RangeIterator = std::forward_iterator<T>;

    /// ***
    /// Require that the specified value type corresponds to the given range type
    /// ***
    template<class R, class T>
    concept RangeValue = Range<R> && BinaryPredicate<R, T>;
}

#endif // !TYPE_CONCEPTS_H
