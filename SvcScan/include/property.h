#pragma once

#ifndef PROPERTY_H
#define PROPERTY_H

#include <iostream>

namespace Scan
{
    /// ***
    /// Property encapsulating a backing field by pointer
    /// ***
    template<class T>
    class Property
    {
    protected: /* Fields */
        const T *m_valueptr; // Backing field pointer

    public: /* Constructors & Destructor */
        Property();
        Property(const Property &prop);
        Property(const T *valueptr);

        virtual ~Property() = default;

    public: /* Operators */
        const bool operator!() const;

        Property &operator=(const T *valueptr);
        Property &operator=(const Property &prop);

        /// Left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const Property &prop) {
            return !prop ? os : (os << prop.get());
        };

    public: /* Methods */
        virtual void set(const T *valueptr);

        virtual const bool empty() const;

        virtual const T get() const;
        virtual const T get();
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property()
{
    this->m_valueptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const Property &prop)
{
    this->operator=(prop);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const T *valueptr)
{
    if (!valueptr)
    {
        this->m_valueptr = nullptr;
        return;
    }
    this->m_valueptr = valueptr;
}

/// ***
/// Logical (unary) NOT operator overload
/// ***
template<class T>
inline const bool Scan::Property<T>::operator!() const
{
    return (m_valueptr == nullptr);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::Property<T> &Scan::Property<T>::operator=(const T *valueptr)
{
    if (!valueptr)
    {
        this->m_valueptr = nullptr;
        return *this;
    }
    this->m_valueptr = valueptr;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::Property<T> &Scan::Property<T>::operator=(const Property &prop)
{
    this->m_valueptr = prop.m_valueptr;
    return *this;
}

/// ***
/// Backing field object specifier
/// ***
template<class T>
inline void Scan::Property<T>::set(const T *valueptr)
{
    if (!valueptr)
    {
        std::cerr << "[x] 'value' cannot be nullptr" << std::endl;
        return;
    }
    this->m_valueptr = valueptr;
}

/// ***
/// Determine if backing field is empty or default value
/// ***
template<class T>
inline const bool Scan::Property<T>::empty() const
{
    if (m_valueptr == nullptr)
    {
        std::cerr << "[!] Value of pointer member is nullptr";
        return true;
    }
    return std::empty<T>({*m_valueptr});
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const T Scan::Property<T>::get() const
{
    return (m_valueptr != nullptr) ? *m_valueptr : T();
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const T Scan::Property<T>::get()
{
    return (m_valueptr != nullptr) ? *m_valueptr : T();
}

#endif // !PROPERTY_H
