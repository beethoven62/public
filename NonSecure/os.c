/**
 * @addtogroup Driver
 * @{
 * @file      os.c
 * @brief     Operating system wrapper module
 * @details   Wraps FreeRTOS to comform to architecture
 * @author    Johnas Cukier
 * @date      Apr 2023
 */

/**
 * @defgroup os Operating system wrapper
 * @brief    RTOS wrapper
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "os_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

os_interface_t os =
{
    .Init                   = &os_Init,
    .IsInsideInterrupt      = &os_IsInsideInterrupt,
    .Ms2Ticks               = &os_Ms2Ticks,
    .Ticks2Ms               = &os_Ticks2Ms,
    .GetTickCount           = &os_GetTickCount,
    .GetTickCountMs         = &os_GetTickCountMs,
    .Yield                  = &os_Yield,
    .EnterCritical          = &os_EnterCritical,
    .ExitCritical           = &os_ExitCritical,
    .EnableInterrupts       = &os_EnableInterrupts,
    .DisableInterrupts      = &os_DisableInterrupts,
    .StartScheduler         = &os_StartScheduler,
    .EndScheduler           = &os_EndScheduler,
    .SuspendAll             = &os_SuspendAll,
    .ResumeAll              = &os_ResumeAll,
    .CreateTask             = &os_CreateTask,
    .AllocateRegions        = &os_AllocateRegions,
    .DeleteTask             = &os_DeleteTask,
    .GetTaskHighWaterMark   = &os_GetTaskHighWaterMark,
    .DelayTicks             = &os_DelayTicks,
    .Delay                  = &os_Delay,
    .Suspend                = &os_Suspend,
    .GetTaskHandle          = &os_GetTaskHandle,
    .GetTaskName            = &os_GetTaskName,
    .CreateQueue            = &os_CreateQueue,
    .DeleteQueue            = &os_DeleteQueue,
    .QueueSend              = &os_QueueSend,
    .QueueReceive           = &os_QueueReceive,
    .QueueMessagesWaiting   = &os_QueueMessagesWaiting,
    .CreateStream           = &os_CreateStream,
    .DeleteStream           = &os_DeleteStream,
    .StreamSend             = &os_StreamSend,
    .StreamReceive          = &os_StreamReceive,
    .StreamEmpty            = &os_StreamEmpty,
    .StreamFull             = &os_StreamFull,
    .StreamReset            = &os_StreamReset,
    .CreateSemaphore        = &os_CreateSemaphore,
    .CreateMutex            = &os_CreateMutex,
    .DeleteSemaphore        = &os_DeleteSemaphore,
    .TakeSemaphore          = &os_TakeSemaphore,
    .GiveSemaphore          = &os_GiveSemaphore,
    .TaskNotifyGive         = &os_TaskNotifyGive,
    .TaskNotifyTake         = &os_TaskNotifyTake,
    .CreateTimer            = &os_CreateTimer,
    .DeleteTimer            = &os_DeleteTimer,
    .TimerReset             = &os_TimerReset,
    .TimerActive            = &os_TimerActive,
    .TimerStart             = &os_TimerStart,
    .TimerStop              = &os_TimerStop,
    .TimerExpireTime        = &os_TimerExpireTime,
    .CreateEvent            = &os_CreateEvent,
    .DeleteEvent            = &os_DeleteEvent,
    .WaitForEvent           = &os_WaitForEvent,
    .SetEvent               = &os_SetEvent,
    .ClearEvent             = &os_ClearEvent,
    .GetEvent               = &os_GetEvent,
    .SyncEvent              = &os_SyncEvent,
};

os_obj_t os_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Private Functions Definition
 */

/*************************************************************************************************************************************
* Public Functions Definition
*/

error_code_module_t os_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( os_obj.is_init == false )
    {
        error = NO_ERROR;
        vTaskStartScheduler();

        /* init local RAM objects */
        os_obj.is_init = true;
    }
    else
    {
        error = ERROR_OS_ALREADY_INIT;
    }

    return error;
}

bool os_IsInsideInterrupt( void )
{
    return xPortIsInsideInterrupt();
}

TickType_t os_GetTickCount( void )
{
    TickType_t ticks;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        ticks = xTaskGetTickCountFromISR();
    }
    else                                                // In normal context
    {
        ticks = xTaskGetTickCount();
    }

    return ticks;
}

uint32_t os_GetTickCountMs( void )
{
    return os_Ticks2Ms( os_GetTickCount() );
}

TickType_t os_Ms2Ticks( uint32_t time_ms )
{
    return pdMS_TO_TICKS( time_ms );
}

uint32_t os_Ticks2Ms( TickType_t ticks )
{
    return ( uint32_t )( ( uint64_t )( ( TickType_t ) ticks * ( TickType_t ) 1000U ) / ( TickType_t ) configTICK_RATE_HZ );
}

void os_Yield( void )
{
    taskYIELD();
}

UBaseType_t os_EnterCritical( void )
{
    UBaseType_t interrupt_status = 0;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        interrupt_status = taskENTER_CRITICAL_FROM_ISR();
    }
    else                                                // In normal context
    {
        taskENTER_CRITICAL();
    }

    return interrupt_status;
}

void os_ExitCritical( UBaseType_t interrupt_status )
{
    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        taskEXIT_CRITICAL_FROM_ISR( interrupt_status );
    }
    else                                                // In normal context
    {
        taskEXIT_CRITICAL();
    }
}

void os_EnableInterrupts( void )
{
    taskENABLE_INTERRUPTS();
}

void os_DisableInterrupts( void )
{
    taskDISABLE_INTERRUPTS();
}

void os_StartScheduler( void )
{
    vTaskStartScheduler();
}

void os_EndScheduler( void )
{
    vTaskEndScheduler();
}

void os_SuspendAll( void )
{
    vTaskSuspendAll();
}

bool os_ResumeAll( void )
{
    return xTaskResumeAll() == pdPASS;
}

TaskHandle_t os_CreateTask( TaskFunction_t task_function, char *task_name, StackType_t *task_stack, uint16_t stack_size, void *parameters, UBaseType_t priority )
{
    TaskHandle_t handle;
    TaskParameters_t task_parameters =
    {
        .pvTaskCode     = task_function,
        .pcName         = task_name,
        .usStackDepth   = stack_size,
        .pvParameters   = parameters,
        .uxPriority     = priority,
        .puxStackBuffer = task_stack,
        .xRegions       =
        {
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
            { 0,                    0,                         0                                                      },
        }
    };

#if configENABLE_MPU == 1
    /* Create an unprivileged task. */
    if ( xTaskCreateRestricted(  &task_parameters, &handle ) == pdPASS )
    {
        return handle;
    }
#else
    /* MPU disabled */
    if ( xTaskCreate( task_parameters.pvTaskCode,
                      task_parameters.pcName,
                      task_parameters.usStackDepth,
                      task_parameters.pvParameters,
                      task_parameters.uxPriority,
                      handle ) == pdPASS )
    {
        return handle;
    }
#endif
    else
    {
        return ( TaskHandle_t ) pdFAIL;
    }
}

void os_AllocateRegions( TaskHandle_t handle, MemoryRegion_t *regions )
{
#if configENABLE_MPU == 1
    vTaskAllocateMPURegions( handle, regions );
#endif
}

void os_DeleteTask( TaskHandle_t handle )
{
    vTaskDelete( handle );
}

uint32_t os_GetTaskHighWaterMark( TaskHandle_t handle )
{
    TaskStatus_t status;
    uint32_t high_watermark;

    high_watermark = uxTaskGetStackHighWaterMark( handle );

    return ( configMINIMAL_STACK_SIZE - high_watermark ) * 1000 / configMINIMAL_STACK_SIZE;
}

void os_DelayTicks( TickType_t ticks )
{
    vTaskDelay( ticks );
}

void os_Delay( uint32_t time )
{
    vTaskDelay( os_Ms2Ticks( time ) == 0 ? 1 : os_Ms2Ticks( time ) );
}

void os_Suspend( TaskHandle_t handle )
{
    vTaskSuspend( handle );
}

void os_Resume( TaskHandle_t handle )
{
    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        xTaskResumeFromISR( handle );
    }
    else                                                // In normal context
    {
        vTaskResume( handle );
    }
}

TaskHandle_t os_GetTaskHandle( void )
{
    return xTaskGetCurrentTaskHandle();
}

char* os_GetTaskName( TaskHandle_t handle )
{
    return pcTaskGetName( handle );
}

QueueHandle_t os_CreateQueue( UBaseType_t queue_length, UBaseType_t item_size )
{
    return xQueueCreate( queue_length, item_size );
}

void os_DeleteQueue( QueueHandle_t handle )
{
    vQueueDelete( handle );
}

bool os_QueueSend( QueueHandle_t handle, void *item, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xQueueSendFromISR( handle, item, &high_priority_task ) == pdPASS;
        if ( high_priority_task )
        {
            portYIELD_FROM_ISR( high_priority_task );
        }
    }
    else                                                // In normal context
    {
        result = xQueueSend( handle, item, os_Ms2Ticks( timeout ) ) == pdPASS;
    }

    return result;
}

bool os_QueueReceive( QueueHandle_t handle, void *item, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xQueueReceiveFromISR( handle, item, &high_priority_task ) == pdPASS;
        if ( high_priority_task )
        {
            portYIELD_FROM_ISR( high_priority_task );
        }
    }
    else                                                // In normal context
    {
        result = xQueueReceive( handle, item, os_Ms2Ticks( timeout ) ) == pdPASS;
    }

    return result;
}

uint32_t os_QueueMessagesWaiting( QueueHandle_t handle )
{
    uint32_t result;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = uxQueueMessagesWaitingFromISR( handle );
    }
    else                                                // In normal context
    {
        result = uxQueueMessagesWaiting( handle );
    }

    return result;
}

StreamBufferHandle_t os_CreateStream( size_t size, size_t trigger )
{
    return xStreamBufferCreate( size, trigger );
}

void os_DeleteStream( StreamBufferHandle_t handle )
{
    vStreamBufferDelete( handle );
}

size_t os_StreamSend( StreamBufferHandle_t handle, const void *data, size_t size, uint32_t timeout )
{
    size_t result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )
    {
        result = xStreamBufferSendFromISR( handle, data, size, &high_priority_task );
        portYIELD_FROM_ISR( high_priority_task );
    }
    else
    {
        result = xStreamBufferSend( handle, data, size, os_Ms2Ticks( timeout ) );
    }

    return result;
}

size_t os_StreamReceive( StreamBufferHandle_t handle, void *data, size_t size, uint32_t timeout )
{
    size_t result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )
    {
        result = xStreamBufferReceiveFromISR( handle, data, size, &high_priority_task );
        portYIELD_FROM_ISR( high_priority_task );
    }
    else
    {
        result = xStreamBufferReceive( handle, data, size, os_Ms2Ticks( timeout ) );
    }

    return result;
}

bool os_StreamEmpty( StreamBufferHandle_t handle )
{
    return xStreamBufferIsEmpty( handle ) == pdTRUE;
}

bool os_StreamFull( StreamBufferHandle_t handle )
{
    return xStreamBufferIsFull( handle ) == pdTRUE;
}

bool os_StreamReset( StreamBufferHandle_t handle )
{
    return xStreamBufferReset( handle ) == pdTRUE;
}

SemaphoreHandle_t os_CreateSemaphore( void )
{
    return xSemaphoreCreateBinary();
}

SemaphoreHandle_t os_CreateMutex( void )
{
    return xSemaphoreCreateMutex();
}

void os_DeleteSemaphore( SemaphoreHandle_t handle )
{
    vSemaphoreDelete( handle );
}

bool os_TakeSemaphore( SemaphoreHandle_t handle, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xSemaphoreTakeFromISR( handle, &high_priority_task ) == pdPASS;
        if ( high_priority_task )
        {
            portYIELD_FROM_ISR( high_priority_task );
        }
    }
    else                                                // In normal context
    {
        result = xSemaphoreTake( handle, os_Ms2Ticks( timeout ) ) == pdPASS;
    }

    return result;
}

bool os_GiveSemaphore( SemaphoreHandle_t handle )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xSemaphoreGiveFromISR( handle, &high_priority_task ) == pdPASS;
        if ( high_priority_task )
        {
            portYIELD_FROM_ISR( high_priority_task );
        }
    }
    else                                                // In normal context
    {
        result = xSemaphoreGive( handle ) == pdPASS;
    }

    return result;
}

bool os_TaskNotifyGive( TaskHandle_t handle )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        vTaskNotifyGiveFromISR( handle, &high_priority_task );
        portYIELD_FROM_ISR( high_priority_task );
        result = true;
    }
    else                                                // In normal context
    {
        result = ( xTaskNotifyGive( handle ) == pdPASS );
    }

    return result;
}

uint32_t os_TaskNotifyTake( bool clear, uint32_t timeout )
{
    return ulTaskNotifyTake( clear ? pdTRUE : pdFALSE, os_Ms2Ticks( timeout ) );
}

TimerHandle_t os_CreateTimer( char *name, uint32_t period, bool reload, void *timer_id, TimerCallbackFunction_t callback )
{
    return xTimerCreate( name, os_Ms2Ticks( period ), reload, timer_id, callback );
}

bool os_DeleteTimer( TimerHandle_t handle, uint32_t timeout )
{
    return ( xTimerDelete( handle, os_Ms2Ticks( timeout ) ) == pdPASS );
}

bool os_TimerReset( TimerHandle_t handle, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = ( xTimerResetFromISR( handle, &high_priority_task ) == pdPASS );
    }
    else                                                // In normal context
    {
        result = ( xTimerReset( handle, os_Ms2Ticks( timeout ) ) == pdPASS );
    }

    return result;
}

bool os_TimerActive( TimerHandle_t handle )
{
    return ( xTimerIsTimerActive( handle ) == pdTRUE );
}

bool os_TimerStart( TimerHandle_t handle, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = ( xTimerStartFromISR( handle, &high_priority_task ) == pdPASS );
    }
    else                                                // In normal context
    {
        result = ( xTimerStart( handle, os_Ms2Ticks( timeout ) ) == pdPASS );
    }

    return result;
}

bool os_TimerStop( TimerHandle_t handle, uint32_t timeout )
{
    bool result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = ( xTimerStopFromISR( handle, &high_priority_task ) == pdPASS );
    }
    else
    {
        result = ( xTimerStop( handle, os_Ms2Ticks( timeout ) ) == pdPASS );
    }

    return result;
}

uint32_t os_TimerExpireTime( TimerHandle_t handle )
{
    return os_Ticks2Ms( xTimerGetExpiryTime( handle ) );
}

EventGroupHandle_t os_CreateEvent( void )
{
    return xEventGroupCreate();
}

void os_DeleteEvent( EventGroupHandle_t handle )
{
    vEventGroupDelete( handle );
}

EventBits_t os_WaitForEvent( const EventGroupHandle_t handle, const EventBits_t event_bits, bool clear, bool wait_bits, uint32_t timeout )
{
    return xEventGroupWaitBits( handle, event_bits, clear, wait_bits, os_Ms2Ticks( timeout ) );
}

EventBits_t os_SetEvent( EventGroupHandle_t handle, const EventBits_t event_bits )
{
    EventBits_t result;
    BaseType_t high_priority_task = pdFALSE;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xEventGroupSetBitsFromISR( handle, event_bits, &high_priority_task );
    }
    else
    {
        result = xEventGroupSetBits( handle, event_bits );
    }

    return result;
}

EventBits_t os_ClearEvent( EventGroupHandle_t handle, const EventBits_t event_bits )
{
    EventBits_t result;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xEventGroupClearBitsFromISR( handle, event_bits );
    }
    else
    {
        result = xEventGroupSetBits( handle, event_bits );
    }

    return result;
}

EventBits_t os_GetEvent( EventGroupHandle_t handle )
{
    EventBits_t result;

    if ( xPortIsInsideInterrupt() )                     // In interrupt context
    {
        result = xEventGroupGetBitsFromISR( handle );
    }
    else
    {
        result = xEventGroupGetBits( handle );
    }

    return result;
}

EventBits_t os_SyncEvent( const EventGroupHandle_t handle, const EventBits_t event_bits_set, const EventBits_t event_bits_wait, uint32_t timeout )
{
    return xEventGroupSync( handle, event_bits_set, event_bits_wait, os_Ms2Ticks( timeout ) );
}

/**
 * @} os
 */

/**
 * @} Driver
 */
