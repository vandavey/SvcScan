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
#include <iterator>
#include <memory>
#include <ranges>
#include <ratio>
#include <type_traits>
#include "../utils/aliases.h"

namespace scan
{
    /**
    * @brief
    *     Require that at least one type is provided in a type parameter pack.
    */
    template<class... ArgsT>
    concept AtLeastOne = sizeof...(ArgsT) >= 1;

    /**
    * @brief
    *     Require that at least two types are provided in a type parameter pack.
    */
    template<class... ArgsT>
    concept AtLeastTwo = sizeof...(ArgsT) >= 2;

    /**
    * @brief
    *     Require that a type is a random access range iterator type.
    */
    template<class I>
    concept Iter = std::random_access_iterator<I>;

    /**
    * @brief
    *     Require that two types are the same.
    */
    template<class T, class T2>
    concept Same = std::same_as<T, T2>;

    /**
    * @brief
    *     Require that two types are the same when they are passed by value.
    */
    template<class T, class T2>
    concept SameByValue = Same<remove_cvref_t<T>, remove_cvref_t<T2>>;

    /**
    * @brief
    *     Require that two types are the same when they are decayed and passed by value.
    */
    template<class T, class T2>
    concept SameDecayed = SameByValue<decay_t<T>, decay_t<T2>>;

    /**
    * @brief
    *     Require that the first type is the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept AnySame = AtLeastOne<ArgsT...> && (Same<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that the first type is the same as any of the
    *     types which follow it when they are passed by value.
    */
    template<class T, class... ArgsT>
    concept AnySameByValue = AtLeastOne<ArgsT...> && (SameByValue<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that the first type is the same as any of the types
    *     which follow it when they are decayed and passed by value.
    */
    template<class T, class... ArgsT>
    concept AnySameDecayed = AtLeastOne<ArgsT...> && (SameDecayed<T, ArgsT> || ...);

    /**
    * @brief
    *     Require that a type is a basic random access range type.
    */
    template<class R>
    concept BasicRange = ranges::random_access_range<R> && requires(R& r_range)
    {
        { ranges::begin(r_range) } noexcept -> Iter;
        { ranges::end(r_range) } noexcept -> Iter;

        { ranges::empty(r_range) } noexcept -> Same<bool>;
        { ranges::size(r_range) } noexcept -> Same<range_size_t<R>>;
    };

    /**
    * @brief
    *     Require that a type can be implicitly casted to another type.
    */
    template<class T, class OutT>
    concept Castable = std::convertible_to<T, OutT>;

    /**
    * @brief
    *     Require that a type is a constant random access range iterator type.
    */
    template<class I>
    concept ConstIter = Iter<I> && Same<iter_reference_t<I>, const iter_value_t<I>&>;

    /**
    * @brief
    *     Require that a type is an integral type.
    */
    template<class T>
    concept Integral = std::integral<remove_cvref_t<T>>;

    /**
    * @brief
    *     Require that a type is a pair.
    */
    template<class T>
    concept Pair = Same<T, pair<typename T::first_type, typename T::second_type>>;

    /**
    * @brief
    *     Require that a type is a random access range type.
    */
    template<class R>
    concept Range = BasicRange<R> && requires(R& r_range)
    {
        typename R::value_type;
        typename R::pointer;
        typename R::const_pointer;
        typename R::reference;
        typename R::const_reference;
        typename R::size_type;
        typename R::difference_type;

        typename R::iterator;
        typename R::const_iterator;

        { ranges::cbegin(r_range) } noexcept -> ConstIter;
        { ranges::cend(r_range) } noexcept -> ConstIter;
    };

    /**
    * @brief
    *     Require that a type is a random access range
    *     that encapsulates a specific value type.
    */
    template<class R, class T>
    concept RangeOf = Range<remove_cvref_t<R>> && SameByValue<T, range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a ratio type.
    */
    template<class T>
    concept Ratio = Same<T, std::ratio<T::num, T::den>>;

    /**
    * @brief
    *     Require that a type is a range that can be sorted using
    *     specific invocable binary predicate and projection types.
    */
    template<class R, class PredicateF, class ProjectF>
    concept Sortable = Range<R> && std::sortable<iterator_t<R>, PredicateF, ProjectF>;

    /**
    * @brief
    *     Require that a type can be treated as a string.
    */
    template<class T>
    concept StringLike = AnySameDecayed<T, char*, const char*, string>;

    /**
    * @brief
    *     Require that a type is a range of strings or a
    *     range whose value type can be treated as a string.
    */
    template<class R>
    concept StringLikeRange = Range<R> && StringLike<range_value_t<R>>;

    /**
    * @brief
    *     Require that a type is a pair whose first type is a constant string.
    */
    template<class T>
    concept StringPair = Pair<T> && Same<typename T::first_type, const string>;

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
    *     Require that a type is a memory allocator type.
    */
    template<class AllocT, class T>
    concept Allocator = requires(AllocT r_alloc,
                                 T* r_ptr,
                                 allocator_traits<AllocT>::size_type r_n)
    {
        typename allocator_traits<AllocT>::value_type;
        typename allocator_traits<AllocT>::size_type;
        typename allocator_traits<AllocT>::difference_type;
        typename allocator_traits<AllocT>::propagate_on_container_move_assignment;
        typename allocator_traits<AllocT>::is_always_equal;

        typename allocator_traits<AllocT>::template rebind_alloc<T>;

        { allocator_traits<AllocT>::allocate(r_alloc, r_n) } -> Same<T*>;
        { allocator_traits<AllocT>::deallocate(r_alloc, r_ptr, r_n) } -> Same<void>;
    };

    /**
    * @brief
    *     Require that a type is an arithmetic type.
    */
    template<class T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    /**
    * @brief
    *     Require that a type can be reinterpreted as another type via bit cast.
    */
    template<class T, class OutT>
    concept BitCastable = requires(const T& r_value)
    {
        { std::bit_cast<OutT>(r_value) } noexcept -> Same<OutT>;
    };

    /**
    * @brief
    *     Require that a type is an integral bit-mask type.
    */
    template<class T>
    concept BitMask = Integral<T> && requires(T r_lhs_mask, T r_rhs_mask, uint_t r_offset)
    {
        { ~r_lhs_mask } noexcept -> Castable<T>;

        { r_lhs_mask & r_rhs_mask } noexcept -> Castable<T>;
        { r_lhs_mask | r_rhs_mask } noexcept -> Castable<T>;
        { r_lhs_mask ^ r_rhs_mask } noexcept -> Castable<T>;

        { r_lhs_mask << r_offset } noexcept -> Castable<T>;
        { r_lhs_mask >> r_offset } noexcept -> Castable<T>;

        { r_lhs_mask &= r_rhs_mask } noexcept -> Same<T&>;
        { r_lhs_mask |= r_rhs_mask } noexcept -> Same<T&>;
        { r_lhs_mask ^= r_rhs_mask } noexcept -> Same<T&>;

        { r_lhs_mask <<= r_offset } noexcept -> Same<T&>;
        { r_lhs_mask >>= r_offset } noexcept -> Same<T&>;
    };

    /**
    * @brief
    *     Require that a type is a constant-qualified type.
    */
    template<class T>
    concept Const = std::is_const_v<remove_reference_t<T>>;

    /**
    * @brief
    *     Require that the first type is constructible from the types which follow it.
    */
    template<class T, class... ArgsT>
    concept Constructible = AtLeastOne<ArgsT...> && requires(ArgsT&&... t_args)
    {
        { T{std::forward<ArgsT>(t_args)...} } -> Same<T>;
    };

    /**
    * @brief
    *     Require that a type is derived from another type.
    */
    template<class T, class BaseT>
    concept DerivedFrom = std::derived_from<remove_cvref_t<T>, remove_cvref_t<BaseT>>;

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
    *     Require that two types are comparable using equality and inequality operators.
    */
    template<class T, class T2>
    concept EqComparable = std::equality_comparable_with<T, T2>;

    /**
    * @brief
    *     Require that a type is a floating-point type.
    */
    template<class T>
    concept FloatingPoint = std::floating_point<remove_cvref_t<T>>;

    /**
    * @brief
    *     Require that a type is a trivial type that can
    *     be reinterpreted as a byte array via bit cast.
    */
    template<class T>
    concept Hashable = Trivial<T> && BitCastable<T, byte_array_t<sizeof(T)>>;

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
    concept LShiftNonString = LShift<T> && !StringLike<T>;

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
    *     Require that the first type is not the same as any of the types which follow it.
    */
    template<class T, class... ArgsT>
    concept NotSame = (!Same<T, ArgsT> && ...);

    /**
    * @brief
    *     Require that a type is a nullary invocable type that returns void.
    */
    template<class F>
    concept NullaryVoid = std::invocable<F> && Same<invoke_result_t<F>, void>;

    /**
    * @brief
    *     Require that a type is numeric type.
    */
    template<class T>
    concept Numeric = Integral<T> || FloatingPoint<T>;

    /**
    * @brief
    *     Require that a type is an invocable projection that
    *     can project the values of a specific range type.
    */
    template<class F, class R>
    concept Projection = Range<R>
                      && std::invocable<F, range_reference_t<R>>
                      && std::regular_invocable<F, range_reference_t<R>>
                      && std::totally_ordered<std::indirect_result_t<F&, iterator_t<R>>>;

    /**
    * @brief
    *     Require that a type is a range to which
    *     values can be appended via back-insertion.
    */
    template<class R>
    concept PushableRange = Range<R> && requires(R& r_range)
    {
        { std::back_inserter(r_range) } noexcept -> Same<std::back_insert_iterator<R>>;
    };

    /**
    * @brief
    *     Require that a type is a range to which values can be appended
    *     via back-insertion that encapsulates a specific value type.
    */
    template<class R, class T>
    concept PushableRangeOf = PushableRange<remove_cvref_t<R>> && RangeOf<R, T>;

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
    concept SmartPtrOf = SmartPtr<P> && Same<T, typename P::element_type>;

    /**
    * @brief
    *     Require that the first type is a smart pointer that
    *     encapsulates any of the value types which follow it.
    */
    template<class P, class... ArgsT>
    concept SmartPtrOfAny = AtLeastOne<ArgsT...> && (SmartPtrOf<P, ArgsT> || ...);

    /**
    * @brief
    *     Require that a type is an invocable projection that can
    *     project the values of a specific sortable range type.
    */
    template<class F, class R>
    concept SortProjection = Projection<F, R> && Sortable<R, ranges::less, F>;

    /**
    * @brief
    *     Require that a type is a map whose key type is a constant string.
    */
    template<class M>
    concept StringMap = Map<M> && StringPair<typename M::value_type>;

    /**
    * @brief
    *     Require that a type is a random access range view type.
    */
    template<class V>
    concept View = ranges::view<V> && BasicRange<V>;

    /**
    * @brief
    *     Require that a type is a random access range
    *     view that encapsulates a specific value type.
    */
    template<class V, class T>
    concept ViewOf = View<remove_cvref_t<V>> && SameByValue<T, range_value_t<V>>;
}

#endif // !SCAN_CONCEPTS_H
