/*
*  http_concepts.h
*  ---------------
*  Header file containing HTTP message concept constraints
*/
#pragma once

#ifndef HTTP_CONCEPTS_H
#define HTTP_CONCEPTS_H

#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/string_body.hpp>
#include "type_concepts.h"

namespace scan
{
    /// ***
    /// Require that the given type is a valid HTTP message body type
    /// ***
    template<class T>
    concept HttpBody = AnySameAs<
        T,
        boost::beast::http::dynamic_body,
        boost::beast::http::string_body
    >;
}

#endif // !HTTP_CONCEPTS_H
