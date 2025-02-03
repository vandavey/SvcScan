/*
* @file
*     concepts.h
* @brief
*     Header file for common concept constraints.
*/
#pragma once

#ifndef SCAN_CONCEPTS_H
#define SCAN_CONCEPTS_H

#include <bit>
#include <chrono>
#include <concepts>
#include <functional>
#include <ratio>
#include <type_traits>
#include "../utils/aliases.h"

namespace scan
{
    /**
    * @brief
    *     Require that two types are the same.
    */
    template<class T, class T2>
    concept Same = std::same_as<T, T2>;

    /**
    * @brief
    *     Require that two types are the same when passed by value without CV qualifiers.
    */
    template<class T, class T2>
    concept SameDecayed = Same<decay_t<remove_cvref_t<T>>, decay_t<remove_cvref_t<T2>>>;

    /**
    * @brief
    *     Require that the first type is the same as any of the types which
    *     follow it when they are passed by value without CV qualifiers.
    */
    template<class T, class... ArgsT>
    concept AnySameDecayed = (SameDecayed<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that at least one type is provided in a type parameter pack.
    */
    template<class... ArgsT>
    concept AtLeastOne = sizeof...(ArgsT) >= 1;

    /**
    * @brief
    *     Require that a type can be implicitly casted to another type.
    */
    template<class T, class OutT>
    concept Castable = std::convertible_to<T, OutT>;

    /**
    * @brief
    *     Require that two types are comparable using equality and inequality operators.
    */
    template<class T, class T2>
    concept EqComparable = std::equality_comparable_with<T, T2>;

    /**
    * @brief
    *     Require that a type is a pair.
    */
    template<class T>
    concept Pair = Same<T, pair<typename T::first_type, typename T::second_type>>;

    /**
    * @brief
    *     Require that a type is a bidirectional range type.
    */
    template<class R>
    concept Range = ranges::bidirectional_range<R> && requires(R r_range)
    {
        typename R::value_type;
        typename R::size_type;
        typename R::difference_type;

        typename R::pointer;
        typename R::const_pointer;
        typename R::reference;
        typename R::const_reference;

        typename R::iterator;
        typename R::const_iterator;

        { r_range.empty() } -> Same<bool>;
        { r_range.size() } -> Same<ranges::range_size_t<R>>;

        { r_range.begin() } -> std::bidirectional_iterator;
        { r_range.end() } -> std::bidirectional_iterator;

        { ranges::begin(r_range) } -> std::bidirectional_iterator;
        { ranges::end(r_range) } -> std::bidirectional_iterator;
        { ranges::size(r_range) } -> Same<ranges::range_size_t<R>>;
    };

    /**
    * @brief
    *     Require that a type is a bidirectional range iterator type.
    */
    template<class T>
    concept RangeIterator = std::bidirectional_iterator<T>;

    /**
    * @brief
    *     Require that a type is a ratio type.
    */
    template<class T>
    concept Ratio = Same<T, std::ratio<T::num, T::den>>;

    /**
    * @brief
    *     Require that a type can be treated as a string.
    */
    template<class T>
    concept String = AnySameDecayed<T, char*, const char*, string>;

    /**
    * @brief
    *     Require that a type is a pair whose first type is a string.
    */
    template<class T>
    concept StringPair = Pair<T> && SameDecayed<typename T::first_type, string>;

    /**
    * @brief
    *     Require that a type is a range of strings or a
    *     range whose value type can be treated as a string.
    */
    template<class R>
    concept StringRange = Range<R> && String<range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a trivial type.
    */
    template<class T>
    concept Trivial = std::is_trivial_v<T>;

    /**
    * @brief
    *     Require that a type is an unsigned integral type.
    */
    template<class T>
    concept Unsigned = std::unsigned_integral<T>;

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

        { r_alloc.allocate(r_count) } -> Same<V*>;
        { r_alloc.deallocate(&r_value, r_count) } -> Same<void>;
    };

    /**
    * @brief
    *     Require that one or more types are all comparable to the first type.
    */
    template<class T, class... ArgsT>
    concept AllEqComparable = AtLeastOne<ArgsT...> && (EqComparable<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that two or more types can all be treated as strings.
    */
    template<class T, class... ArgsT>
    concept AllStrings = String<T> && (String<ArgsT> && ...);

    /**
    * @brief
    *     Require that the first type is the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept AnySame = (Same<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that a type is an arithmetic type.
    */
    template<class T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    /**
    * @brief
    *     Require that at least two types are provided in a type parameter pack.
    */
    template<class... ArgsT>
    concept AtLeastTwo = sizeof...(ArgsT) >= 2;

    /**
    * @brief
    *     Require that a type can be reinterpreted as another type using a bit cast.
    */
    template<class T, class OutT>
    concept BitCastable = requires(T r_value)
    {
        std::bit_cast<OutT>(r_value);
    };

    /**
    * @brief
    *     Require that a type is derived from another type.
    */
    template<class T, class BaseT>
    concept Derived = std::derived_from<decay_t<T>, BaseT>;

    /**
    * @brief
    *     Require that a type is a duration type.
    */
    template<class T>
    concept Duration = Arithmetic<typename T::rep>
                    && Ratio<typename T::period>
                    && Same<T, chrono::duration<typename T::rep, typename T::period>>;

    /**
    * @brief
    *     Require that a type is a trivial type that can be
    *     reinterpreted as a byte array using a bit cast.
    */
    template<class T>
    concept Hashable = Trivial<T> && BitCastable<T, byte_array<sizeof(T)>>;

    /**
    * @brief
    *     Require that a type is an integral type.
    */
    template<class T>
    concept Integral = std::integral<T>;

    /**
    * @brief
    *     Require that a type is a range whose value type is integral.
    */
    template<class R>
    concept IntegralRange = Range<R> && Integral<range_value_t<R>>;

    /**
    * @brief
    *     Require that a type has a bitwise left-shift operator
    *     overload returning an output stream reference.
    */
    template<class T>
    concept LShift = requires(ostream& r_os, const T& r_obj)
    {
        { r_os << r_obj } -> Same<ostream&>;
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
    *     Require that a type is a map type.
    */
    template<class M>
    concept Map = Same<M, map<typename M::key_type, typename M::mapped_type>>;

    /**
    * @brief
    *     Require that a type is a member function pointer.
    */
    template<class F>
    concept MemberFuncPtr = std::is_member_function_pointer_v<F>;

    /**
    * @brief
    *     Require that a type is not a bidirectional range iterator type.
    */
    template<class T>
    concept NonRangeIterator = !RangeIterator<T>;

    /**
    * @brief
    *     Require that the first type is not the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept NotSame = (!Same<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that a type is numeric type.
    */
    template<class T>
    concept Numeric = Integral<T> || std::floating_point<T>;

    /**
    * @brief
    *     Require that a type is a pointer type.
    */
    template<class P>
    concept Pointer = std::is_pointer_v<P>;

    /**
    * @brief
    *     Require that a range type and value type correspond with one another.
    */
    template<class R, class T>
    concept RangeValue = Range<R> && Same<T, range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a signed numeric type.
    */
    template<class T>
    concept Signed = Numeric<T> && !Unsigned<T>;

    /**
    * @brief
    *     Require that a type is a unique or shared smart pointer type.
    */
    template<class P>
    concept SmartPtr = Same<P, shared_ptr<typename P::element_type>>
                    || Same<P, unique_ptr<typename P::element_type>>;

    /**
    * @brief
    *     Require that a type is a smart pointer that encapsulates a specific value type.
    */
    template<class P, class T>
    concept SmartPtrOfType = SmartPtr<P> && Same<T, typename P::element_type>;

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
