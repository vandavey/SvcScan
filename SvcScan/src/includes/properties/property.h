/*
*  property.h
*  ----------
*  Header file for object property class template
*  using a backing field pointer
*/
#pragma once

#ifndef PROPERTY_H
#define PROPERTY_H

#include <iostream>

namespace scan
{
    /// ***
    /// Property that encapsulates a backing field
    /// of type <T> by pointer
    /// ***
    template<class T>
    class Property
    {
    protected:  /* Types & Fields */
        using value_type = T;

        const value_type *m_ptr;  // Backing field pointer

    public:  /* Constructors & Destructor */
        Property();
        Property(const Property &t_prop);
        explicit Property(const value_type *t_ptr);

        virtual ~Property() = default;

    public:  /* Operators */
        const bool operator!() const noexcept;

        Property &operator=(const value_type *t_ptr) noexcept;
        Property &operator=(const Property &t_prop) noexcept;

        /// Bitwise left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &t_os,
                                               const Property &t_prop) {
            if (t_prop.get() == value_type())
            {
                return t_os;
            }
            return (t_os << t_prop.get());
        };

    public:  /* Methods */
        virtual void set(const value_type *t_ptr) noexcept;

        virtual const typename value_type get() const;
        virtual typename value_type get();
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property()
{
    m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property(const Property &t_prop)
{
    m_ptr = t_prop.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property(const value_type *t_ptr)
{
    m_ptr = t_ptr ? t_ptr : nullptr;
}

/// ***
/// Logical (unary) NOT operator overload
/// ***
template<class T>
inline const bool scan::Property<T>::operator!() const noexcept
{
    return m_ptr == nullptr;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::Property<T> &scan::Property<T>::operator=(const value_type *t_ptr)
noexcept {
    m_ptr = t_ptr ? t_ptr : nullptr;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::Property<T> &scan::Property<T>::operator=(const Property &t_prop)
noexcept {
    m_ptr = t_prop.m_ptr;
    return *this;
}

/// ***
/// Backing field object value specifier
/// ***
template<class T>
inline void scan::Property<T>::set(const value_type *t_ptr) noexcept
{
    m_ptr = t_ptr ? t_ptr : nullptr;
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline const typename scan::Property<T>::value_type scan::Property<T>::get()
const {
    if (m_ptr == nullptr)
    {
        return value_type();
    }
    return static_cast<value_type>(*m_ptr);
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline typename scan::Property<T>::value_type scan::Property<T>::get()
{
    if (m_ptr == nullptr)
    {
        return value_type();
    }
    return static_cast<value_type>(*m_ptr);
}

#endif // !PROPERTY_H
