/*
* @file
*     concepts.h
* @brief
*     Header file for common concept constraints.
*/
#pragma once

#ifndef SCAN_CONCEPTS_H
#define SCAN_CONCEPTS_H

#include <chrono>
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
        { r_range.begin() } -> std::bidirectional_iterator;
        { r_range.end() } -> std::bidirectional_iterator;
        { r_range.size() } -> std::same_as<ranges::range_size_t<R>>;

        { ranges::begin(r_range) } -> std::bidirectional_iterator;
        { ranges::end(r_range) } -> std::bidirectional_iterator;
        { ranges::size(r_range) } -> std::same_as<ranges::range_size_t<R>>;
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
    *     Require that a type can be statically casted to another type.
    */
    template<class T, class OutT>
    concept StaticCastable = requires(T r_from_value)
    {
        static_cast<OutT>(r_from_value);
    };

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
    *     Require that a type is a container allocator type
    *     that can be used by all standard library ranges.
    */
    template<class A, class V>
    concept Allocator = requires(A r_alloc, V r_value, size_t r_count)
    {
        typename A::value_type;
        typename A::size_type;
        typename A::difference_type;
        typename A::propagate_on_container_move_assignment;
        typename A::is_always_equal;

        { r_alloc.allocate(r_count) } -> std::same_as<V*>;
        { r_alloc.deallocate(&r_value, r_count) } -> std::same_as<void>;
    };

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
    *     Require that a type is derived from another type.
    */
    template<class T, class B>
    concept Derived = std::derived_from<T, B>;

    /**
    * @brief
    *     Require that a type is a duration type.
    */
    template<class T>
    concept Duration = Derived<T, chrono::duration<typename T::rep, typename T::period>>;

    /**
    * @brief
    *     Require that a type is a fundamental (primitive) type.
    */
    template<class T>
    concept Fundamental = std::is_fundamental_v<T>;

    /**
    * @brief
    *     Require that a type is a hashable byte type.
    */
    template<class T>
    concept HashableByte = sizeof(T) == 1
                        && StaticCastable<T, uchar_t>
                        && (std::integral<T> || std::is_enum_v<T>)
                        && (sizeof(size_t) == 4 || sizeof(size_t) == 8);

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
    *     Require that a type is a member function pointer.
    */
    template<class F>
    concept MemberFuncPtr = std::is_member_function_pointer_v<F>;

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
    *     Require that a type is a sortable range that can be sorted using
    *     specific comparison predicate and projection functor types.
    */
    template<class R, class F = ranges::less, class P = std::identity>
    concept SortableRange = Range<R> && std::sortable<range_iterator_t<R>, F, P>;

    /**
    * @brief
    *     Require that a type is a map whose key type is a string.
    */
    template<class M>
    concept StringMap = Map<M> && StringPair<typename M::value_type>;
}

#endif // !SCAN_CONCEPTS_H
