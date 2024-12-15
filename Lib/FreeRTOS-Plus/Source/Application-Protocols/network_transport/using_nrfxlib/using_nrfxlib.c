/**
 * @file using_nrfxlib.c
 *
 * @brief TLS transport interface implementations. This implementation uses nrfxlib.
 *
 *  This interface is the key to make FreeRTOS libraries such as MQTT working in nRF9160 SiP.
 */

/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* TLS transport header. */
//#include "nrfxlib_config.h"
#include "using_nrfxlib.h"

/* FreeRTOS Socket wrapper include. */
#include "sockets_wrapper.h"

/* Others */
#include "defs.h"
#include "init.h"
#include "thread.h"
#include "log.h"
#include "cli.h"

#include "modem.h"



/*----------------------------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer as seen below whenever the header file
 * of this transport implementation is included to your project.
 *
 * @note When using multiple transports in the same compilation unit,
 *       define this pointer as void *.
 */
struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

static LogOutput_t xLogOutput;


/*----------------------------------------------------------------------------*/

void TLS_FreeRTOS_init( void )
{
    xLogOutput.xHandle = xGetLogHandle();
}

/*----------------------------------------------------------------------------*/

TlsTransportStatus_t TLS_FreeRTOS_Connect( NetworkContext_t * pNetworkContext,
                                           const char * pHostName,
                                           uint16_t port,
                                           const NetworkCredentials_t * pNetworkCredentials,
                                           uint32_t receiveTimeoutMs,
                                           uint32_t sendTimeoutMs )
{
    TlsTransportParams_t * pTlsTransportParams = NULL;
    int32_t err;
    char buffer[128];
    
    sprintf( buffer, "0,0,TLS,%s,%u,0,0", pHostName, port );
    
    LOG( DebugLog, "TLS_FreeRTOS_Connect: %s", buffer );

    pTlsTransportParams = pNetworkContext->pParams;
    if ( pTlsTransportParams == NULL )
    {
        return TLS_TRANSPORT_INVALID_PARAMETER;
    }

    /* The output socket will be stored to `tlsSocket` */
    err = modem_socket_cmd( MODEM_SCOKET_CONNECT, &pTlsTransportParams->tlsSocket, buffer, 0 );

    return 0;
}

/*----------------------------------------------------------------------------*/

void TLS_FreeRTOS_Disconnect( NetworkContext_t * pNetworkContext )
{
    TlsTransportParams_t * pTlsTransportParams = NULL;
    int32_t err;
    char buffer[128];

    pTlsTransportParams = pNetworkContext->pParams;
    if ( pTlsTransportParams == NULL )
    {
        return;
    }

    sprintf( buffer, "%d", pTlsTransportParams->tlsSocket ); // TODO: use directly the socket param
    LOG( DebugLog, "TLS_FreeRTOS_Disconnect: %s", buffer );

    err = modem_socket_cmd( MODEM_SCOKET_DISCONNECT, &pTlsTransportParams->tlsSocket, buffer, 0 );

    return;
}

/*----------------------------------------------------------------------------*/

int32_t TLS_FreeRTOS_send( NetworkContext_t * pNetworkContext,
                           const void * pBuffer,
                           size_t bytesToSend )
{
    TlsTransportParams_t * pTlsTransportParams = NULL;
    int32_t err;

    pTlsTransportParams = pNetworkContext->pParams;
    if ( pTlsTransportParams == NULL )
    {
        return -1;
    }
    // LOG( DebugLog, "TLS_FreeRTOS_send: socket=%d", pTlsTransportParams->tlsSocket );

    err = modem_socket_cmd( MODEM_SCOKET_SEND, &pTlsTransportParams->tlsSocket, (char *)pBuffer, bytesToSend );

    return err;
}

/*----------------------------------------------------------------------------*/

int32_t TLS_FreeRTOS_recv( NetworkContext_t * pNetworkContext,
                           void * pBuffer,
                           size_t bytesToRecv )
{
    TlsTransportParams_t * pTlsTransportParams = NULL;
    int32_t err;

    pTlsTransportParams = pNetworkContext->pParams;
    if ( pTlsTransportParams == NULL )
    {
        return -1;
    }
    // LOG( DebugLog, "TLS_FreeRTOS_recv: socket=%d", pTlsTransportParams->tlsSocket );

    err = modem_socket_cmd( MODEM_SCOKET_RECV, &pTlsTransportParams->tlsSocket, pBuffer, bytesToRecv );

    return err;
}
/*----------------------------------------------------------------------------*/
