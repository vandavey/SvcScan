/*
* @file
*     socket_concepts.h
* @brief
*     Header file for network socket concept constraints.
*/
#pragma once

#ifndef SOCKET_CONCEPTS_H
#define SOCKET_CONCEPTS_H

#include "../inet/sockets/tls_client.h"
#include "type_concepts.h"

namespace scan
{
    /**
    * @brief  Require that the given type is a TCP socket client.
    */
    template<class T>
    concept NetClient = SameAsAny<T, TcpClient, TlsClient>;

    /**
    * @brief  Require that the given type is a TCP socket client smart pointer.
    */
    template<class T>
    concept NetClientPtr = SmartPtr<T, TcpClient> || SmartPtr<T, TlsClient>;
}

#endif // !SOCKET_CONCEPTS_H
