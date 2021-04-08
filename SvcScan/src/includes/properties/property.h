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

namespace Scan
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
        Property(const Property &prop);
        explicit Property(const value_type *ptr);

        virtual ~Property() = default;

    public:  /* Operators */
        const bool operator!() const noexcept;

        Property &operator=(const value_type *ptr) noexcept;
        Property &operator=(const Property &prop) noexcept;

        /// Bitwise left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const Property &prop) {
            if (prop.get() == value_type())
            {
                return os;
            }
            return (os << prop.get());
        };

    public:  /* Methods */
        virtual void set(const value_type *ptr) noexcept;

        virtual const typename value_type get() const;
        virtual typename value_type get();
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property()
{
    this->m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const Property &prop)
{
    this->m_ptr = prop.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const value_type *ptr)
{
    this->m_ptr = ptr ? ptr : nullptr;
}

/// ***
/// Logical (unary) NOT operator overload
/// ***
template<class T>
inline const bool Scan::Property<T>::operator!() const noexcept
{
    return (m_ptr == nullptr);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::Property<T> &Scan::Property<T>::operator=(const value_type *ptr)
noexcept {
    m_ptr = ptr ? ptr : nullptr;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::Property<T> &Scan::Property<T>::operator=(const Property &prop)
noexcept {
    m_ptr = prop.m_ptr;
    return *this;
}

/// ***
/// Backing field object value specifier
/// ***
template<class T>
inline void Scan::Property<T>::set(const value_type *ptr) noexcept
{
    m_ptr = ptr ? ptr : nullptr;
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline const typename Scan::Property<T>::value_type Scan::Property<T>::get()
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
inline typename Scan::Property<T>::value_type Scan::Property<T>::get()
{
    if (m_ptr == nullptr)
    {
        return value_type();
    }
    return static_cast<value_type>(*m_ptr);
}

#endif // !PROPERTY_H
