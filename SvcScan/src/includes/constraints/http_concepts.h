/*
*  http_concepts.h
*  ---------------
*  Header file containing HTTP message concept constraints
*/
#pragma once

#ifndef HTTP_CONCEPTS_H
#define HTTP_CONCEPTS_H

#include <sdkddkver.h>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/string_body.hpp>

namespace scan
{
    /// ***
    /// Require that a given HTTP messages body type is 'dynamic_body'
    /// ***
    template<class T>
    concept DynamicBody = std::same_as<T, boost::beast::http::dynamic_body>;

    /// ***
    /// Require that a given HTTP messages body type is 'string_body'
    /// ***
    template<class T>
    concept StringBody = std::same_as<T, boost::beast::http::string_body>;

    /// ***
    /// Require that a given type is a valid HTTP messages body type
    /// ***
    template<class T>
    concept HttpBody = DynamicBody<T> || StringBody<T>;
}

#endif // !HTTP_CONCEPTS_H
