/*
* @file
*     http_concepts.h
* @brief
*     Header file for HTTP concept constraints.
*/
#pragma once

#ifndef SCAN_HTTP_CONCEPTS_H
#define SCAN_HTTP_CONCEPTS_H

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include "../inet/net_aliases.h"

namespace scan
{
    /**
    * @brief
    *     Require that a type is a valid HTTP message body type.
    */
    template<class T>
    concept HttpBody = requires
    {
        http::request<T>{};
        http::response<T>{};
    };

    /**
    * @brief
    *     Require that a type is a valid HTTP message type.
    */
    template<class T>
    concept HttpMessage = requires(stream_t& r_stream, flat_buffer_t& r_buffer, T& r_msg)
    {
        http::read(r_stream, r_buffer, r_msg);
        http::write(r_stream, r_msg);
    };
}

#endif // !SCAN_HTTP_CONCEPTS_H
