/*
*  autoprop.h
*  ----------
*  Header file for object property class template
*  using an automatic backing field
*/
#pragma once

#ifndef AUTO_PROP_H
#define AUTO_PROP_H

#include "property.h"

namespace Scan
{
    /// ***
    /// Property that automatically encapsulates
    /// a backing field of type <T>
    /// ***
    template<class T>
    class AutoProp : public Property<T>
    {
    public:  /* Types */
        using value_type = typename Property<T>::value_type;

    private:  /* Fields */
        value_type m_value;  // Backing field

    public:  /* Constructors & Destructor */
        AutoProp();
        AutoProp(const AutoProp &ap);
        explicit AutoProp(const value_type &val);

        virtual ~AutoProp() = default;

    public:  /* Operators */
        AutoProp &operator=(const value_type &val) noexcept;
        AutoProp &operator=(const AutoProp &ap) noexcept;

        T operator+(const value_type &val) const;

        AutoProp &operator+=(const value_type &val);
        AutoProp &operator+=(const AutoProp &ap);

        /// Bitwise left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &os,
                                               const AutoProp &ap) {
            return (os << ap.get());
        };

    public:  /* Methods */
        void set(const value_type &val);

        const typename Property<T>::value_type get() const override;
        typename Property<T>::value_type get() override;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp()
{
    this->m_value = value_type();
    this->m_ptr = static_cast<value_type *>(&m_value);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const AutoProp &ap)
{
    this->m_value = ap.m_value;
    this->m_ptr = static_cast<value_type *>(&m_value);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::AutoProp<T>::AutoProp(const value_type &val)
{
    this->m_value = val;
    this->m_ptr = static_cast<value_type *>(&m_value);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const value_type &val)
noexcept {
    m_value = val;
    this->m_ptr = static_cast<value_type *>(&m_value);

    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator=(const AutoProp &ap)
noexcept {
    m_value = ap.get();
    this->m_ptr = static_cast<value_type *>(&m_value);

    return *this;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline T Scan::AutoProp<T>::operator+(const value_type &val) const
{
    return static_cast<value_type>(m_value + static_cast<value_type>(val));
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const value_type &val)
{
    m_value = operator+(val);
    this->m_ptr = static_cast<value_type *>(&m_value);

    return *this;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::AutoProp<T> &Scan::AutoProp<T>::operator+=(const AutoProp &ap)
{
    m_value = operator+(ap.get());
    this->m_ptr = static_cast<value_type *>(&m_value);

    return operator+=(ap.get());
}

/// ***
/// Backing field object specifier
/// ***
template<class T>
inline void Scan::AutoProp<T>::set(const value_type &val)
{
    operator=(val);
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline const typename Scan::Property<T>::value_type Scan::AutoProp<T>::get()
const {
    return static_cast<value_type>(m_value);
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline typename Scan::Property<T>::value_type Scan::AutoProp<T>::get()
{
    return static_cast<value_type>(m_value);
}

#endif // !AUTO_PROP_H
