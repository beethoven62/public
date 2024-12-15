/**
 * @file using_nrfxlib.h
 * @brief TLS transport interface header.
 */

#ifndef USING_NRFXLIB
#define USING_NRFXLIB

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Logging related header files are required to be included in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define LIBRARY_LOG_NAME and  LIBRARY_LOG_LEVEL.
 * 3. Include the header file "logging_stack.h".
 */

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Logging configuration for the Sockets. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "TlsTransport"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif

/* Prototype for the function used to print to console on Windows simulator
 * of FreeRTOS.
 * The function prints to the console before the network is connected;
 * then a UDP port after the network has connected. */
extern void vLoggingPrintf( const char * pcFormatString,
                            ... );

/* Map the SdkLog macro to the logging function to enable logging
 * on Windows simulator. */
#ifndef SdkLog
//    #error "SdkLog defined already"
//#else
    #define SdkLog( message )    vLoggingPrintf( message )
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/* FreeRTOS+TCP include. */
#include "FreeRTOS_Sockets.h"

/* Transport interface include. */
#include "transport_interface.h"

/* SSL TLS includes. */
// FIXME: Eval if required?

/**
 * @brief Secured connection context.
 */
typedef struct SSLContext
{
    char *dummy_str;
    int32_t dummy_int;
} SSLContext_t;

/**
 * @brief Parameters for the network context of the transport interface
 * implementation that uses mbedTLS and FreeRTOS+TCP sockets.
 */
typedef struct TlsTransportParams
{
    Socket_t tcpSocket;
    SSLContext_t sslContext;
    int32_t tlsSocket;
} TlsTransportParams_t;

/**
 * @brief Contains the credentials necessary for tls connection setup.
 */
typedef struct NetworkCredentials
{
    /**
     * @brief To use ALPN, set this to a NULL-terminated list of supported
     * protocols in decreasing order of preference.
     *
     * See [this link]
     * (https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/)
     * for more information.
     */
    char ** pAlpnProtos;

    /**
     * @brief Disable server name indication (SNI) for a TLS session.
     */
    BaseType_t disableSni;

    const uint8_t * pRootCa;     /**< @brief String representing a trusted server root certificate. */
    size_t rootCaSize;           /**< @brief Size associated with #NetworkCredentials.pRootCa. */
    const uint8_t * pClientCert; /**< @brief String representing the client certificate. */
    size_t clientCertSize;       /**< @brief Size associated with #NetworkCredentials.pClientCert. */
    const uint8_t * pPrivateKey; /**< @brief String representing the client certificate's private key. */
    size_t privateKeySize;       /**< @brief Size associated with #NetworkCredentials.pPrivateKey. */
} NetworkCredentials_t;

/**
 * @brief TLS Connect / Disconnect return status.
 */
typedef enum TlsTransportStatus
{
    TLS_TRANSPORT_SUCCESS = 0,         /**< Function successfully completed. */
    TLS_TRANSPORT_INVALID_PARAMETER,   /**< At least one parameter was invalid. */
    TLS_TRANSPORT_INSUFFICIENT_MEMORY, /**< Insufficient memory required to establish connection. */
    TLS_TRANSPORT_INVALID_CREDENTIALS, /**< Provided credentials were invalid. */
    TLS_TRANSPORT_HANDSHAKE_FAILED,    /**< Performing TLS handshake with server failed. */
    TLS_TRANSPORT_INTERNAL_ERROR,      /**< A call to a system API resulted in an internal error. */
    TLS_TRANSPORT_CONNECT_FAILURE      /**< Initial connection to the server failed. */
} TlsTransportStatus_t;

/**
 * @brief Create a TLS connection with FreeRTOS sockets.
 *
 * @param[out] pNetworkContext Pointer to a network context to contain the
 * initialized socket handle.
 * @param[in] pHostName The hostname of the remote endpoint.
 * @param[in] port The destination port.
 * @param[in] pNetworkCredentials Credentials for the TLS connection.
 * @param[in] receiveTimeoutMs Receive socket timeout.
 * @param[in] sendTimeoutMs Send socket timeout.
 *
 * @return #TLS_TRANSPORT_SUCCESS, #TLS_TRANSPORT_INSUFFICIENT_MEMORY, #TLS_TRANSPORT_INVALID_CREDENTIALS,
 * #TLS_TRANSPORT_HANDSHAKE_FAILED, #TLS_TRANSPORT_INTERNAL_ERROR, or #TLS_TRANSPORT_CONNECT_FAILURE.
 */
TlsTransportStatus_t TLS_FreeRTOS_Connect( NetworkContext_t * pNetworkContext,
                                           const char * pHostName,
                                           uint16_t port,
                                           const NetworkCredentials_t * pNetworkCredentials,
                                           uint32_t receiveTimeoutMs,
                                           uint32_t sendTimeoutMs );

/**
 * @brief Gracefully disconnect an established TLS connection.
 *
 * @param[in] pNetworkContext Network context.
 */
void TLS_FreeRTOS_Disconnect( NetworkContext_t * pNetworkContext );

/**
 * @brief Receives data from an established TLS connection.
 *
 * This is the TLS version of the transport interface's
 * #TransportRecv_t function.
 *
 * @param[in] pNetworkContext The Network context.
 * @param[out] pBuffer Buffer to receive bytes into.
 * @param[in] bytesToRecv Number of bytes to receive from the network.
 *
 * @return Number of bytes (> 0) received if successful;
 * 0 if the socket times out without reading any bytes;
 * negative value on error.
 */
int32_t TLS_FreeRTOS_recv( NetworkContext_t * pNetworkContext,
                           void * pBuffer,
                           size_t bytesToRecv );

/**
 * @brief Sends data over an established TLS connection.
 *
 * This is the TLS version of the transport interface's
 * #TransportSend_t function.
 *
 * @param[in] pNetworkContext The network context.
 * @param[in] pBuffer Buffer containing the bytes to send.
 * @param[in] bytesToSend Number of bytes to send from the buffer.
 *
 * @return Number of bytes (> 0) sent on success;
 * 0 if the socket times out without sending any bytes;
 * else a negative value to represent error.
 */
int32_t TLS_FreeRTOS_send( NetworkContext_t * pNetworkContext,
                           const void * pBuffer,
                           size_t bytesToSend );

void TLS_FreeRTOS_init( void );

#endif /* ifndef USING_NRFXLIB */
