/*
*  property.h
*  ----------
*  Header file for object property class template
*  with backing field pointer
*/
#pragma once

#ifndef PROPERTY_H
#define PROPERTY_H

namespace Scan
{
    /// ***
    /// Property encapsulating a backing field by pointer
    /// ***
    template<class T>
    class Property
    {
    protected: /* Fields */
        const T *m_ptr; // Backing field pointer

    public: /* Constructors & Destructor */
        Property() noexcept;
        Property(const Property &prop) noexcept;
        Property(const T *ptr) noexcept;

        virtual ~Property() = default;

    public: /* Operators */
        const bool operator!() const noexcept;

        Property &operator=(const T *ptr) noexcept;
        Property &operator=(const Property &prop) noexcept;

        /// Bitwise left shift operator
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const Property &prop) {
            if (prop.get() == T())
            {
                return os;
            }
            return (os << prop.get());
        };

    public: /* Methods */
        virtual void set(const T *ptr) noexcept;

        virtual const T get() const;
        virtual T get();
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property() noexcept
{
    this->m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const Property &prop) noexcept
{
    this->m_ptr = prop.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Property<T>::Property(const T *ptr) noexcept
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
inline Scan::Property<T> &Scan::Property<T>::operator=(const T *ptr)
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
inline void Scan::Property<T>::set(const T *ptr) noexcept
{
    m_ptr = ptr ? ptr : nullptr;
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline const T Scan::Property<T>::get() const
{
    return (m_ptr == nullptr) ? T() : *m_ptr;
}

/// ***
/// Backing field object value accessor
/// ***
template<class T>
inline T Scan::Property<T>::get()
{
    return (m_ptr == nullptr) ? T() : *m_ptr;
}

#endif // !PROPERTY_H
