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
    /// Require that the range and value types can be used in a binary predicate
    /// ***
    template<class R, class T>
    concept BinaryPredicate = std::indirect_binary_predicate<
        std::ranges::equal_to,
        std::projected<std::ranges::iterator_t<R>, std::identity>,
        const T *
    >;

    /// ***
    /// Require that a given type has a cast operator overload for another type
    /// ***
    template<class From, class To>
    concept Castable = requires(From t_from)
    {
        { static_cast<To>(t_from) };
    };

    /// ***
    /// Require that a given type has a bitwise left-shift operator overload
    /// ***
    template<class T>
    concept LShift = requires(std::ostream &t_os, const T &t_obj)
    {
        { t_os << t_obj } -> std::same_as<std::ostream &>;
    };

    /// ***
    /// Require that a given type is an iterable range type
    /// ***
    template<class R>
    concept Range = std::ranges::forward_range<R>;

    /// ***
    /// Require that a given type is a clearable range
    /// ***
    template<class R>
    concept Clearable = Range<R> && requires(R t_range)
    {
        { t_range.clear() } -> std::same_as<void>;
        { t_range.shrink_to_fit() } -> std::same_as<void>;
    };

    /// ***
    /// Require that a variadic parameter list contains only iterable range types
    /// ***
    template<class R, class ...Args>
    concept RangeTypes = Range<R> && (RangeTypes<Args> && ...);

    /// ***
    /// Require that the specified value type is correct for the given range
    /// ***
    template<class R, class T>
    concept RangeValue = Range<R> && BinaryPredicate<R, T>;
}

#endif // !TYPE_CONCEPTS_H
