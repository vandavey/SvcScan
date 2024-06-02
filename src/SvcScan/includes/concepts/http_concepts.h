/*
* @file
*     http_concepts.h
* @brief
*     Header file for HTTP concept constraints.
*/
#pragma once

#ifndef SCAN_HTTP_CONCEPTS_H
#define SCAN_HTTP_CONCEPTS_H

#include <sdkddkver.h>
#include <boost/beast/http/dynamic_body.hpp>
#include "../inet/net_defs.h"
#include "concepts.h"

namespace scan
{
    /**
    * @brief
    *     Require that the given type is a valid HTTP message body type.
    */
    template<class T>
    concept HttpBody = SameAsAny<T, string_body, http::dynamic_body>;
}

#endif // !SCAN_HTTP_CONCEPTS_H
