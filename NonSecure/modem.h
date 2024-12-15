/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      modem.h
 * @details   LTE modem on the nRF9160DK and the Thingy:91 demo boards.
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup Modem
 * @{
 */
#ifndef __MODEM_H__
#define __MODEM_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <nrf.h>
#include <nrf_modem.h>
#include <nrf_modem_os.h>
#include <nrf_modem_at.h>
#include <nrf_socket.h>
#include <nrf_errno.h>
#include <nrfx_ipc.h>
#ifdef NRFXLIB_V1
#include <nrf_modem_platform.h>
#include <nrf_modem_limits.h>
#endif
#include "os.h"
#include "uart.h"
#include "dmm.h"
#include "app.h"
#include "log.h"
#include "tls.h"
#include "eelcodes.h"
#include "embedded_cli.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

#define NRF_SEND_TIMEOUT                    ( 4 )
#define NRF_RECV_TIMEOUT                    ( 3 )
#define NRF_MODEM_SHMEM_TX_SIZE             ( 0x1A00 )
#define NRF_MODEM_SHMEM_RX_SIZE             ( 0x1A00 )
#define NRF_MODEM_SHMEM_TRACE_SIZE          ( 0x3000 )
#define TOTAL_NRF_SHMEM_SIZE                ( NRF_MODEM_SHMEM_CTRL_SIZE + NRF_MODEM_SHMEM_TX_SIZE + NRF_MODEM_SHMEM_RX_SIZE + NRF_MODEM_SHMEM_TRACE_SIZE )
#define NRF_MODEM_NETWORK_IRQ               IPC_IRQn
#ifdef NRFXLIB_V1
#define TRACE_IRQ                           EGU2_IRQn
#define TRACE_IRQ_PRIORITY                  6
#define TRACE_IRQ_HANDLER                   EGU2_IRQHandler
#define NRF_SO_SEC_HOSTNAME                 NRF_SO_HOSTNAME
#define NRF_SO_SEC_CIPHERSUITE_LIST         NRF_SO_CIPHERSUITE_LIST
#else
#define NRF_MODEM_NETWORK_IRQ_PRIORITY      ( 0 )
#endif

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

#ifndef NRFXLIB_V1
typedef struct nrf_sockaddr_in nrf_sockaddr_in_t;
typedef struct nrf_sockaddr nrf_sockaddr_t;
#endif

/**
 * Specifies the public interface functions of the modem driver.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    error_code_module_t ( *ATCmd )( char *cmd, char *response );
    error_code_module_t ( *Start )( void );
    error_code_module_t ( *Stop )( void );
    int32_t             ( *Connect )( const char *transport_protocol,
                                      const char *host_name,
                                      uint16_t port,
                                      uint32_t receive_timeout_ms,
                                      uint32_t send_timeout_ms );
    int32_t             ( *Disconnect )( int32_t socket );
    bool                ( *Registered )( void );
    void                ( *Status )( void );
    int32_t             ( *Receive)( int32_t fd, uint8_t *buf, uint32_t size );
    int32_t             ( *Send )( int32_t fd, uint8_t *buf, uint32_t size );
    char*               ( *StriStr )( const char *buffer, const char *search_string );
    void                ( *Strip )( char *buffer, char strip );
    error_code_module_t ( *GetTime )( uint32_t *network_time_ms );
} const modem_interface_t;

/* Create one contiguous memory space for the three buffers required by the modem driver */
typedef struct {
    uint8_t nrf_modem_ctrl[ NRF_MODEM_SHMEM_CTRL_SIZE ];
    uint8_t nrf_modem_tx[ NRF_MODEM_SHMEM_TX_SIZE ];
    uint8_t nrf_modem_rx[ NRF_MODEM_SHMEM_RX_SIZE ];
#if CONFIG_NRF_MODEM_LIB_TRACE_ENABLED
    uint8_t nrf_modem_trace[ NRF_MODEM_SHMEM_TRACE_SIZE ];
#endif
} nrf_modem_bufs_t;

/***************************************************************************************************************************
 * Public variables
 */

extern modem_interface_t modem;

#endif /* __MODEM_H__ */

/**
 * @} Modem
 */

/**
 * @} Driver
 */
