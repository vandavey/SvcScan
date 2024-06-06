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
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include "../inet/net_defs.h"

namespace scan
{
    /**
    * @brief
    *     Require that a type is a valid HTTP message body type.
    */
    template<class T>
    concept HttpBody = requires(T)
    {
        http::request<T>{};
        http::response<T>{};
    };

    /**
    * @brief
    *     Require that a type is a valid HTTP message type.
    */
    template<class T>
    concept HttpMessage = requires(T r_msg, stream_t r_stream, flat_buffer r_buff)
    {
        http::read(r_stream, r_buff, r_msg);
        http::write(r_stream, r_msg);
    };
}

#endif // !SCAN_HTTP_CONCEPTS_H
