/**
 * @addtogroup Application Application
 * @{
 * @file      app.c
 * @brief     Application module
 * @details   Starts all other tasks and sets up shared memory used by tasks under MPU control.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
* @defgroup App Application module
* @brief     Application module that creates all tasks in the system
* @{
*/

/*************************************************************************************************************************************
 * Includes
*/
#include "app_priv.h"
#include "eelcodes.h"
#include "board.h"
#include "dmm.h"
#include "log.h"
#include "twdt.h"
#include "cli.h"
#include "modem.h"
#include "tls.h"
#include "slm.h"
#include "mqtt.h"
#include "fs.h"
#include "blinky.h"
#if NRFX_SPIM_ENABLED
#include "spimtest.h"
#endif
#if NRFX_SPIS_ENABLED
#include "spistest.h"
#endif

/***************************************************************************************************************************
 * Global variables
 */

app_interface_t app =
{
    .Init               = &app_Init,
    .GetLogHandle       = &app_GetLogHandle,
    .GetCliQHandle      = &app_GetCliQHandle,
    .GetSlmQHandle      = &app_GetSlmQHandle,
    .GetFsQHandle       = &app_GetFsQHandle,
    .GetMqttQHandle     = &app_GetMqttQHandle,
};

app_obj_t app_obj =
{
    .is_init            = false,
};

uint8_t shared_mem[ SHAREDMEM_SIZE ] __attribute__( ( aligned( 32 ) ) );

/*************************************************************************************************************************************
 * Public Functions Definition
 */

void app_Thread( void * parameter_ptr )
{
    error_code_module_t error;

    twdt.Configure( TWDT_TIMEOUT );
    Log.InfoPrint( "Application task started" );

    if ( ( error = cli.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    if ( ( error = blinky.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    if ( ( error = modem.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    if ( ( error = tls.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    if ( ( error = slm.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    if ( ( error = mqtt.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

#if NRFX_SPIM_ENABLED
    if ( ( error = spimtest.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }
#endif

#if NRFX_SPIS_ENABLED
    if ( ( error = spistest.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }
#endif

    if ( ( error = fs.Init() ) != NO_ERROR )
    {
        Log.ErrorPrint( "Error: 0x%04x", error );
    }

    modem.Start();

    for( ; ; )
    {
        twdt.Update();
        os.Delay( TWDT_KICK_TIME );
    }
}

error_code_module_t app_Init( void )
{
    error_code_module_t error = NO_ERROR;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    /* Singleton pattern */
    if ( app_obj.is_init == false )
    {
        /* Make sure the heap, board, logging and watchdog tasks are initialized first */
        if ( ( error = dmm.Init( dmm_handle_0, shared_struct->heap, APP_HEAP_SIZE ) != NO_ERROR ) )
        {
            printf( "Error: 0x%04x\r\n", error );
        }

        if ( ( error = board.Init() ) != NO_ERROR )
        {
            printf( "Error: 0x%04x\r\n", error );
        }

        if ( ( error = twdt.Init() ) != NO_ERROR )
        {
            printf( "Error: 0x%04x\r\n", error );
        }

        if ( ( error = Log.Init() ) != NO_ERROR )
        {
            printf( "Error: 0x%04x\r\n", error );
        }

        error = NO_ERROR;

        /* Set up application task */
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( app_Thread,
                                             "App",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY | portPRIVILEGE_BIT );
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
        app_obj.is_init = true;
    }
    else
    {
        error = ERROR_APP_ALREADY_INIT;
    }

    return error;
}

QueueHandle_t app_GetLogHandle( void )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    return shared_struct->log_queue;
}

QueueHandle_t app_GetCliQHandle( void )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    return shared_struct->cli_queue;
}

QueueHandle_t app_GetSlmQHandle( void )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    return shared_struct->slm_queue;
}

QueueHandle_t app_GetFsQHandle( void )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    return shared_struct->fs_queue;
}

QueueHandle_t app_GetMqttQHandle( void )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    return shared_struct->mqtt_queue;
}

/**
 * @} App
 */

/**
 * @} Applicaton
 */
