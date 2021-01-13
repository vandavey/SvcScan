/*
*  autoprop.h
*  ----------
*  Header file for object property template class with
*  automatic backing field
*/
#pragma once

#ifndef AUTOPROP_H
#define AUTOPROP_H

#include "property.h"

namespace Scan
{
    /// ***
    /// Property that automatically encapsulates a backing field
    /// ***
    template<class T>
    class AutoProp : public Property<T>
    {
    private:  /* Fields */
        T m_value;  // Backing field

    public:  /* Constructors & Destructor */
        AutoProp() noexcept;
        AutoProp(const AutoProp &ap) noexcept;
        AutoProp(const T &value) noexcept;

        virtual ~AutoProp() = default;

    public:  /* Operators */
        AutoProp &operator=(const T &value) noexcept;
        AutoProp &operator=(const AutoProp &ap) noexcept;

        T operator+(const T &value) const;

        AutoProp &operator+=(const T &value);
        AutoProp &operator+=(const AutoProp &ap);

        /// Bitwise left shift operator
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const AutoProp &ap) {
            return (os << ap.get());
        };

    public:  /* Methods */
        void set(const T &value) noexcept;

        const T get() const noexcept override;
        T get() noexcept override;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp() noexcept
{
    this->m_value = T();
    this->m_ptr = static_cast<T *>(&m_value);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const AutoProp &ap) noexcept
{
    this->m_value = ap.get();
    this->m_ptr = static_cast<T *>(&m_value);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const T &value) noexcept
{
    this->m_value = value;
    this->m_ptr = static_cast<T *>(&m_value);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const T &value)
noexcept {
    m_value = value;
    this->m_ptr = static_cast<T *>(&m_value);
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const AutoProp &ap)
noexcept {
    m_value = ap.get();
    this->m_ptr = static_cast<T *>(&m_value);
    return *this;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline T Scan::AutoProp<T>::operator+(const T &value) const
{
    return static_cast<T>(m_value + static_cast<T>(value));
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const T &value)
{
    m_value = operator+(value);
    this->m_ptr = static_cast<T *>(&m_value);
    return *this;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const AutoProp &ap)
{
    m_value = operator+(ap.get());
    this->m_ptr = static_cast<T *>(&m_value);
    return operator+=(ap.get());
}

/// ***
/// Backing field object specifier
/// ***
template<class T>
inline void Scan::AutoProp<T>::set(const T &value) noexcept
{
    operator=(value);
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const T Scan::AutoProp<T>::get() const noexcept
{
    return m_value;
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline T Scan::AutoProp<T>::get() noexcept
{
    return m_value;
}

#endif // !AUTOPROP_H
