/*
* @file
*     concepts.h
* @brief
*     Header file for common concept constraints.
*/
#pragma once

#ifndef SCAN_CONCEPTS_H
#define SCAN_CONCEPTS_H

#include <concepts>
#include <type_traits>
#include "../utils/aliases.h"

namespace scan
{
    /**
    * @brief
    *     Require that at least one type is provided in a type parameter pack.
    */
    template<class... ArgsT>
    concept AtLeastOneParam = sizeof...(ArgsT) > 0;

    /**
    * @brief
    *     Require that a type can be implicitly and statically casted to another type.
    */
    template<class T, class OutT>
    concept Castable = std::convertible_to<T, OutT>;

    /**
    * @brief
    *     Require that two types are comparable using equality and inequality operators.
    */
    template<class T, class T2>
    concept Comparable = requires(T r_lhs, T r_rhs)
    {
        { r_lhs == r_rhs } -> std::same_as<bool>;
        { r_lhs != r_rhs } -> std::same_as<bool>;
    };

    /**
    * @brief
    *     Require that a type is a pair.
    */
    template<class T>
    concept Pair = std::same_as<T, pair<typename T::first_type, typename T::second_type>>;

    /**
    * @brief
    *     Require that a type is a bidirectional range type.
    */
    template<class R>
    concept Range = ranges::bidirectional_range<R> && requires(R r_range)
    {
        { r_range.size() } -> std::same_as<ranges::range_size_t<R>>;
    };

    /**
    * @brief
    *     Require that two types are the same when passed by value without CV qualifiers.
    */
    template<class T, class T2>
    concept SameEnoughAs = std::same_as<std::decay_t<std::remove_cvref_t<T>>, T2>;

    /**
    * @brief
    *     Require that the first type is the same as any of the types which
    *     follow it when they are passed by value without CV qualifiers.
    */
    template<class T, class... ArgsT>
    concept SameEnoughAsAny = (SameEnoughAs<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that a type can be treated as a string.
    */
    template<class T>
    concept String = SameEnoughAsAny<T, char*, const char*, string>;

    /**
    * @brief
    *     Require that a type is a pair whose first type is a string.
    */
    template<class T>
    concept StringPair = Pair<T> && SameEnoughAs<typename T::first_type, string>;

    /**
    * @brief
    *     Require that a type is a range of strings or a
    *     range whose value type can be treated as a string.
    */
    template<class R>
    concept StringRange = Range<R> && String<range_value_t<R>>;

    /**
    * @brief
    *     Require that one or more types are all comparable to the first type.
    */
    template<class T, class... ArgsT>
    concept AllComparable = AtLeastOneParam<ArgsT...> && (Comparable<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that two or more types can all be treated as strings.
    */
    template<class T, class... ArgsT>
    concept AllStrings = String<T> && (String<ArgsT> && ...);

    /**
    * @brief
    *     Require that a type is an integral or enumeration type.
    */
    template<class T>
    concept EnumOrIntegral = std::integral<T> || std::is_enum_v<T>;

    /**
    * @brief
    *     Require that a type is a hashable byte type.
    */
    template<class T>
    concept HashableByte = EnumOrIntegral<T> && sizeof(T) == 1 && requires(T r_byte)
    {
        static_cast<uchar_t>(r_byte);
    };

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
    concept LShift = requires(ostream& r_os, const T& r_obj)
    {
        { r_os << r_obj } -> std::same_as<ostream&>;
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
    *     Require that a type is a map.
    */
    template<class M>
    concept Map = std::same_as<M, map<typename M::key_type, typename M::mapped_type>>;

    /**
    * @brief
    *     Require that the first type is not the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept NotSameAs = (!std::same_as<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that a type is a pointer type.
    */
    template<class T>
    concept Pointer = std::is_pointer_v<T>;

    /**
    * @brief
    *     Require that a type is a bidirectional range iterator type.
    */
    template<class T>
    concept RangeIterator = std::bidirectional_iterator<T>;

    /**
    * @brief
    *     Require that a range type and value type correspond with one another.
    */
    template<class R, class T = range_value_t<R>>
    concept RangeValue = Range<R> && std::same_as<T, range_value_t<R>>;

    /**
    * @brief
    *     Require that the first type is the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept SameAsAny = (std::same_as<T, ArgsT> || ...);

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
    concept Sortable = Range<R> && std::sortable<range_iterator_t<R>, F>;

    /**
    * @brief
    *     Require that a functor type can be used as a range
    *     sorting predicate for a specific range type.
    */
    template<class F, class R = vector<size_t>>
    concept SortPredicate = Sortable<R, F>;

    /**
    * @brief
    *     Require that a type is a map whose key type is a string.
    */
    template<class M>
    concept StringMap = Map<M> && StringPair<typename M::value_type>;
}

#endif // !SCAN_CONCEPTS_H
