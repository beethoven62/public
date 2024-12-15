/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      os.h
 * @brief     Operating system wrapper module
 * @author    Johnas Cukier
 * @date      April 2023
 */

/**
 * @addtogroup os
 * @{
 */
#ifndef __OS_H__
#define __OS_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stream_buffer.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    bool ( *IsInsideInterrupt )( void );
    TickType_t ( *GetTickCount )( void );
    uint32_t ( *GetTickCountMs )( void );
    TickType_t ( *Ms2Ticks )( uint32_t time_ms );
    uint32_t ( *Ticks2Ms )( TickType_t ticks );
    void ( *Yield )( void );
    UBaseType_t ( *EnterCritical )( void );
    void ( *ExitCritical )( UBaseType_t interrupt_status );
    void ( *EnableInterrupts )( void );
    void ( *DisableInterrupts )( void );
    void ( *StartScheduler )( void );
    void ( *EndScheduler )( void );
    void ( *SuspendAll )( void );
    bool ( *ResumeAll )( void );
    TaskHandle_t ( *CreateTask )( TaskFunction_t task_function, char *task_name, StackType_t *task_stack, uint16_t stack_size, void *parameters, UBaseType_t priority );
    void ( *AllocateRegions )( TaskHandle_t handle, MemoryRegion_t *regions );
    void ( *DeleteTask )( TaskHandle_t handle );
    uint32_t ( *GetTaskHighWaterMark )( TaskHandle_t handle );
    void ( *DelayTicks )( TickType_t ticks );
    void ( *Delay )( uint32_t time_ms );
    void ( *Suspend )( TaskHandle_t handle );
    void ( *Resume )( TaskHandle_t handle );
    TaskHandle_t ( *GetTaskHandle )( void );
    char* ( *GetTaskName )( TaskHandle_t handle );
    QueueHandle_t ( *CreateQueue )( UBaseType_t queue_length, UBaseType_t item_size );
    void ( *DeleteQueue )( QueueHandle_t handle );
    bool ( *QueueSend )( QueueHandle_t handle, void *item, uint32_t timeout );
    bool ( *QueueReceive )( QueueHandle_t handle, void *item, uint32_t timeout );
    uint32_t ( *QueueMessagesWaiting )( QueueHandle_t handle );
    StreamBufferHandle_t ( *CreateStream )( size_t size, size_t trigger );
    void ( *DeleteStream )( StreamBufferHandle_t handle );
    size_t ( *StreamSend )( StreamBufferHandle_t handle, const void *data, size_t size, uint32_t timeout );
    size_t ( *StreamReceive )( StreamBufferHandle_t handle, void *data, size_t size, uint32_t timeout );
    bool ( *StreamEmpty )( StreamBufferHandle_t handle );
    bool ( *StreamFull )( StreamBufferHandle_t handle );
    bool ( *StreamReset )( StreamBufferHandle_t handle );
    SemaphoreHandle_t ( *CreateSemaphore )( void );
    SemaphoreHandle_t ( *CreateMutex )( void );
    void ( *DeleteSemaphore )( SemaphoreHandle_t handle );
    bool ( *TakeSemaphore )( SemaphoreHandle_t handle, uint32_t timeout );
    bool ( *GiveSemaphore )( SemaphoreHandle_t handle );
    bool ( *TaskNotifyGive )( TaskHandle_t handle );
    uint32_t ( *TaskNotifyTake )( bool clear, uint32_t timeout );
    TimerHandle_t ( *CreateTimer )( char *name, uint32_t period, bool reload, void *timer_id, TimerCallbackFunction_t callback );
    bool ( *DeleteTimer )( TimerHandle_t handle, uint32_t timeout );
    bool ( *TimerReset )( TimerHandle_t handle, uint32_t timeout );
    bool ( *TimerActive )( TimerHandle_t handle );
    bool ( *TimerStart )( TimerHandle_t handle, uint32_t timeout );
    bool ( *TimerStop )( TimerHandle_t handle, uint32_t timeout );
    uint32_t ( *TimerExpireTime )( TimerHandle_t handle );
    EventGroupHandle_t ( *CreateEvent )( void );
    void ( *DeleteEvent )( EventGroupHandle_t handle );
    EventBits_t ( *WaitForEvent )( const EventGroupHandle_t handle, const EventBits_t event_bits, bool clear, bool wait_bits, uint32_t timeout );
    EventBits_t ( *SetEvent )( EventGroupHandle_t handle, const EventBits_t event_bits );
    EventBits_t ( *ClearEvent )( EventGroupHandle_t handle, const EventBits_t event_bits );
    EventBits_t ( *GetEvent )( EventGroupHandle_t handle );
    EventBits_t ( *SyncEvent )( const EventGroupHandle_t handle, const EventBits_t event_bits_set, const EventBits_t event_bits_wait, uint32_t timeout );
} const os_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern os_interface_t os;

#endif /* __OS_H__ */

/**
 * @} os
 */

/**
 * @} Driver
 */
