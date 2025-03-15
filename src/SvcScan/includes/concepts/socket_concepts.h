/*
* @file
*     socket_concepts.h
* @brief
*     Header file for network socket concept constraints.
*/
#pragma once

#ifndef SCAN_SOCKET_CONCEPTS_H
#define SCAN_SOCKET_CONCEPTS_H

#include "../inet/sockets/tcp_client.h"
#include "../inet/sockets/tls_client.h"
#include "concepts.h"

namespace scan
{
    /**
    * @brief
    *     Require that the given type is a TCP socket client smart pointer.
    */
    template<class P>
    concept ClientPtr = SmartPtrOfAny<P, TcpClient, TlsClient>;
}

#endif // !SCAN_SOCKET_CONCEPTS_H
