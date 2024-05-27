/*
* @file
*     http_concepts.h
* @brief
*     Header file for HTTP concept constraints.
*/
#pragma once

#ifndef HTTP_CONCEPTS_H
#define HTTP_CONCEPTS_H

#include <sdkddkver.h>
#include <boost/beast/http/dynamic_body.hpp>
#include "../inet/net_defs.h"
#include "type_concepts.h"

namespace scan
{
    /**
    * @brief  Require that the given type is a valid HTTP message body type.
    */
    template<class T>
    concept HttpBody = SameAsAny<T, string_body, http::dynamic_body>;
}

#endif // !HTTP_CONCEPTS_H
