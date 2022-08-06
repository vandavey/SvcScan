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
#include <iterator>
#include <ranges>

namespace scan
{
    /**
    * @brief  Require that the given range and value types can be used by
    *         algorithms that call binary predicates as arguments.
    */
    template<class R, class T>
    concept BinaryPredicate = std::indirect_binary_predicate<
        std::ranges::equal_to,
        std::projected<std::ranges::iterator_t<R>, std::identity>,
        const T *
    >;

    /**
    * @brief  Require that the given type is a range type that
    *         is clearable and can be shrank-to-fit.
    */
    template<class R>
    concept ClearableRange = std::ranges::forward_range<R> && requires(R t_range)
    {
        { t_range.clear() } -> std::same_as<void>;
        { t_range.shrink_to_fit() } -> std::same_as<void>;
    };

    /**
    * @brief  Require that the given type has a bitwise left-shift operator
    *         overload that returns an output stream reference.
    */
    template<class T>
    concept LShift = requires(std::ostream &t_os, const T &t_obj)
    {
        { t_os << t_obj } -> std::same_as<std::ostream &>;
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
    concept Range = std::ranges::forward_range<R>;

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
    concept RangeValue = Range<R> && BinaryPredicate<R, T>;

    /**
    * @brief  Require that the first given type is the same as
    *         all the other specified types.
    */
    template<class T, class ...Args>
    concept SameAsAll = (std::same_as<T, Args> && ...);

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
    template<class T, class V>
    concept SmartPtr = SameAsAny<T, std::shared_ptr<V>, std::unique_ptr<V>>;
}

#endif // !TYPE_CONCEPTS_H
