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
    protected:  /* Types */
        using value_type = T;

    protected:  /* Fields */
        const value_type *m_vptr;  // Backing field pointer

    public:  /* Constructors & Destructor */
        Property();
        Property(const Property &t_prop);
        explicit Property(const value_type *t_ptr);

        virtual ~Property() = default;

    public:  /* Operators */
        virtual operator value_type() const;

        Property &operator=(const value_type *t_ptr);
        Property &operator=(const Property &t_prop);

        Property &operator()(const value_type *t_ptr);

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

        virtual value_type get() const;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property()
{
    m_vptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property(const Property &t_prop)
{
    m_vptr = t_prop.m_vptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Property<T>::Property(const value_type *t_ptr)
{
    m_vptr = t_ptr ? t_ptr : nullptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::Property<T>::operator value_type() const
{
    return (m_vptr == nullptr) ? value_type() : *m_vptr;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::Property<T> &scan::Property<T>::operator=(const value_type *t_ptr)
{
    m_vptr = t_ptr ? t_ptr : nullptr;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::Property<T> &scan::Property<T>::operator=(const Property &t_prop)
 {
    m_vptr = t_prop.m_vptr;
    return *this;
}

/// ***
/// Function call operator overload
/// ***
template<class T>
inline scan::Property<T> &scan::Property<T>::operator()(const value_type *t_ptr)
{
    return operator=(t_ptr);
}

/// ***
/// Backing field object value specifier
/// ***
template<class T>
inline void scan::Property<T>::set(const value_type *t_ptr) noexcept
{
    m_vptr = t_ptr ? t_ptr : nullptr;
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline typename scan::Property<T>::value_type scan::Property<T>::get() const
{
    return (m_vptr == nullptr) ? value_type() : *m_vptr;
}

#endif // !PROPERTY_H
