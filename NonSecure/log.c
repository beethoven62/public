/**
 * @addtogroup Application Application
 * @{
 * @file      log.c
 * @brief     Log module
 * @details   Acts as a user interface and sends output to the user via serial terminal.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup Log Log output
 * @brief     Logging task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "log_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

log_interface_t Log =
{
    .Init               = &log_Init,
    .Print              = &log_Print,
    .ErrorPrint         = &log_ErrorPrint,
    .InfoPrint          = &log_InfoPrint,
    .DebugPrint         = &log_DebugPrint,
    .InsanePrint        = &log_InsanePrint,
    .GetTimestamp       = &log_GetTimestamp,
    .SetLevel           = &log_SetLevel,
    .GetLevel           = &log_GetLevel,
    .SetListType        = &log_SetListType,
    .Putchar            = &log_Putchar,
};

log_obj_t log_obj =
{
    .is_init            = false,
    .log_level          = loglevel_debug,
};

/*************************************************************************************************************************************
 * Private Functions Definition
 */

void log_Thread( void *parameter_ptr )
{
    uint32_t i;
    log_qmessage_t msg;
    TickType_t msg_time = os.Ticks2Ms( os.GetTickCount() );
    char time_buf[128];

    twdt.Configure( TWDT_TIMEOUT );
#ifdef INIT_LOG_LEVEL
    log_obj.log_level = INIT_LOG_LEVEL;
#else
    log_obj.log_level = loglevel_info;
#endif
    log_obj.task_list.type = logtask_all;
    log_GetTimestamp( msg_time, time_buf );
    printf( "[%-12s] <%s>: %s\r\n",
            os.GetTaskName( os.GetTaskHandle() ),                  // Task name
            time_buf,                                              // Timestamp
            "Log task started" );                                  // Message

    for( ; ; )
    {
        twdt.Update();

        /* Wait for the maximum period for data to become available on the queue.
         * The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h.
         */
        if( os.QueueReceive( app.GetLogHandle(), &msg, os.Ms2Ticks( TWDT_KICK_TIME ) ) )
        {
            switch ( msg.log_level )
            {
            case loglevel_char:
                putchar( msg.data[ 0 ] );
                break;

            case loglevel_force:
                printf( "%s", msg.data );
                break;

            case loglevel_task:
                log_obj.task_list.type = msg.task_list.type;
                for ( i = 0; i < LOG_MAX_LIST; i++ )
                {
                    log_obj.task_list.handle[ i ] = msg.task_list.handle[ i ];
                }
                break;

            default:
                /* xLogMessage now contains the received data. */
                msg_time = os.Ticks2Ms( msg.ticks );

                if ( log_Show( msg ) )
                {
                    log_GetTimestamp( msg_time, time_buf );
                    printf( "[%-12s] <%s>: %s\r\n",
                            os.GetTaskName( msg.handle ),               // Task name
                            time_buf,                                   // Timestamp
                            msg.data );                                 // Message
                }
                break;
            }
        }
    }
}

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t log_Init( void )
{
    uint16_t i;
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( log_obj.is_init == false )
    {
        /* Set up log task */
        shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
        shared_struct->log_queue = os.CreateQueue( QUEUE_LEN_LONG, sizeof( log_qmessage_t ) );
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( log_Thread,
                                             "Log",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_HIGH_PRIORITY | portPRIVILEGE_BIT );
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
        log_obj.is_init = true;
    }
    else
    {
        error = ERROR_LOG_ALREADY_INIT;
    }

    return error;
}

void log_Print( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    log_LevelPrint( loglevel_force, fmt, args );
    va_end( args );
}

void log_ErrorPrint( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    log_LevelPrint( loglevel_error, fmt, args );
    va_end( args );
}

void log_InfoPrint( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    log_LevelPrint( loglevel_info, fmt, args );
    va_end( args );
}

void log_DebugPrint( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    log_LevelPrint( loglevel_debug, fmt, args );
    va_end( args );
}

void log_InsanePrint( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    log_LevelPrint( loglevel_insane, fmt, args );
    va_end( args );
}

void log_LevelPrint( log_level_t log_level, const char *fmt, va_list args )
{
    log_qmessage_t log_msg;

    /* Using the stack to allocate a fixed size (128 bytes) log buffer
     * temporarily. This can be modified or perhaps dynamically allocated
     * using the FreeRTOS heap in the future.
     */
    vsnprintf( log_msg.data, SHORT_MSG_MAX, fmt, args );
    log_msg.ticks = os.GetTickCount();
    log_msg.log_level = log_level;
    log_msg.handle = os.GetTaskHandle();

    if ( !os.QueueSend( app.GetLogHandle(), &log_msg, os.Ms2Ticks( QUEUE_WAIT_TIME ) ) )
    {
        /* Message failed to send */
    }
}

void log_GetTimestamp( TickType_t tick_time, char* buf )
{
    char net_time[ 64 ];
    if ( log_GetNetTime( net_time ) )
    {
        sprintf( buf, "%s", net_time );
    }
    else
    {
        sprintf( buf, "+%d:%02d:%02d.%03d",
                 tick_time / ( 1000 * 60 * 60 ),                        // Hours
                 ( tick_time / ( 1000 * 60 ) ) % 60,                    // Minutes
                 ( tick_time / 1000 ) % 60,                             // Seconds
                 ( tick_time % 1000  ) );                               // Milliseconds
    }
}

bool log_GetNetTime( void *buf )
{
    bool result = true;
    int16_t hr, min, sec, msec;
    uint32_t network_time_ms = 0, time_now_ms;

    if ( modem.GetTime( &network_time_ms ) == NO_ERROR && network_time_ms != 0 )
    {
        time_now_ms = network_time_ms + os.Ticks2Ms( os.GetTickCount() );
        hr = ( time_now_ms / ( 1000 * 60 * 60 ) ) % 24;
        min = ( time_now_ms / ( 1000 * 60 ) ) % 60;
        sec = ( time_now_ms / 1000 ) % 60;
        msec = time_now_ms % 1000;

        sprintf( ( char *)buf, "%02d:%02d:%02d:%03d", hr, min, sec, msec );
    }
    else
    {
        result = false;
    }

    return result;
}

void log_SetListType( log_tasklist_t task_list )
{
    log_qmessage_t log_msg;

    log_msg.log_level = loglevel_task;
    log_msg.task_list = task_list;
    if ( !os.QueueSend( app.GetLogHandle(), &log_msg, os.Ms2Ticks( QUEUE_WAIT_TIME ) ) )
    {
        /* Message failed to send */
    }
}

void log_SetLevel( log_level_t log_level )
{
    log_obj.log_level = log_level;
}

log_level_t log_GetLevel( void )
{
    return log_obj.log_level;
}

bool log_Show( log_qmessage_t msg )
{
    bool flag = true;
    uint32_t i;

    if ( log_obj.task_list.type != logtask_all )
    {
        flag = log_obj.task_list.type == logtask_show ? false : true;
        for ( i = 0; log_obj.task_list.handle[ i ] != NULL && i < LOG_MAX_LIST; i++ )
        {
            if ( msg.handle ==  log_obj.task_list.handle[ i ] )
            {
                flag = !flag;
                break;
            }
        }
    }

    if ( msg.log_level > log_obj.log_level )
    {
        flag = false;
    }

    return flag;
}

void log_Putchar( char c )
{
    log_qmessage_t log_msg;

    log_msg.data[ 0 ] = c;
    log_msg.log_level = loglevel_char;
    if ( !os.QueueSend( app.GetLogHandle(), &log_msg, os.Ms2Ticks( QUEUE_WAIT_TIME ) ) )
    {
        /* Message failed to send */
    }
}

/**
 * @} Log
 */

/**
 * @} Application
 */
