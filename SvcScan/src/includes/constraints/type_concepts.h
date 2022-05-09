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
    /// Require that a given type has a cast operator overload for another type
    /// ***
    template<class From, class To>
    concept Castable = requires(From t_from_obj)
    {
        { static_cast<To>(t_from_obj) } -> std::same_as<To>;
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
    template<class T>
    concept Range = std::ranges::range<T>;

    /// ***
    /// Require that a variadic parameter list contains only iterable range types
    /// ***
    template<class T, class ...Args>
    concept RangeTypes = Range<T> && (RangeTypes<Args> && ...);
}

#endif // !TYPE_CONCEPTS_H
