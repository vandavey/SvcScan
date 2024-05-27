/*
* @file
*     type_concepts.h
* @brief
*     Header file for data type concept constraints.
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
    * @brief
    *     Require that a type can be implicitly and statically casted to another type.
    */
    template<class FromT, class OutT>
    concept Castable = std::convertible_to<FromT, OutT>;

    /**
    * @brief
    *     Require that a type is a forward range type.
    */
    template<class R>
    concept Range = ranges::forward_range<R> && requires(R r_range)
    {
        { r_range.size() } -> std::same_as<ranges::range_size_t<R>>;
    };

    /**
    * @brief
    *     Require that the first type is the same as any of the types which follow.
    */
    template<class T, class ...ArgsT>
    concept SameAsAny = (std::same_as<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that one ore more types can be implicitly casted as strings.
    */
    template<class ...ArgsT>
    concept String = (Castable<ArgsT, string>, ...);

    /**
    * @brief
    *     Require that a type is a range of strings or
    *     a range whose value type is string-castable.
    */
    template<class R>
    concept StringRange = Range<R> && String<range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a range whose value type is integral.
    */
    template<class R>
    concept IntegralRange = Range<R> && std::integral<range_value_t<R>>;

    /**
    * @brief
    *     Require that a type has a bitwise left-shift operator
    *     overload returning an output stream reference.
    */
    template<class T>
    concept LShift = requires(ostream &r_os, const T &r_obj)
    {
        { r_os << r_obj } -> std::same_as<ostream &>;
    };

    /**
    * @brief
    *     Require that a type is not string-like and has a bitwise left-shift
    *     operator overload returning an output stream reference.
    */
    template<class T>
    concept LShiftNonString = LShift<T> && !String<T>;

    /**
    * @brief
    *     Require that a type is a range whose value type has a bitwise
    *     left-shift operator overload returning an output stream reference.
    */
    template<class R>
    concept LShiftRange = Range<R> && LShift<range_value_t<R>>;

    /**
    * @brief
    *     Require that the first type is not the same as any of the types which follow.
    */
    template<class T, class ...ArgsT>
    concept NotSameAs = (!std::same_as<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that a type is a forward range iterator type.
    */
    template<class T>
    concept RangeIterator = std::forward_iterator<T>;

    /**
    * @brief
    *     Require that a range type and value type correspond with one another.
    */
    template<class R, class T>
    concept RangeValue = Range<R> && std::same_as<T, range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a smart pointer that encapsulates a specific value type.
    */
    template<class P, class T>
    concept SmartPtr = SameAsAny<P, shared_ptr<T>, unique_ptr<T>>;

    /**
    * @brief
    *     Require that a type is sortable range that can be
    *     sorted according to a specific predicate functor type.
    */
    template<class R, class F = ranges::less>
    concept Sortable = Range<R> && std::sortable<ranges::iterator_t<R>, F>;

    /**
    * @brief
    *     Require that a functor type can be used as a range
    *     sorting predicate for a specific range type.
    */
    template<class F, class R = vector<size_t>>
    concept SortPredicate = Sortable<R, F>;
}

#endif // !TYPE_CONCEPTS_H
