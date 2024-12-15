/**
 * @addtogroup Driver
 * @{
 * @file      slm.c
 * @brief     Serial LTE modem interface
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup Modem Modem module
 * @brief     Serial LTE modem task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "slm_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

slm_interface_t slm =
{
    .Init               = &slm_Init,
    .Command            = &slm_Command,
    .Bindings           = &slm_Bindings,
    .Writechar          = &slm_Writechar,
};

slm_obj_t slm_obj =
{
    .is_init            = false,
    .slm_active         = false,
    .socket             = -1,
};

/*************************************************************************************************************************************
 * Private Functions Definition
 */
void slm_Thread( void *parameter_ptr )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    slm_msg_t msg;
    char byte;
    char *buf;
    uint32_t i;
    EmbeddedCli *embedded_cli;
    CLI_UINT cli_buffer[ BYTES_TO_CLI_UINTS( SLM_BUFFER_SIZE ) ];
    CliCommandBinding binding[] =
    {
        {
            "connect",
            "connect socket (service type = TCP or UDP, URL = IP address or DNS, port number): connect TCP www.google.com 80",
            true,
            NULL,
            slm_Connect
        },
        {
            "disconnect",
            "disconnect socket (socket handle): disconnect 2",
            true,
            NULL,
            slm_Disconnect
        },
        {
            "status",
            "get report on modem state",
            false,
            NULL,
            slm_Status
        },
        {
            "credentials",
            "get credentials",
            false,
            NULL,
            slm_Credentials
        },
        {
            "start",
            "start modem",
            false,
            NULL,
            slm_Start
        },
        {
            "stop",
            "stop modem",
            false,
            NULL,
            slm_Stop
        },
        {
            "ntp",
            "ntp request (server name): ntp us.pool.ntp.org",
            true,
            NULL,
            slm_Ntpreq
        },
        {
            "http",
            "http request (server name): http www.google.com",
            true,
            NULL,
            slm_Httpreq
        },
        {
            "https",
            "https request (server name): http www.google.com",
            true,
            NULL,
            slm_Httpsreq
        },
        {
            "mqtt",
            "mqtt request (server name): http www.google.com",
            true,
            NULL,
            slm_Mqttreq
        },
        {
            "exit",
            "Exit SLM subcommand mode",
            false,
            NULL,
            slm_Exit
        },
    };

    embedded_cli = slm_Bindings( binding, sizeof( binding ) / sizeof( CliCommandBinding ), cli_buffer );
    embedded_cli->onCommand = slm_Dummy;
    embedded_cli->writeChar = slm_Writechar;

    twdt.Configure( TWDT_TIMEOUT );
    Log.InfoPrint( "SLM task started" );

    /**
     * @details
     * Logical flow:
     *      1. Deactivate CLI
     *      2. Any characters of sub-command received?
     *          2a. Characters received
     *              Process sub-command
     *          2b. No characters received:
     *              Activate SLM CLI interface:
     *      3. SLM CLI active?
     *      3a. SLM CLI active:
     *          4. Character received?
     *              4a. Character received:
     *                  Process character
     *                  Yield CPU only
     *              4b. No character received:
     *                  Delay one tick time
     *      3b. SLM CLI is not active:
     *          Reactivate CLI
     *          Delay watchdog kick time
     */
    for( ; ; )
    {
        twdt.Update();

        /* Wait for the maximum period for data to become available on the queue.
         * The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h.
         */
        if( os.QueueReceive( app.GetSlmQHandle(), &msg, os.Ms2Ticks( TWDT_KICK_TIME ) ) )
        {
            cli.Enable( false );
            if ( embeddedCliGetTokenCount( msg.msg ) > 0 )
            {
                buf = ( char *) embeddedCliGetToken( msg.msg, 1 );
                for ( i = 0; buf[ i ] != NULL && i < SHORT_MSG_MAX; i++ )
                {
                    embeddedCliReceiveChar( embedded_cli, buf[ i ] );
                }
                embeddedCliProcess( embedded_cli );
            }
            else
            {
                slm_obj.slm_active = true;
                while ( slm_obj.slm_active )
                {
                    if ( ( byte = getchar() ) != 0xff )
                    {
                        if ( Log.GetLevel() != loglevel_none )
                        {
                            slm_obj.log_level = Log.GetLevel();
                            Log.SetLevel( loglevel_none );
                        }
                        embeddedCliReceiveChar( embedded_cli, byte );

                        if ( byte == '\r' )
                        {
                            Log.SetLevel( slm_obj.log_level );
                        }

                        embeddedCliProcess( embedded_cli );
                    }
                    else
                    {
                        // Throttle task (must be set to at least 1 tick)
                        os.Delay( CLI_WAIT_TIME );
                    }
                }
            }
            cli.Enable( true );
        }
    }
}

void slm_Dummy( EmbeddedCli *embedded_cli, CliCommand *command )
{
    Log.ErrorPrint( "Unrecognized modem command." );
}

void slm_Connect( EmbeddedCli *embedded_cli, char *args, void *context )
{
    uint32_t i, parm_count;

    parm_count = embeddedCliGetTokenCount( args );
    Log.DebugPrint( "Number of args: %d", parm_count );
    for ( i = 0; i < parm_count; i++ )
    {
        Log.DebugPrint( "Parm[%d] = %s", i, embeddedCliGetToken( args, i + 1 ) );
    }

    if ( parm_count == 3 )
    {
        slm_obj.socket = modem.Connect( embeddedCliGetToken( args, 1 ), embeddedCliGetToken( args, 2 ), atoi( embeddedCliGetToken( args, 3 ) ), 5000, 5000 );
    }
    else
    {
        slm_obj.socket = -1;
    }
    if( slm_obj.socket < 0 )
    {
        Log.ErrorPrint( "Connect unsuccessful" );
    }
    else
    {
        Log.InfoPrint( "Connect successful to socket: %d", slm_obj.socket );
    }
}

void slm_Disconnect( EmbeddedCli *embedded_cli, char *args, void *context )
{
    uint32_t i, parm_count;
    int32_t socket;

    parm_count = embeddedCliGetTokenCount( args );
    Log.DebugPrint( "Number of args: %d", parm_count );
    for ( i = 0; i < parm_count; i++ )
    {
        Log.DebugPrint( "Parm[%d] = %s", i, embeddedCliGetToken( args, i + 1 ) );
    }

    if ( parm_count > 0 )
    {
        socket = atoi( embeddedCliGetToken( args, 1 ) );
    }
    else
    {
        socket = slm_obj.socket;
        slm_obj.socket = -1;
    }

    if ( modem.Disconnect( socket ) != 0 )
    {
        Log.ErrorPrint( "Disconnect unsuccessful" );
    }
    else
    {
        Log.InfoPrint( "Disconnect from socket: %d successful", socket );
    }
}

void slm_Status( EmbeddedCli *embedded_cli, char *args, void *context )
{
    modem.Status();
}

void slm_Credentials( EmbeddedCli *embedded_cli, char *args, void *context )
{
    tls.Dump();
}

void slm_Start( EmbeddedCli *embedded_cli, char *args, void *context )
{
    modem.Start();
}

void slm_Stop( EmbeddedCli *embedded_cli, char *args, void *context )
{
    modem.Stop();
}

void slm_GetRequest( const char *transport_protocol, const char *host_name, uint16_t port )
{
    int32_t result, received_bytes = 0;
    bool no_error = true;
    uint8_t *data = malloc( LONG_MSG_MAX );
    ntp_packet_t *ntp_pkt = ( ntp_packet_t *)data;

    if ( data == NULL )
    {
        Log.ErrorPrint( "Could not allocate memory for receive buffer" );
        no_error = false;
    }

    if ( no_error )
    {
        /* Make connection */
        if ( transport_protocol != NULL && host_name != NULL )
        {
            slm_obj.socket = modem.Connect( transport_protocol, host_name, port, 5000, 5000 );
        }
        else
        {
            slm_obj.socket = -1;
            no_error = false;
        }
    }

    if ( no_error )
    {
        /* Send request */
        if( slm_obj.socket < 0 )
        {
            Log.ErrorPrint( "Connect to server unsuccessful" );
            no_error = false;
        }
        else
        {
            Log.InfoPrint( "Connect to server successful" );
            if ( modem.StriStr( transport_protocol, "udp" ) != NULL )
            {
                memset( ntp_pkt, 0, NTP_PACKET_SIZE );

                // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
                data[ 0 ] = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

                Log.DebugPrint( "[NTP_CLIENT] Sending data" );
                result = nrf_send( slm_obj.socket, data, NTP_PACKET_SIZE, 0 );
            }
            else
            {
                sprintf( ( char *)data, "GET / HTTP/1.0\r\nHost:%s\r\n\r\n", host_name );
                Log.DebugPrint( "[HTTP_CLIENT] Sending data: %s", ( char * )data );
                result = nrf_send( slm_obj.socket, data, strlen( ( char * )data ), 0 );
            }
            if ( result < 0 )
            {
                Log.ErrorPrint( "[CLIENT] Error sending data (len: %d)", strlen( ( char * )data ) );
                no_error = false;
            }
        }
    }

    if ( no_error )
    {
        /* Get response */
        do
        {
            if ( modem.StriStr( transport_protocol, "udp" ) != NULL )
            {
                os.Delay( 10 );
                result = modem.Receive( slm_obj.socket, data, NTP_PACKET_SIZE );
                if ( received_bytes >= NTP_PACKET_SIZE )
                {
                    result = 0;
                    uint32_t hr, min, sec, start;
                    const uint32_t offset = 0x83aa7e80;
                    start = NRF_NTOHL( ntp_pkt->txTm_s ) - offset;
                    hr = ( start / ( 60 * 60 ) ) % 24;
                    min = ( start / 60 ) % 60;
                    sec = start % 60;
                    Log.InfoPrint( "NTP time: %02d:%02d:%02d", hr, min, sec );
                }
            }
            else
            {
                result = modem.Receive( slm_obj.socket, data, LONG_MSG_MAX );
            }
            if ( result < 0 )
            {
                Log.ErrorPrint( "[CLIENT] Error: %d", errno );
                no_error = false;
            }
            else
            {
                received_bytes += result;
                slm_Process( data, result );
                if ( Log.GetLevel() < loglevel_insane )
                {
                    os.Delay( 100 );
                }
            }
        } while( result > 0 );
        Log.InfoPrint( "Total bytes received: %d", received_bytes );

        if ( modem.Disconnect( slm_obj.socket ) != 0 )
        {
            Log.ErrorPrint( "Disconnect unsuccessful" );
        }
        else
        {
            Log.InfoPrint( "Disconnect from socket: %d successful", slm_obj.socket );
        }
    }

    if ( no_error )
    {
        Log.DebugPrint( "[CLIENT] Successful" );
    }
    else
    {
        Log.ErrorPrint( "[CLIENT] Error: %d", result );
    }

    /* Clean up */
    if ( data != NULL )
    {
        free( data );
    }
}

void slm_Ntpreq( EmbeddedCli *embedded_cli, char *args, void *context )
{
    if ( embeddedCliGetTokenCount( args ) >= 1 )
    {
        slm_GetRequest( "udp", embeddedCliGetToken( args, 1 ), 123 );
    }
    else
    {
        slm_obj.socket = -1;
    }
}

void slm_Httpreq( EmbeddedCli *embedded_cli, char *args, void *context )
{
    if ( embeddedCliGetTokenCount( args ) >= 1 )
    {
        slm_GetRequest( "tcp", embeddedCliGetToken( args, 1 ), 80 );
    }
    else
    {
        slm_obj.socket = -1;
    }
}

void slm_Httpsreq( EmbeddedCli *embedded_cli, char *args, void *context )
{
    if ( embeddedCliGetTokenCount( args ) >= 1 )
    {
        slm_GetRequest( "tls", embeddedCliGetToken( args, 1 ), 443 );
    }
    else
    {
        slm_obj.socket = -1;
    }
}

void slm_Mqttreq( EmbeddedCli *embedded_cli, char *args, void *context )
{
    if ( embeddedCliGetTokenCount( args ) >= 1 )
    {
        if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "send" ) )
        {
            mqtt.Send( 0, ( char * )embeddedCliGetToken( args, 2 ) );
        }
        else if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "unsubscribe" ) )
        {
            mqtt.Unsubscribe();
        }
        else if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "subscribe" ) )
        {
            mqtt.Subscribe( ( char * )embeddedCliGetToken( args, 2 ) );
        }
        else if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "status" ) )
        {
            mqtt.Status();
        }
        else if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "enable" ) )
        {
            blinky.EnableMQTT( true );
        }
        else if ( modem.StriStr( embeddedCliGetToken( args, 1 ), "disable" ) )
        {
            blinky.EnableMQTT( false );
        }
        else
        {
            Log.ErrorPrint( "Invalid argument" );
        }
    }
    else
    {
        slm_obj.socket = -1;
    }
}

void slm_Exit( EmbeddedCli *embedded_cli, char *args, void *context )
{
    slm_obj.slm_active = false;
}

void slm_Writechar( EmbeddedCli *embedded_cli, char c )
{
    Log.Putchar( c );
}

void slm_Process( uint8_t *data, uint32_t length)
{
    // Do nothing
}

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t slm_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( slm_obj.is_init == false )
    {
        error = NO_ERROR;

        /* Set up SLM task */
        shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
        shared_struct->slm_queue = os.CreateQueue( QUEUE_LEN, MAX_TASKS * sizeof( TaskHandle_t ) );
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( slm_Thread,
                                             "SLM",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY | portPRIVILEGE_BIT);
        MemoryRegion_t regions[] =
        {
            { ( void *)shared_mem,  SHAREDMEM_SIZE,            tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
        };
        os.AllocateRegions( handle, regions );

        /* init local RAM objects */
        slm_obj.is_init = true;
    }
    else
    {
        error = ERROR_SLM_ALREADY_INIT;
    }

    return error;
}

EmbeddedCli* slm_Bindings( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer )
{
    uint32_t i;
    EmbeddedCli *embedded_cli;
    EmbeddedCliConfig *config = embeddedCliDefaultConfig();

    config->cliBuffer = buffer;
    config->cliBufferSize = SLM_BUFFER_SIZE;
    config->rxBufferSize = SLM_RX_BUFFER_SIZE;
    config->cmdBufferSize = SLM_CMD_BUFFER_SIZE;
    config->historyBufferSize = SLM_HISTORY_SIZE;
    config->maxBindingCount = nbindings;
    config->invitation = SLM_PROMPT;
    embedded_cli = embeddedCliNew( config );

    if ( embedded_cli != NULL )
    {
        for ( i = 0; i < nbindings; i++ )
        {
            embeddedCliAddBinding( embedded_cli, binding[i] );
        }
    }

    return embedded_cli;
}

void slm_Command( char *args )
{
    uint32_t i;
    slm_msg_t cmd_msg;

    cmd_msg.handle = os.GetTaskHandle();
    memset( cmd_msg.msg, 0, sizeof( cmd_msg.msg ) );
    for ( i = 0; i < embeddedCliGetTokenCount( args ); i++ )
    {
        strcat( cmd_msg.msg, embeddedCliGetToken( args, i + 1 ) );
        strcat( cmd_msg.msg, " " );
    }
    cmd_msg.msg[ strlen( cmd_msg.msg ) - 1 ] = '\r';

    if ( !os.QueueSend( app.GetSlmQHandle(), &cmd_msg, os.Ms2Ticks( QUEUE_WAIT_TIME ) ) )
    {
        /* Message failed to send */
    }
}

/**
 * @} Modem
 */

/**
 * @} Driver
 */
