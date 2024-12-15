/**
 * @addtogroup Driver
 * @{
 * @file      modem.c
 * @brief     Modem module
 * @details   LTE modem on the nRF9160DK and the Thingy:91 demo boards.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup Modem Modem module
 * @brief     LTE modem interface module
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "modem_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

modem_interface_t modem =
{
    .Init               = &modem_Init,
    .ATCmd              = &modem_ATCmd,
    .Start              = &modem_Start,
    .Stop               = &modem_Stop,
    .Connect            = &modem_Connect,
    .Disconnect         = &modem_Disconnect,
    .Registered         = &modem_Registered,
    .Status             = &modem_Status,
    .Receive            = &modem_Receive,
    .Send               = &modem_Send,
    .StriStr            = &modem_StriStr,
    .Strip              = &modem_Strip,
    .GetTime            = &modem_GetTime,
};

modem_obj_t modem_obj =
{
    .is_init            = false,
    .is_registered      = false,
    .network_time_ms    = 0,
    .sleeping_task      =
    {
        .handle         = 0,
        .start_time     = 0,
    },
};

static nrf_modem_bufs_t modem_shm __attribute__( ( section( ".modem_shm" ) ) );

/* Asynchronous modem responses */
static char *modem_response[] =
{
    "CESQ",
    "CSCON",
    "CEREG",
    "XOPNAME",
};

/*************************************************************************************************************************************
 * Private Functions Definition
 */

void nrf_modem_os_init( void )
{
#ifdef NRFXLIB_V1
    NVIC_SetPriority( NRF_MODEM_APPLICATION_IRQ, NRF_MODEM_APPLICATION_IRQ_PRIORITY );
    NVIC_ClearPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
    NVIC_EnableIRQ( NRF_MODEM_APPLICATION_IRQ );

    NVIC_SetPriority( TRACE_IRQ, TRACE_IRQ_PRIORITY );
    NVIC_ClearPendingIRQ( TRACE_IRQ );
    NVIC_EnableIRQ( TRACE_IRQ );
#endif

    /* Initialize the glue layer and required peripherals. */
    dmm.Init( dmm_handle_1, modem_shm.nrf_modem_tx, NRF_MODEM_SHMEM_TX_SIZE );
}

void *nrf_modem_os_shm_tx_alloc( size_t bytes )
{
    /* Allocate a buffer on the TX area of shared memory. */
    return dmm.Alloc( dmm_handle_1, bytes );
}

void nrf_modem_os_shm_tx_free( void *mem )
{
    /* Free a shared memory buffer in the TX area. */
    dmm.Free( dmm_handle_1, mem );
}

void *nrf_modem_os_alloc( size_t bytes )
{
    /* Allocate a buffer on the library heap. */
    void* ptr = NULL;

    if ( bytes > 0 )
    {
        // We simply wrap the FreeRTOS call into a standard form
        ptr = malloc( bytes );
    } // else NULL if there was an error

    return ptr;
}

void nrf_modem_os_free( void *mem )
{
    /* Free a memory buffer in the library heap. */
    if ( mem )
    {
        // We simply wrap the FreeRTOS call into a standard form
        free( mem );
        mem = NULL;
    }
}

void nrf_modem_os_busywait( int32_t usec )
{
    /* Busy wait for a given amount of microseconds. */
    //os.Delay( usec / 1000 );
    uint32_t i;
    for ( i = 0; i < usec; i++ )
    {
    }
}

int32_t nrf_modem_os_timedwait( uint32_t context, int32_t *timeout )
{
    /* Put a thread to sleep for a specific time or until an event occurs.
       Wait for the timeout.
       All waiting threads shall be woken by nrf_modem_event_notify.
       A blind return value of zero will cause a blocking wait. */

#ifndef NRFXLIB_V1
    if ( !nrf_modem_is_initialized() )
    {
        return -NRF_ESHUTDOWN;
    }
#endif

    TickType_t now = os.GetTickCount();
    TickType_t sleep_time;
    int32_t result;

    /*
     * Input timeout == 0, shut off timed wait
     */
    if ( *timeout == 0 )
    {
        result = NRF_ETIMEDOUT;
    }
    else
    {
        /*
         * If task is awake, put task to sleep for *timeout milliseconds
         * Otherwise, report back time left to sleep (*timeout) in milliseconds
         */
        if ( *timeout < 0 )
        {
            sleep_time = portMAX_DELAY;
        }
        else
        {
            sleep_time = os.Ms2Ticks( *timeout );
        }

        //Log.DebugPrint( "[modem_os_timedwait] sleeping: %d", *timeout >= 0 ? *timeout : -1 );
        modem_obj.sleeping_task.handle = os.GetTaskHandle();
        modem_obj.sleeping_task.start_time = now;
        os.TaskNotifyTake( false, sleep_time );
        if ( sleep_time == portMAX_DELAY )
        {
            result = 0;
        }
        else
        {
            /* Timed out?
             * No:  now < start time + sleep time (still sleeping) - return remaining time
             * Yes: now >= start time + sleep time (timed out) - return timed out error message
             */
            if ( now < modem_obj.sleeping_task.start_time + sleep_time )
            {
                *timeout -= os.Ticks2Ms( modem_obj.sleeping_task.start_time + sleep_time - now );
                *timeout = *timeout < MIN_WAIT_MS ? MIN_WAIT_MS : *timeout;
                result = 0;
            }
            else
            {
                *timeout = 0;
                result = NRF_ETIMEDOUT;
            }
        }
    }

    return result;
}

void nrf_modem_os_errno_set( int errno_val )
{
    /* Set OS errno. */
    errno = errno_val;
    //Log.ErrorPrint( "Error: %d", errno_val );
}

bool nrf_modem_os_is_in_isr( void )
{
    /* Check if executing in interrupt context. */
    return os.IsInsideInterrupt();
}

int nrf_modem_os_sem_init( void **sem, unsigned int initial_count, unsigned int limit )
{
    /* The function shall allocate and initialize a semaphore and return its address
       through the `sem` parameter. If an address of an already allocated semaphore is provided as
       an input, the allocation part is skipped and the semaphore is only reinitialized. */
    return 0;
}

void nrf_modem_os_sem_give( void *sem )
{
    /* Give a semaphore. */
}

int nrf_modem_os_sem_take( void *sem, int timeout )
{
    /* Try to take a semaphore with the given timeout. */
    return 0;
}

void nrf_modem_os_log( int level, const char *fmt, ... )
{
    char mdm_buf[ 128 ];

    if ( !nrf_modem_os_is_in_isr() )
    {
        /* Generic logging procedure. */
        va_list args;
        va_start( args, fmt );
        vsnprintf( mdm_buf, sizeof( mdm_buf ) - 1, fmt, args );
        Log.DebugPrint( "[Modem: %d]: %s", level, mdm_buf );
        va_end( args );
    }
}

void nrf_modem_os_logdump( int level, const char *str, const void *data, size_t len )
{
    /* Log hex representation of object. */
}

void IPC_IRQHandler( void )
{
    nrfx_ipc_irq_handler();
}

void ModemNotificationCb( const char * notification )
{
    uint32_t i;
    int32_t parameters[ 5 ];
    char *result;

    for ( i = 0; i < sizeof( modem_response ) / sizeof( char * ); i++ )
    {
        result = modem_StriStr( notification, modem_response[ i ] );
        if ( result != NULL )
        {
            break;
        }
    }

    if ( result != NULL )
    {
        switch ( i )
        {
        case 0:         // CESQ:    RSRP, RSRP Threshold, RSRQ, RSRQ Threshold
            result = modem_StriStr( notification, ":" ) + 1;
            sscanf( result, "%d, %d, %d, %d",
                    &parameters[ 0 ],
                    &parameters[ 1 ],
                    &parameters[ 2 ],
                    &parameters[ 3 ] );
            Log.DebugPrint( "RSRP: %d dBm\tRSRP Threshold: %d\tRSRQ: %d dBm\tRSRQ Threshold: %d",
                            parameters[ 0 ] - 141,
                            parameters[ 1 ],
                            ( parameters[ 2 ] - 40 ) / 2,
                            parameters[ 3 ] );
            break;
        case 1:         // CSCON:   a,b,c
            result = modem_StriStr( notification, ":" ) + 1;
            sscanf( result, "%d", &parameters[ 0 ] );
            Log.DebugPrint( "%s", parameters[ 0 ] == 1 ? "Connected" : "Idle" );
            break;
        case 2:         // CEREG:   a,"b","c",d,,,"e","f"
            result = modem_StriStr( notification, ":" ) + 1;
            sscanf( result, "%d", &parameters[ 0 ] );
            Log.DebugPrint( "%s", parameters[ 0 ] == 1 || parameters[ 0 ] == 5 ? "Registered" : "Not registered" );
            if ( ( parameters[ 0 ] == 1 ) || ( parameters[ 0 ] == 5 ) )
            {
                Log.InfoPrint( "Modem is registered on %s network", parameters[ 0 ] == 1 ? "home" : "roaming" );
                modem_obj.is_registered = true;
                modem_obj.network_time_ms = 0;
            }
            else
            {
                Log.InfoPrint( "Modem is unregistered" );
                modem_obj.is_registered = false;
            }
            break;
        case 3:         // XOPNAME: "a","b","c"
            break;
        }
    }
}

#ifndef NRFXLIB_V1
void nrf_modem_os_shutdown( void )
{
    /* Deinitialize the glue layer.
       When shutdown is called, all pending calls to nrf_modem_os_timedwait
       shall exit and return -NRF_ESHUTDOWN. */
}

void nrf_modem_os_event_notify( uint32_t context )
{
    /* Notify the application that an event has occurred.
       This shall wake all threads sleeping in nrf_modem_os_timedwait. */
    Log.Print( "Modem notify\r\n" );
}

void modem_Fault( nrf_modem_fault_info_t *fault_info )
{
    // TODO: handle modem fault
    Log.ErrorPrint( "Modem fault: PC = 0x%08x, Reason = %d", fault_info->program_counter, fault_info->reason );
}

int nrf_modem_os_sleep( uint32_t timeout )
{
    return 0;
}

unsigned int nrf_modem_os_sem_count_get(void *sem)
{
    return 0;
}
#else
const char *nrf_modem_os_log_strdup(const char *str)
{
    return str;
}

void nrf_modem_os_application_irq_set( void )
{
    NVIC_SetPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
}

void nrf_modem_os_application_irq_clear( void )
{
    NVIC_ClearPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
}

void EGU1_IRQHandler( void )
{
    nrf_modem_application_irq_handler();
    os.TaskNotifyGive( modem_obj.sleeping_task.handle );
}

void nrf_modem_os_trace_irq_set( void )
{
    NVIC_SetPendingIRQ( TRACE_IRQ );
}

void nrf_modem_os_trace_irq_clear( void )
{
    NVIC_ClearPendingIRQ( TRACE_IRQ );
}

void TRACE_IRQ_HANDLER( void )
{
    nrf_modem_trace_irq_handler();
}

int32_t nrf_modem_os_trace_put( const uint8_t * const p_buffer, uint32_t buf_len )
{
    // Store buffer to chosen medium.
    // Traces can be dropped if not needed.
#if CONFIG_NRF_MODEM_LIB_TRACE_ENABLED
    uart.Transmit( uart_util_port, ( uint8_t *)p_buffer, buf_len );
#endif

    return 0;
}
#endif

void to_upper( char *s )
{
    uint32_t i;

    if (s != NULL )
    {
        for ( i = 0; s[ i ] != 0; i++ )
        {
            s[ i ] = ( char ) toupper( ( int ) s[ i ] );
        }
    }
}

bool modem_Registered( void )
{
    return modem_obj.is_registered;
}

char *modem_StriStr( const char *buffer, const char *search_string )
{
    const char *p1 = buffer;
    const char *p2 = search_string;
    const char *r = *p2 == 0 ? buffer : 0;

    while( *p1 != 0 && *p2 != 0 )
    {
        if ( tolower( ( unsigned char )*p1 ) == tolower( ( unsigned char )*p2 ) )
        {
            if ( r == 0 )
            {
                r = p1;
            }

            p2++;
        }
        else
        {
            p2 = search_string;
            if( r != 0 )
            {
                p1 = r + 1;
            }

            if ( tolower( ( unsigned char )*p1 ) == tolower( ( unsigned char )*p2 ) )
            {
                r = p1;
                p2++;
            }
            else
            {
                r = 0;
            }
        }

        p1++;
    }

    return *p2 == 0 ? ( char * )r : 0;
}

void modem_Strip( char *buffer, char strip )
{
    char *p, c[ 2 ], *s = "\r\n,\"";
    uint32_t i;

    c[ 1 ] = 0;
    if ( strip == 0 )
    {
        for ( i = 0; i < strlen( s ); i++ )
        {
            c[ 0 ] = s[ i ];
            while ( ( p = modem_StriStr( buffer, c ) ) != NULL )
            {
                *p = ' ';
            }
        }
    }
    else
    {
        c[ 0 ] = strip;
        while ( ( p = modem_StriStr( buffer, c ) ) != NULL )
        {
            *p = ' ';
        }
    }
}

void address_table_init( void )
{
    memset( &modem_obj.address_table_hdl, 0, sizeof( address_table_hdl_t ) );
    modem_obj.address_table_hdl.eval = eval_address_table;
}

static int eval_address_table( bool search_only, const void * name, void * data_ptr )
{
    int status = -1;
    char *server_name = ( char * ) name;
    int i = 0;
    for ( ; i < ADDRESS_TABLE_ENTRY_CNT; i++ )
    {
        if ( strlen( modem_obj.address_table_hdl.server_name[ i ] ) == 0 )
        {
            if ( search_only )
            {
                status = -1;
                break;
            }
            nrf_sockaddr_in_t *addrinfo = ( nrf_sockaddr_in_t * )data_ptr;
            strcpy( modem_obj.address_table_hdl.server_name[ i ], server_name );

            memcpy( modem_obj.address_table_hdl.server_info[ i ], addrinfo, sizeof( nrf_sockaddr_in_t ) );
            Log.DebugPrint( "Added new address info: %s", modem_obj.address_table_hdl.server_name[ i ] );
            status = 0;
            break;
        }
        else if ( modem_StriStr( server_name, modem_obj.address_table_hdl.server_name[ i ] ) != NULL )
        {
            Log.DebugPrint( "Existing address info: %s", server_name );
            nrf_sockaddr_in_t **ptr2ptr = ( nrf_sockaddr_in_t ** )data_ptr;
            *ptr2ptr = modem_obj.address_table_hdl.server_info[ i ];
            status = 1;
            break;
        }
    }

    return status;
}

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t modem_Init( void )
{
    int32_t i, status;
    error_code_module_t error = NO_ERROR;

    nrf_modem_init_params_t modem_parms =
    {
        .shmem 	= {
            .ctrl   = {
                .base = ( uint32_t )modem_shm.nrf_modem_ctrl,
                .size = sizeof( modem_shm.nrf_modem_ctrl )
            },
            .tx     = {
                .base = ( uint32_t )modem_shm.nrf_modem_tx,
                .size = sizeof( modem_shm.nrf_modem_tx )
            },
            .rx     = {
                .base = ( uint32_t )modem_shm.nrf_modem_rx,
                .size = sizeof( modem_shm.nrf_modem_rx )
            },
#if CONFIG_NRF_MODEM_LIB_TRACE_ENABLED
            .trace  = {
                .base = ( uint32_t )modem_shm.nrf_modem_trace,
                .size = sizeof( modem_shm.nrf_modem_trace )
            },
#endif
        },

        .ipc_irq_prio   = NRF_MODEM_NETWORK_IRQ_PRIORITY,
#ifndef NRFXLIB_V1
        .fault_handler  = modem_Fault
#endif
    };

    if ( modem_obj.is_init == false )
    {
        Log.DebugPrint( "Initializing modem..." );
        Log.InfoPrint( "Modem shared memory: 0x%08x, 0x%08x, 0x%08x, 0x%08x",
                       ( uint32_t ) modem_parms.shmem.ctrl.base,
                       ( uint32_t ) modem_parms.shmem.tx.base,
                       ( uint32_t ) modem_parms.shmem.rx.base,
#if CONFIG_NRF_MODEM_LIB_TRACE_ENABLED
                       ( uint32_t ) modem_parms.shmem.trace.base
#else
                       0
#endif
                     );
        NVIC_SetPriority( NRF_MODEM_NETWORK_IRQ, NRF_MODEM_NETWORK_IRQ_PRIORITY );
        NVIC_ClearPendingIRQ( NRF_MODEM_NETWORK_IRQ );
        NVIC_EnableIRQ( NRF_MODEM_NETWORK_IRQ );

        nrf_modem_at_notif_handler_set( ModemNotificationCb );

#ifdef NRFXLIB_V1
        if ( ( status = nrf_modem_init( &modem_parms, NORMAL_MODE ) ) < 0 )
#else
        if ( ( status = nrf_modem_init( &modem_parms ) ) < 0 )
#endif
        {
            Log.ErrorPrint( "Modem initialization failed: %d", status );
            error = ERROR_MODEM_NOT_INIT;
        }
        else
        {
            modem_obj.network_time_ms = 0;
            nrf_modem_at_notif_handler_set( ModemNotificationCb );
            address_table_init();
            for ( i = 0; i < ADDRESS_TABLE_ENTRY_CNT; i++ )
            {
                modem_obj.socket[ i ].fd = -1;
                modem_obj.socket[ i ].ipaddr = 0;
            }
            modem_obj.mutex_handle = os.CreateMutex();
            modem_obj.is_init = true;
            Log.InfoPrint( "Modem initialization complete." );
        }
    }
    else
    {
        error = ERROR_MODEM_ALREADY_INIT;
    }

    return error;
}

error_code_module_t modem_ATCmd( char *cmd, char *response )
{
    int32_t error = 0;

    error = nrf_modem_at_cmd( response, SHORT_MSG_MAX, "%s", cmd );
    Log.DebugPrint( "-> %s", cmd );
    Log.DebugPrint( "<- %s", response );

    return error;
}

error_code_module_t modem_Start( void )
{
    error_code_module_t error = NO_ERROR;
    uint32_t i;
    char response[ SHORT_MSG_MAX ];
    char *cmd[] = {
        "AT+CFUN?",                                                                                         // Get LTE modem status
        "AT+CFUN=0",                                                                                        // Turn LTE modem on
        "AT%XMAGPIO=1,1,1,7,0,791,849,1,880,960,3,824,894,4,1574,1577,5,698,748,5,1710,2200,7,746,803",     // Antenna tuning according to schematics VSM1.53 rev C (for Qorvo QPC6082)
        "AT%XMAGPIO?",                                                                                      // Get antenna tuning parameters
        "AT%XMODEMTRACE=1,4",                                                                               // Enable modem trace (IP only)
        "AT%HWVERSION",                                                                                     // Print modem hardware version
        "AT+CGMR",                                                                                          // Get modem firmware version
        "AT+CGSN",                                                                                          // Get modem IMEI
        "AT%XOPCONF=10",                                                                                    // Workaround for SIM rotation (SIM REFRESH behavior). Only works for modem firmware v1.2.7 and higher versions.
        "AT%XSYSTEMMODE=1,0,0,0",                                                                           // Set system mode for CAT-M1
        "AT%XSYSTEMMODE?",                                                                                  // Get system mode
        "AT+CSCON=3",                                                                                       // Signal connection status level 3
        "AT+CNEC=24",                                                                                       // Report network error codes
        "AT%CESQ=1",                                                                                        // Extended signal quality
        "AT%XOPNAME=1",                                                                                     // Operator name indications
        "AT+CGDCONT=0,\"IP\",\"eseye1\"",                                                                   // Define PDP context
        //"AT+CGDCONT=0,\"IP\",\"ibasis.iot\"",                                                             // Define PDP context
        "AT+CGDCONT?",                                                                                      // Query active PDP contexts
        "AT+CEREG=5",                                                                                       // Network registration status
        "AT+CPSMS=0,,,\"11111111\",\"11111111\"",                                                           // Disable power saving mode
        "AT%CESQ=1",
        "AT%XOPNAME=1",
        "AT+CSCON=3",
        "AT+CNEC=24",
        "AT%XDATAPRFL?",
        "AT%XDATAPRFL=4",                                                                                   // Set data profile for better performance
        "AT+CFUN=1",                                                                                        // Turn LTE modem on
        "AT+CFUN?",                                                                                         // Get LTE modem status
        "AT%XDATAPRFL?",
        "AT%XTEMP?",
    };

    if ( modem_obj.is_init == true && os.TakeSemaphore( modem_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        // Run command sequence
        for ( i = 0; i < sizeof( cmd ) / sizeof( char* ); i++ )
        {
            modem.ATCmd( cmd[i], response );
            Log.DebugPrint( "%s", response );
            os.Delay( 100 );
        }

        os.GiveSemaphore( modem_obj.mutex_handle );
    }
    else
    {
        error = ERROR_MODEM_NOT_INIT;
    }

    return error;
}

error_code_module_t modem_Stop( void )
{
    error_code_module_t error = NO_ERROR;
    uint32_t i;
    char response[ SHORT_MSG_MAX ];
    char *cmd[] = {
        "AT+CFUN=0",                                                                                        // Turn LTE modem on
    };
    if ( modem_obj.is_init == true && os.TakeSemaphore( modem_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        // Run command sequence
        for ( i = 0; i < sizeof( cmd ) / sizeof( char* ); i++ )
        {
            modem.ATCmd( cmd[i], response );
            Log.DebugPrint( "%s", response );
            os.Delay( 100 );
        }

        os.GiveSemaphore( modem_obj.mutex_handle );
    }
    else
    {
        error = ERROR_MODEM_NOT_INIT;
    }

    return error;
}

int32_t modem_Connect( const char *transport_protocol,
                       const char *host_name,
                       uint16_t port,
                       uint32_t receive_timeout_ms,
                       uint32_t send_timeout_ms )
{
    int32_t err = 0;
    int fd = -1;
    struct nrf_addrinfo *addr_info = NULL;
    nrf_sockaddr_in_t *remote_addr = NULL;
    struct nrf_addrinfo hints =
    {
        .ai_family = NRF_AF_INET,
        .ai_socktype = NRF_SOCK_STREAM,
    };
    uint32_t receive_timeout_s = receive_timeout_ms / 1000;
    uint32_t send_timeout_s = send_timeout_ms / 1000;
    if ( receive_timeout_s < NRF_RECV_TIMEOUT )
    {
        Log.ErrorPrint( "modem_Connect() param receive_timeout_ms too low, rectify.." );
        receive_timeout_s = NRF_RECV_TIMEOUT;
    }
    if ( send_timeout_s < NRF_SEND_TIMEOUT )
    {
        Log.ErrorPrint( "modem_Connect() param send_timeout_ms too low, rectify.." );
        send_timeout_s = NRF_SEND_TIMEOUT;
    }
    struct nrf_timeval receive_timeout = {
        .tv_sec = receive_timeout_s,
        .tv_usec = 0,
    };
    struct nrf_timeval send_timeout = {
        .tv_sec = send_timeout_s,
        .tv_usec = 0,
    };

    err = modem_obj.address_table_hdl.eval( true, host_name, &remote_addr );
    if ( err == -1 )
    {
        err = nrf_getaddrinfo( host_name, NULL, &hints, &addr_info );
        if ( err == 0 )
        {
            Log.DebugPrint( "nrf_getaddrinfo() success" );
            // assert addr_info != NULL
            remote_addr = ( nrf_sockaddr_in_t * ) addr_info->ai_addr;
            // Add new entry
            err = modem_obj.address_table_hdl.eval( false, host_name, remote_addr );
            // assert err == 0
        }
        else
        {
            Log.ErrorPrint( "nrf_getaddrinfo() failed, server_addr == %s\r\n", ( addr_info == NULL ? "NULL" : "OBJECT" ) );
        }

        if ( err != 0 )
        {
            Log.ErrorPrint( "Error getting address info from (%s) (errorno: %d)", host_name, errno );   //a2kd0a956w8xr7-ats.iot.us-west-1.amazonaws.com
            return -1;
        }

        // check error for either getaddrinfo or eval function
#if DO_PING_SERVER // ICMP - Ping
        ping_argv.src = extract_client_ip();
        ping_argv.dest = remote_addr->sin_addr.s_addr;
        ping_argv.len = 144; // Ping data length
        Log.DebugPrint( "PING the Server with IP: 0x%X", ping_argv.dest );
        Log.DebugPrint( "Client info, IP: 0x%X", ping_argv.src );
        os.Delay( 500 );

        ping_server();
        errno = 0;    // Some servers do not support ICMP, ignore error!
#endif
    }
    // else, assert( err == 1 ) and assert( remote_addr != NULL )

    if ( remote_addr != NULL )
    {
        remote_addr->sin_port = nrf_htons( port );
        remote_addr->sin_len = sizeof( nrf_sockaddr_in_t );
        remote_addr->sin_family = NRF_AF_INET;

        Log.DebugPrint( "Server IP: %X -> %u.%u.%u.%u",
                        remote_addr->sin_addr.s_addr,
                        ( remote_addr->sin_addr.s_addr ) & 0xff,
                        ( remote_addr->sin_addr.s_addr >> 8 ) & 0xff,
                        ( remote_addr->sin_addr.s_addr >> 16 ) & 0xff,
                        ( remote_addr->sin_addr.s_addr >> 24 ) & 0xff );
        Log.DebugPrint( "Server port: %u", nrf_ntohs( remote_addr->sin_port ) );
    }
    else
    {
        Log.ErrorPrint( "Error evaluating address info for %s", host_name );
        return -1;
    }

    if ( modem_StriStr( transport_protocol, "TCP" ) != NULL )
    {
        fd = nrf_socket( NRF_AF_INET, NRF_SOCK_STREAM, NRF_IPPROTO_TCP );
        Log.DebugPrint( "Opening TCP socket handle: %d", fd );
    }
    else if ( modem_StriStr( transport_protocol, "TLS" ) != NULL )
    {
        fd = nrf_socket( NRF_AF_INET, NRF_SOCK_STREAM, NRF_SPROTO_TLS1v2 );
        Log.DebugPrint( "Opening TLS socket handle: %d", fd );
        if ( fd > -1 )
        {
            if ( tolower( transport_protocol[ 0 ] ) == 'm' )
            {
                Log.DebugPrint( "Setup mTLS protocol for %s", host_name );
                err = tls.Setup( fd, host_name, tag_mtls );
            }
            else
            {
                err = tls.Setup( fd, host_name, tag_tls );
            }
            Log.DebugPrint( "Did setup TLS return: %d (errorno: %d)", err, ( err == 0 ? 0 : errno ) );
        }
        else
        {
            Log.ErrorPrint( "Error getting socket" );
            return -1;
        }
    }
    else if ( modem_StriStr( transport_protocol, "UDP" ) != NULL )
    {
        fd = nrf_socket( NRF_AF_INET, NRF_SOCK_DGRAM, NRF_IPPROTO_UDP );
        Log.DebugPrint( "Opening UDP socket handle: %d", fd );
    }

    if ( fd < 0 )
    {
        Log.ErrorPrint( "Error getting socket" );
        return -1;
    }

    err = nrf_setsockopt( fd, NRF_SOL_SOCKET, NRF_SO_RCVTIMEO, &receive_timeout, sizeof( receive_timeout ) );
    if ( err != 0 )
    {
        Log.ErrorPrint( "Error setting socket options for recv timeout (error code: %d)", err );
        return -1;
    }

    err = nrf_setsockopt( fd, NRF_SOL_SOCKET, NRF_SO_SNDTIMEO, &send_timeout, sizeof( send_timeout ) );
    if ( err != 0 )
    {
        Log.ErrorPrint( "Error setting socket options for send timeout (error code: %d)", err );
        return -1;
    }

#ifdef NRFXLIB_V1
    err = nrf_connect( fd, (const nrf_sockaddr_in_t *)remote_addr, sizeof( nrf_sockaddr_in_t ) );
#else
    err = nrf_connect( fd, (const nrf_sockaddr_t *)remote_addr, sizeof( nrf_sockaddr_in_t ) );
#endif
    if ( err != 0 )
    {
        if ( remote_addr != NULL )
        {
            Log.ErrorPrint( "Error connecting to remote address: %X -> %u.%u.%u.%u, port=%d, (errorno: %d)",
                            remote_addr->sin_addr.s_addr,
                            ( remote_addr->sin_addr.s_addr ) & 0xff,
                            ( remote_addr->sin_addr.s_addr >> 8 ) & 0xff,
                            ( remote_addr->sin_addr.s_addr >> 16 ) & 0xff,
                            ( remote_addr->sin_addr.s_addr >> 24 ) & 0xff,
                            nrf_ntohs( remote_addr->sin_port ),
                            errno );
        }
        else
        {
            Log.ErrorPrint( "Remote address is null, (errorno: %d)",
                            remote_addr->sin_addr.s_addr,
                            errno );
        }
        nrf_close( fd );
        fd = -1;
    }
    else
    {
        modem_obj.socket[ fd ].fd = fd;
        modem_obj.socket[ fd ].ipaddr = remote_addr->sin_addr.s_addr;
    }

    if ( addr_info != NULL )
    {
        nrf_freeaddrinfo( addr_info );
        addr_info = NULL;
    }

    return fd;
}

int32_t modem_Disconnect( int32_t fd )
{
    int32_t err = nrf_close( fd );

    if( err < 0 )
    {
        Log.ErrorPrint( "Error closing socket: %d, errno: %d", err, errno );
    }
    else if ( fd >= 0 )
    {
        modem_obj.socket[ fd ].fd = -1;
        modem_obj.socket[ fd ].ipaddr = 0;
    }

    return err;
}

void modem_Status( void )
{
    uint32_t i;
    int32_t iarg;
    char buf[ SHORT_MSG_MAX ];
    char *arg;

    modem.ATCmd( "AT+CGMR", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 1 );
    Log.Print( "Version: %s\r\n", arg );
    modem.ATCmd( "AT+CGSN", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 1 );
    Log.Print( "IMEI: %s\r\n", arg );
    modem.ATCmd( "AT+CESQ", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 6 );
    iarg = atoi( arg );
    Log.Print( "Signal Quality: RSRQ: %d.%d dB, ", ( iarg - 39 ) >> 1, ( iarg - 39 ) % 2 == 0 ? 0 : 5 );
    arg = ( char * )embeddedCliGetToken( buf, 7 );
    iarg = atoi( arg );
    Log.Print( "RSRP: %d dBm\r\n", iarg - 140 );
    modem.ATCmd( "AT%XCBAND", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 2 );
    Log.Print( "RF Band: %s\r\n", arg );
    modem.ATCmd( "AT+COPS?", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 4 );
    Log.Print( "MCC/MNC: %s\r\n", arg );
    modem.ATCmd( "AT+CCLK?", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 2 );
    Log.Print( "Network Time: %s ", arg );
    arg = ( char * )embeddedCliGetToken( buf, 3 );
    Log.Print( "%s\r\n", arg );
    modem.ATCmd( "AT+CGDCONT?", buf );
    modem.Strip( buf, 0 );
    embeddedCliTokenizeArgs( buf );
    arg = ( char * )embeddedCliGetToken( buf, 5 );
    Log.Print( "IP Address: %s\r\n", arg );

    Log.Print( "Modem state: %s\r\n", modem.Registered() ? "registered" : "unregistered" );
    if ( modem.Registered() )
    {
        Log.Print( "Open sockets:\r\n" );
        for ( i = 0; i < ADDRESS_TABLE_ENTRY_CNT; i++ )
        {
            if ( modem_obj.socket[ i ].fd >= 0 )
            {
                Log.Print( "\tSocket: %d\tServer IP address: %u.%u.%u.%u\r\n",
                           modem_obj.socket[ i ].fd,
                           ( modem_obj.socket[ i ].ipaddr ) & 0xff,
                           ( modem_obj.socket[ i ].ipaddr >> 8 ) & 0xff,
                           ( modem_obj.socket[ i ].ipaddr >> 16 ) & 0xff,
                           ( modem_obj.socket[ i ].ipaddr >> 24 ) & 0xff );
            }
        }
    }
}

error_code_module_t modem_GetTime( uint32_t *network_time_ms )
{
    error_code_module_t err = NO_ERROR;
    char buf[ SHORT_MSG_MAX ];
    char *start;
    int8_t hr, min, sec;
    uint32_t offset;

    if ( network_time_ms == NULL )
    {
        err = ERROR_MODEM_BAD_PARAM;
    }

    if ( !modem_obj.is_init || !modem_obj.is_registered )
    {
        err = ERROR_MODEM_NOT_INIT;
    }

    if ( err == NO_ERROR )
    {
        if ( modem_obj.network_time_ms == 0 && os.TakeSemaphore( modem_obj.mutex_handle, QUEUE_WAIT_TIME ) )
        {
            offset = os.Ticks2Ms( os.GetTickCount() );
            modem_ATCmd( "AT+CCLK?", buf );

            start = modem_StriStr( buf, "," ) + 1;
            if ( ( start != NULL ) &&
                    ( start - buf < sizeof( buf ) ) )
            {
                /* start should have the time string in this format: hh:mm:ss-zz */
                hr = atoi( &start[ 0 ] );
                min = atoi( &start[ 3 ] );
                sec = atoi( &start[ 6 ] );
                modem_obj.network_time_ms = 1000 * ( 60 * ( 60 * hr + min ) + sec ) - offset;
            }

            os.GiveSemaphore( modem_obj.mutex_handle );
        }
        else
        {
            *network_time_ms = modem_obj.network_time_ms;
        }
    }

    return err;
}

int32_t modem_Receive( int32_t fd, uint8_t *buf, uint32_t size )
{
    int32_t result;
    uint32_t i = 0;

    if ( os.TakeSemaphore( modem_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        do
        {
            result = nrf_recv( fd, buf, size, NRF_MSG_DONTWAIT );
            if ( result < 0 && errno == NRF_EAGAIN )
            {
                os.Delay( 500 );
            }
            i++;
        } while ( i < RECEIVE_RETRY_MAX && result < 0 && errno == NRF_EAGAIN );

        os.GiveSemaphore( modem_obj.mutex_handle );
    }
    else
    {
        result = -1;
    }

    return result;
}

int32_t modem_Send( int32_t fd, uint8_t *buf, uint32_t size )
{
    int32_t result;

    if ( os.TakeSemaphore( modem_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        result = nrf_send( fd, buf, size, 0 );

        os.GiveSemaphore( modem_obj.mutex_handle );
    }
    else
    {
        result = -1;
    }

    return result;
}

/**
 * @} Modem
 */

/**
 * @} Driver
 */
