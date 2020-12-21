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
    private: /* Fields */
        T m_value; // Backing field

    public: /* Constructors & Destructor */
        AutoProp();
        AutoProp(const AutoProp &ap);
        AutoProp(const T &value);

        virtual ~AutoProp() = default;

    public: /* Operators */
        AutoProp &operator=(const T &value);
        AutoProp &operator=(const AutoProp &ap);

        T operator+(const T &value) const;

        AutoProp &operator+=(const T &value);
        AutoProp &operator+=(const AutoProp &ap);

        /// Left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const AutoProp &ap) {
            return (os << ap.get());
        };

    public: /* Methods */
        void set(const T &value);

        const T get() const override;
        const T get() override;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp()
{
    this->operator=(m_value);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const AutoProp &ap)
{
    this->operator=(ap);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const T &value)
{
    this->operator=(value);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const T &value)
{
    this->m_value = value;
    this->m_valueptr = &m_value;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const AutoProp &ap)
{
    return this->operator=(ap.get());
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline T Scan::AutoProp<T>::operator+(const T &value) const
{
    return (this->m_value + value);
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const T &value)
{
    this->m_value = operator+(value);
    return *this;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const AutoProp &ap)
{
    return this->operator+=(ap.get());
}

/// ***
/// Backing field object specifier
/// ***
template<class T>
inline void Scan::AutoProp<T>::set(const T &value)
{
    this->operator=(value);
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const T Scan::AutoProp<T>::get() const
{
    return this->m_value;
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const T Scan::AutoProp<T>::get()
{
    return this->m_value;
}

#endif // !AUTOPROP_H
