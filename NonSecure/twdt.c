/**
 * @addtogroup Application Application
 * @{
 * @file      twdt.c
 * @brief     Task watchdog timer module
 * @details   Checks health of registered tasks in the system.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup TWDT Task watchdog timer
 * @brief     Task watchdog task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/

#include "twdt_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

twdt_interface_t twdt =
{
    .Init               = &twdt_Init,
    .Update             = &twdt_Update,
    .Configure          = &twdt_Configure,
    .SetTimeout         = &twdt_SetTimeout,
    .Report             = &twdt_Report,
    .List               = &twdt_List,
};

twdt_obj_t twdt_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

void twdt_Thread( void * parameter_ptr )
{
    uint32_t i;
    TickType_t tick_now, tick_sample;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    Log.InfoPrint( "WatchDog task started" );

    for( ; ; )
    {
        /*
         * Check currently registered tasks for timeout.
         */

        // Get current time
        tick_now = os.GetTickCount();

        for ( i = 0; shared_struct->task_status[ i ].handle != NULL && i < MAX_TASKS; i++ )
        {
            if ( ( shared_struct->task_status[ i ].tick_timeout > 0 ) &&
                    ( tick_now - shared_struct->task_status[ i ].last_tick_update > shared_struct->task_status[ i ].tick_timeout ) )
            {
                // Indicate error
                Log.ErrorPrint( "Watchdog timeout: %s", os.GetTaskName( shared_struct->task_status[ i ].handle ) );
                shared_struct->task_status[ i ].last_tick_update = tick_now;
            }
        }

        os.Delay( TWDT_KICK_TIME );
    }
}

error_code_module_t twdt_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( twdt_obj.is_init == false )
    {
        /* Set up watchdog task */
        uint32_t i;
        shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( twdt_Thread,
                                             "Watchdog",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY );
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
        for ( i = 0; i < MAX_TASKS; i++ )
        {
            shared_struct->task_status[ i ].handle = NULL;
            shared_struct->task_status[ i ].avg_tick_time = 0;
        }

        twdt_obj.is_init = true;
    }
    else
    {
        error = ERROR_TWIM_ALREADY_INIT;
    }

    return error;
}

void twdt_Update( void )
{
    uint32_t i;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    TaskHandle_t handle = os.GetTaskHandle();
    TickType_t tick_now = os.GetTickCount();
    TickType_t tick_sample;

    for ( i = 0; i < MAX_TASKS; i++)
    {
        if ( shared_struct->task_status[ i ].handle == handle )
        {
            tick_sample = tick_now - shared_struct->task_status[ i ].last_tick_update;
            if ( shared_struct->task_status[ i ].avg_tick_time == 0 )
            {
                shared_struct->task_status[ i ].avg_tick_time = tick_sample;
            }
            else
            {
                shared_struct->task_status[ i ].avg_tick_time =
                    ( 7 * shared_struct->task_status[ i ].avg_tick_time + tick_sample ) >> 3;
            }
            shared_struct->task_status[ i ].last_tick_update = tick_now;
            break;
        }
    }
}

void twdt_Configure( uint32_t timeout )
{
    uint32_t i;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    // Find end of non-empty list
    for ( i = 0; shared_struct->task_status[ i ].handle != NULL && i < MAX_TASKS; i++ );

    // Create new entry
    if ( i < MAX_TASKS )
    {
        shared_struct->task_status[ i ].last_tick_update = os.GetTickCount();
        shared_struct->task_status[ i ].tick_timeout = timeout;
        shared_struct->task_status[ i ].handle = os.GetTaskHandle();
    }
}

void twdt_SetTimeout( uint32_t timeout )
{
    uint32_t i;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    TaskHandle_t handle = os.GetTaskHandle();

    for ( i = 0; i < MAX_TASKS; i++)
    {
        if ( shared_struct->task_status[ i ].handle == handle )
        {
            shared_struct->task_status[ i ].tick_timeout = timeout;
            break;
        }
    }
}

void twdt_Report( void )
{
    uint32_t i, watermark;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    // Check watchdog timer for every registered task
    Log.Print( "Task report:\r\n" );
    Log.Print( "Name\t\tAvg. Time\t%%Stack\r\n" );
    for ( i = 0; shared_struct->task_status[ i ].handle != NULL && i < MAX_TASKS; i++ )
    {
        watermark = os.GetTaskHighWaterMark( shared_struct->task_status[ i ].handle );
        Log.Print( "%-10s\t%d\t\t%d.%u\r\n",
                   os.GetTaskName( shared_struct->task_status[ i ].handle ),
                   os.Ticks2Ms( shared_struct->task_status[ i ].avg_tick_time ),
                   watermark / 10, watermark % 10 );
    }
}

void twdt_List( void )
{
    uint32_t i;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    TaskHandle_t task_handle[ MAX_TASKS ];

    memset( task_handle, NULL, sizeof( task_handle ) );
    for ( i = 0; shared_struct->task_status[ i ].handle != NULL && i < MAX_TASKS; i++ )
    {
        task_handle[ i ] = shared_struct->task_status[ i ].handle;
    }
    if ( !os.QueueSend( app.GetCliQHandle(), task_handle, os.Ms2Ticks( QUEUE_WAIT_TIME ) ) )
    {
        /* Message failed to send */
    }
}

/**
 * @} TWDT
 */

/**
 * @} Application
 */
