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

namespace scan
{
    /// ***
    /// Property that automatically encapsulates
    /// a backing field of type <T>
    /// ***
    template<class T>
    class AutoProp : public Property<T>
    {
    public:  /* Types */
        using value_type = T;

    private:  /* Fields */
        value_type m_value;  // Backing field

    public:  /* Constructors & Destructor */
        AutoProp();
        AutoProp(const AutoProp &t_ap);
        AutoProp(const value_type &t_val);

        virtual ~AutoProp() = default;

    public:  /* Operators */
        operator value_type() const override;

        AutoProp &operator=(const value_type &t_val);
        AutoProp &operator=(const AutoProp &t_ap);

        T operator+(const value_type &t_val) const;

        AutoProp &operator+=(const value_type &t_val);
        AutoProp &operator+=(const AutoProp &t_ap);

        /// Bitwise left shift operator overload
        inline friend std::ostream &operator<<(std::ostream &t_os,
                                               const AutoProp &t_ap) {
            return (t_os << t_ap.get());
        };

    public:  /* Methods */
        void set(const value_type &t_val);

        value_type get() const override;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::AutoProp<T>::AutoProp()
{
    m_value = value_type();
    this->m_vptr = &m_value;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::AutoProp<T>::AutoProp(const AutoProp &t_ap)
{
    m_value = t_ap.m_value;
    this->m_vptr = &m_value;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::AutoProp<T>::AutoProp(const value_type &t_val)
{
    m_value = t_val;
    this->m_vptr = &m_value;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::AutoProp<T>::operator value_type() const
{
    return m_value;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::AutoProp<T> &scan::AutoProp<T>::operator=(const value_type &t_val)
{
    m_value = t_val;
    this->m_vptr = static_cast<value_type *>(&m_value);

    return *this;
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::AutoProp<T> &scan::AutoProp<T>::operator=(const AutoProp &t_ap)
{
    m_value = t_ap;
    this->m_vptr = static_cast<value_type *>(&m_value);

    return *this;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline T scan::AutoProp<T>::operator+(const value_type &t_val) const
{
    return m_value + t_val;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline scan::AutoProp<T> &scan::AutoProp<T>::operator+=(const value_type &t_val)
{
    m_value = operator+(t_val);
    this->m_vptr = &m_value;

    return *this;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline scan::AutoProp<T> &scan::AutoProp<T>::operator+=(const AutoProp &t_ap)
{
    m_value = operator+(t_ap);
    this->m_vptr = &m_value;

    return operator+=(t_ap);
}

/// ***
/// Backing field object specifier
/// ***
template<class T>
inline void scan::AutoProp<T>::set(const value_type &t_val)
{
    operator=(t_val);
}

/// ***
/// Backing field object accessor
/// ***
template<class T>
inline typename scan::AutoProp<T>::value_type scan::AutoProp<T>::get() const
{
    return m_value;
}

#endif // !AUTO_PROP_H
