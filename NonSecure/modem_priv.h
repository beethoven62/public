/** @file modem_priv.h
 *
 * @brief       Modem module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup modem
 * @{
 */

#ifndef __NRF_MODEM_PRIV_H__
#define __NRF_MODEM_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "modem.h"

/***************************************************************************************************************************
 * Private constants and macros
 */
#define SERVER_NAME_LENGTH_MAX              ( 64 )
#define ADDRESS_TABLE_ENTRY_CNT             ( 5 )
#define RECEIVE_RETRY_MAX                   ( 20 )
#define MIN_WAIT_MS                         ( 20 )

#define SHM_TX_MAX                  (128) //8
#define SHM_TX_CHUNK_SIZE           (NRF_MODEM_SHMEM_TX_SIZE / SHM_TX_MAX)

/***************************************************************************************************************************
 * Private data structures and typedefs
 */
typedef struct {
    bool used;
    uint32_t group_id;
} shm_tx_tbl_entry_t;

typedef struct AddressTableHdl
{
    char server_name[ ADDRESS_TABLE_ENTRY_CNT ][ SERVER_NAME_LENGTH_MAX ];
    nrf_sockaddr_in_t server_info[ ADDRESS_TABLE_ENTRY_CNT ][ sizeof( nrf_sockaddr_in_t ) ];
    int ( *eval )( bool, const void *, void * );
    //int ( *search )( void *, void * );
} address_table_hdl_t;

typedef struct
{
    TaskHandle_t                handle;
    TickType_t                  start_time;
} sleeping_task_t;

typedef struct
{
    int32_t                     fd;
    uint32_t                    ipaddr;
} socket_t;

typedef struct
{
    bool                        is_init;
    bool                        is_registered;
    uint32_t                    network_time_ms;
    socket_t                    socket[ ADDRESS_TABLE_ENTRY_CNT ];
    sleeping_task_t             sleeping_task;
    address_table_hdl_t         address_table_hdl;
    SemaphoreHandle_t           mutex_handle;
} modem_obj_t;

#ifndef NRFXLIB_V1
typedef struct nrf_modem_fault_info nrf_modem_fault_info_t;
typedef struct nrf_modem_init_params nrf_modem_init_params_t;
#endif

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @param[out]  did_reset_system    (Optional) Indicates whether the system was reset by Watchdog.
 * @return      Error code.
 */
static error_code_module_t modem_Init( void );

/**
 * @brief       Modem AT command interface.
 * @param[in]   cmd       Modem command input.
 * @param[out]  response  Modem response.
 * @return      Error code.
 */
static error_code_module_t modem_ATCmd( char *cmd, char *response );

/**
 * @brief       Modem start sequence for connection.
 * @return      Error code.
 */
static error_code_module_t modem_Start( void );

/**
 * @brief       Modem stop sequence for connection.
 * @return      Error code.
 */
static error_code_module_t modem_Stop( void );

/**
 * @brief       Implements the socket connection to a remote server.
 * @details     API to create as socket that supports most transport protocol.
 * @param[in]   transport_protocol  Network transport protocol such as UDP, TCP, TLS and MTLS (mutually auth TLS)
 * @param[in]   host_name           Host-name of the server
 * @param[in]   port                IP port of the server
 * @param[in]   receive_timeout_ms  Receiving time-out in milliseconds
 * @param[in]   send_timeout_ms     Sending time-out in milliseconds
 *
 * @return:     x >= 0 , where x is socket file descriptor (FD)
 *              x <  0 , where x is error status if not successful
 *
 */
static int32_t modem_Connect( const char *transport_protocol,
                              const char *host_name,
                              uint16_t port,
                              uint32_t receive_timeout_ms,
                              uint32_t send_timeout_ms );

/**
 * @brief       Implements the socket connection to a remote server.
 * @details     API to create as socket that supports most transport protocol.
 * @param[in]   socket              Socket file descriptor (FD)
 *
 * @return:     zero when successful
 *              negative when failed
 */
static int32_t modem_Disconnect( int32_t socket );

/**
 * @brief Get modem connection state
 * @return      connected = true
 */
static bool modem_Registered( void );

/**
 * @brief       Prints a report on the state of the modem and its parts
 */
static void modem_Status( void );

/**
 * @brief       Implements a blocking socket receive from a remote server.
 * @param[in]   fd                  Socket file descriptor (FD)
 * @param[in]   buf                 User supplied buffer pointer
 * @param[in]   size                Maximum size of buffer
 *
 * @return:     number of bytes received when successful
 *              negative when error
 */
static int32_t modem_Receive( int32_t fd, uint8_t *buf, uint32_t size );

/**
 * @brief       Implements a blocking socket send to a remote server.
 * @param[in]   fd                  Socket file descriptor (FD)
 * @param[in]   buf                 User supplied buffer pointer
 * @param[in]   size                Maximum size of buffer
 *
 * @return:     number of bytes sent when successful
 *              negative when error
 */
static int32_t modem_Send( int32_t fd, uint8_t *buf, uint32_t size );
/**
 * @brief case agnostic string search.
 * @param[in]   buffer          String buffer to be searched.
 * @param[in]   search_string   Search string
 */
static char *modem_StriStr( const char *buffer, const char *search_string );

/**
 * @brief case agnostic string search.
 * @param[in]   buffer          String buffer to be searched.
 * @param[in]   strip           Strip character
 */
static void modem_Strip( char *buffer, char strip );

/**
 * @brief Get network time.
 * @param[out]  network_time_ms     Network time (converted to milliseconds)
 * @return  error code
 */
static error_code_module_t modem_GetTime( uint32_t *network_time_ms );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief Implements the task which interfaces with the LTE modem.
 * @param[in]   cNotification   Modem response.
 */
static void ModemNotificationCb( const char *notification );

/**
 * @brief Up case string.
 * @param[in/out]   s           String buffer to be up-cased.
 */
static void to_upper( char *s );

/**
 * @brief Initialize remote server address table.
 */
static void address_table_init( void );

/**
 * @brief Maintains remote server address table.
 * @param[in]       search_only     Perform search only (true)
 * @param[in]       name            URL string
 * @param[in]       data_ptr        Socket address

 * @return status:
 *     0  : add successful
 *     1  : existing
 *    -1  : failed
 */
static int eval_address_table( bool search_only, const void * name, void * data_ptr );

#ifndef NRFXLIB_V1
static void modem_Fault( struct nrf_modem_fault_info *fault_info );
#endif

#endif /* __NRF_MODEM_PRIV_H__ */

/**
 * @}
 */
