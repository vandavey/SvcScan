/*
*  type_concepts.h
*  ---------------
*  Header file containing data type concept constraints
*/
#pragma once

#ifndef TYPE_CONCEPTS_H
#define TYPE_CONCEPTS_H

#include <concepts>
#include <iterator>
#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Require that the given type can be casted to the specified type.
    */
    template<class From, class To>
    concept Castable = std::same_as<From, To> || std::is_convertible_v<From, To>;

    /**
    * @brief  Require that the given type has a bitwise left-shift operator
    *         overload that returns an output stream reference.
    */
    template<class T>
    concept LShift = requires(ostream &t_os, const T &t_obj)
    {
        { t_os << t_obj } -> std::same_as<ostream &>;
    };

    /**
    * @brief  Require that the first given type is not the same as
    *         any of the other given types.
    */
    template<class T, class ...Args>
    concept NotSameAs = (!std::same_as<T, Args> && ...);

    /**
    * @brief  Require that the given type is a forward range type.
    */
    template<class R>
    concept Range = ranges::forward_range<R> && requires(R t_range)
    {
        { t_range.size() } -> std::same_as<ranges::range_size_t<R>>;
    };

    /**
    * @brief  Require that the given type is a range whose value type is integral.
    */
    template<class R>
    concept IntegralRange = Range<R> && std::integral<range_value_t<R>>;

    /**
    * @brief  Require that the given type is a range whose value type has a bitwise
    *         left-shift operator overload that returns an output stream reference.
    */
    template<class R>
    concept LShiftRange = Range<R> && LShift<range_value_t<R>>;

    /**
    * @brief  Require that the given type is a forward range iterator type.
    */
    template<class T>
    concept RangeIterator = std::forward_iterator<T>;

    /**
    * @brief  Require that the given range type and value type
    *         correspond with one another.
    */
    template<class R, class T>
    concept RangeValue = Range<R> && std::same_as<T, range_value_t<R>>;

    /**
    * @brief  Require that the first given type is the same as
    *         any of the other specified types.
    */
    template<class T, class ...Args>
    concept SameAsAny = (std::same_as<T, Args> || ...);

    /**
    * @brief  Require that the given type is a smart pointer that
    *         encapsulates the specified value type.
    */
    template<class P, class T>
    concept SmartPtr = SameAsAny<P, shared_ptr<T>, unique_ptr<T>>;

    /**
    * @brief  Require that the given type is a range of strings.
    */
    template<class R>
    concept StringRange = Range<R> && std::same_as<range_value_t<R>, string>;
}

#endif // !TYPE_CONCEPTS_H
