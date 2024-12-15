/** @file os_priv.h
 *
 * @brief       Operating system wrapper module (private header).
 * @author      Johnas Cukier
 * @date        April 2023
 *
 */

/**
 * @addtogroup os
 * @{
 */

#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "os.h"

/***************************************************************************************************************************
* Private constants and macros
*/

/***************************************************************************************************************************
* Private data structures and typedefs
*/

typedef struct
{
    bool                        is_init;
} os_obj_t;

/***************************************************************************************************************************
* Private variables
*/

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @return      Error code.
 */
static error_code_module_t os_Init( void );

/**
 * @brief       Get the tick count since reset.
 * @return      True = inside interrupt.
 */
static bool os_IsInsideInterrupt( void );

/**
 * @brief       Get the tick count since reset.
 * @return      Tick count.
 */
static TickType_t os_GetTickCount( void );

/**
 * @brief       Get the tick count in milliseconds since reset.
 * @return      Tick count in milliseconds.
 */
static uint32_t os_GetTickCountMs( void );

/**
 * @brief       Convert milliseconds to ticks.
 * @param[in]   time_ms     Milliseconds
 * @return      Ticks.
 */
static TickType_t os_Ms2Ticks( uint32_t time_ms );

/**
 * @brief       Convert ticks to milliseconds.
 * @param[in]   ticks       Ticks
 * @return      Milliseconds.
 */
static uint32_t os_Ticks2Ms( TickType_t ticks );

/**
 * @brief       Yield to OS.
 */
static void os_Yield( void );

/**
 * @brief       Enter critical section.
 * @return      Interrupt status (if called inside ISR).
 */
static UBaseType_t os_EnterCritical( void );

/**
 * @brief       Exit critical section.
 * @param[in]   interrupt_status    Interrupt status (if called inside ISR)
 */
static void os_ExitCritical( UBaseType_t interrupt_status );

/**
 * @brief       Enable interrupts.
 */
static void os_EnableInterrupts( void );

/**
 * @brief       Disable interrupts.
 */
static void os_DisableInterrupts( void );

/**
 * @brief       Start OS scheduler.
 */
static void os_StartScheduler( void );

/**
 * @brief       Stop OS scheduler.
 */
static void os_EndScheduler( void );

/**
 * @brief       Suspend all tasks.
 */
static void os_SuspendAll( void );

/**
 * @brief       Resume all tasks.
 * @return      Operation successful.
 */
static bool os_ResumeAll( void );

/**
 * @brief       Create new task.
 * @param[in]   task_function       Pointer to task thread function
 * @param[in]   task_name           Task name string
 * @param[in]   task_stack          Pointer to task stack memory block
 * @param[in]   stack_size          Task stack size
 * @param[in]   parameters          Pointer to parameters to pass into task thread
 * @param[in]   priority            Task priority
 * @return      Task handle.
 */
static TaskHandle_t os_CreateTask( TaskFunction_t task_function, char *task_name, StackType_t *task_stack, uint16_t stack_size, void *parameters, UBaseType_t priority );

/**
 * @brief       Allocate MPU memory regions.
 * @param[in]   handle              Task handle
 * @param[in]   regions             Pointer to memory regions
 */
static void os_AllocateRegions( TaskHandle_t handle, MemoryRegion_t *regions );

/**
 * @brief       Delete task.
 * @param[in]   handle              Task handle
 */
static void os_DeleteTask( TaskHandle_t handle );

/**
 * @brief       Get task stack high watermark.
 * @param[in]   handle              Task handle
 * @return      Task stack high watermark in 0.1% (100.0% = 1000)
 */
static uint32_t os_GetTaskHighWaterMark( TaskHandle_t handle );

/**
 * @brief       Delay (yield to OS).
 * @param[in]   ticks               Number of ticks to delay
 */
static void os_DelayTicks( TickType_t ticks );

/**
 * @brief       Delay (yield to OS).
 * @param[in]   time                Number of milliseconds to delay
 */
static void os_Delay( uint32_t time );

/**
 * @brief       Suspend task.
 * @param[in]   handle              Task handle of task to suspend
 */
static void os_Suspend( TaskHandle_t handle );

/**
 * @brief       Resume task.
 * @param[in]   handle              Task handle of task to resume
 */
static void os_Resume( TaskHandle_t handle );

/**
 * @brief       Get handle of currently running task.
 * @return      Task handle.
 */
static TaskHandle_t os_GetTaskHandle( void );

/**
 * @brief       Get task name.
 * @param[in]   handle              Task handle
 * @return      Task name string.
 */
static char* os_GetTaskName( TaskHandle_t handle );

/**
 * @brief       Create new queue.
 * @param[in]   queue_length        Maximum number of items in queue
 * @param[in]   item_size           Size of each item (in bytes)
 * @return      Queue handle.
 */
static QueueHandle_t os_CreateQueue( UBaseType_t queue_length, UBaseType_t item_size );

/**
 * @brief       Delete queue.
 * @param[in]   handle              Queue handle
 */
static void os_DeleteQueue( QueueHandle_t handle );

/**
 * @brief       Send message to queue.
 * @param[in]   handle              Queue handle
 * @param[in]   item                Pointer to message item to send
 * @param[in]   timeout             Maximum wait time (ms)
 * @return      Success.
 */
static bool os_QueueSend( QueueHandle_t handle, void *item, uint32_t timeout );

/**
 * @brief       Receive message from queue.
 * @param[in]   handle              Queue handle
 * @param[out]  item                Pointer to buffer to receive message item
 * *param[in]   timeout             Maximum wait time (ms)
 * @return      Success.
 */
static bool os_QueueReceive( QueueHandle_t handle, void *item, uint32_t timeout );

/**
 * @brief       Query number of messages waiting in queue.
 * @param[in]   handle              Queue handle
 * @return      Number of messages currently waiting in the queue.
 */
static uint32_t os_QueueMessagesWaiting( QueueHandle_t handle );

/**
 * @brief       Create stream buffer.
 * @param[in]   size                Size of stream buffer
 * @param[in]   trigger             Trigger level to release
 * @return      Stream buffer handle.
 */
static StreamBufferHandle_t os_CreateStream( size_t size, size_t trigger );

/**
 * @brief       Delete stream buffer.
 * @param[in]   Stream buffer handle.
 */
static void os_DeleteStream( StreamBufferHandle_t handle );

/**
 * @brief       Send data to stream buffer.
 * @param[in]   handle              Stream buffer handle
 * @param[in]   data                Pointer to data to send
 * @param[in]   size                Size of data (in bytes)
 * @param[in]   timeout             Maximum wait time (ms)
 * @return      Number of bytes actually sent.
 */
static size_t os_StreamSend( StreamBufferHandle_t handle, const void *data, size_t size, uint32_t timeout );

/**
 * @brief       Receive data from stream buffer.
 * @param[in]   handle              Stream buffer handle
 * @param[in]   data                Pointer to buffer for data received
 * @param[in]   size                Maximum buffer size (in bytes)
 * @param[in]   timeout             Maximum wait time (ms)
 * @return      Number of bytes actually received.
 */
static size_t os_StreamReceive( StreamBufferHandle_t handle, void *data, size_t size, uint32_t timeout );

/**
 * @brief       Is stream buffer empty.
 * @param[in]   handle              Stream buffer handle
 * @return      Empty.
 */
static bool os_StreamEmpty( StreamBufferHandle_t handle );

/**
 * @brief       Is stream buffer full.
 * @param[in]   handle              Stream buffer handle
 * @return      Full.
 */
static bool os_StreamFull( StreamBufferHandle_t handle );

/**
 * @brief       Reset stream buffer.
 * @param[in]   handle              Stream buffer handle
 * @return      Success.
 */
static bool os_StreamReset( StreamBufferHandle_t handle );

/**
 * @brief       Create new binary semaphore.
 * @return      Semaphore handle.
 */
static SemaphoreHandle_t os_CreateSemaphore( void );

/**
 * @brief       Create new mutex.
 * @return      Semaphore handle.
 */
static SemaphoreHandle_t os_CreateMutex( void );

/**
 * @brief       Delete binary semaphore.
 * @param[in]   handle              Semaphore handle
 */
static void os_DeleteSemaphore( SemaphoreHandle_t handle );

/**
 * @brief       Take semaphore (reserve).
 * @param[in]   handle              Semaphore handle
 * @param[in]   timeout             Maximum wait time (ms)
 * @return      Success.
 */
static bool os_TakeSemaphore( SemaphoreHandle_t handle, uint32_t timeout );

/**
 * @brief       Give semaphore (release).
 * @param[in]   handle              Semaphore handle
 * @return      Success.
 */
static bool os_GiveSemaphore( SemaphoreHandle_t handle );

/**
 * @brief       Notify task when counting semaphore taken.
 * @param[in]   handle              Task handle
 * @return      Success.
 */
static bool os_TaskNotifyGive( TaskHandle_t handle );

/**
 * @brief       Notify task when counting semaphore given.
 * @param[in]   clear               Clear semaphore count
 * @param[in]   timeout             Maximum time to wait (ms)
 * @return      Success.
 */
static uint32_t os_TaskNotifyTake( bool clear, uint32_t timeout );

/**
 * @brief       Create new timer.
 * @param[in]   name                Timer name string
 * @param[in]   period              Timeout period (ms)
 * @param[in]   reload              Recurring
 * @param[out]  timer_id            Pointer to timer ID
 * @param[in]   callback            Pointer to callback function on timeout
 * @return      Timer handle.
 */
static TimerHandle_t os_CreateTimer( char *name, uint32_t period, bool reload, void *timer_id, TimerCallbackFunction_t callback );

/**
 * @brief       Delete timer.
 * @param[in]   handle              Timer handle
 * @param[in]   timeout             Time (ms) to block before completing operation
 * @return      Success
 */
static bool os_DeleteTimer( TimerHandle_t handle, uint32_t timeout );

/**
 * @brief       Reset timer.
 * @param[in]   handle              Timer handle
 * @param[in]   timeout             Time (ms) to block before completing operation
 * @return      Success
 */
static bool os_TimerReset( TimerHandle_t handle, uint32_t timeout );

/**
 * @brief       Is timer active?
 * @param[in]   handle              Timer handle
 * @return      Active
 */
static bool os_TimerActive( TimerHandle_t handle );

/**
 * @brief       Start timer.
 * @param[in]   handle              Timer handle
 * @param[in]   timeout             Time (ms) to block before completing operation
 * @return      Success
 */
static bool os_TimerStart( TimerHandle_t handle, uint32_t timeout );

/**
 * @brief       Stop timer.
 * @param[in]   handle              Timer handle
 * @param[in]   timeout             Time (ms) to block before completing operation
 * @return      Success
 */
static bool os_TimerStop( TimerHandle_t handle, uint32_t timeout );

/**
 * @brief       Get expiration time.
 * @param[in]   handle              Timer handle
 * @return      Number of ms until expiration of timer
 */
static uint32_t os_TimerExpireTime( TimerHandle_t handle );

/**
 * @brief       Create new event.
 * @return      Event handle
 */
static EventGroupHandle_t os_CreateEvent( void );

/**
 * @brief       Delete event.
 * @param[in]   handle              Timer handle
 */
static void os_DeleteEvent( EventGroupHandle_t handle );

/**
 * @brief       Wait for event.
 * @param[in]   handle              Timer handle
 * @param[in]   event_bits          Event bits to wait for
 * @param[in[   clear               Clear event bits set
 * @param[in]   wait_bits           Wait for any (false) or all (true) bits to be set
 * @param[in]   timeout             Maximum time to wait (ms)
 * @return      Actual gits set in event group
 */
static EventBits_t os_WaitForEvent( const EventGroupHandle_t handle, const EventBits_t event_bits, bool clear, bool wait_bits, uint32_t timeout );

/**
 * @brief       Set event bits.
 * @param[in]   handle              Event handle
 * @param[in]   event_bits          Bits to set
 * @return      Actual bits set in event group
 */
static EventBits_t os_SetEvent( EventGroupHandle_t handle, const EventBits_t event_bits );

/**
 * @brief       Clear event bits.
 * @param[in]   handle              Timer handle
 * @param[in]   event_bits          Bits to clear
 * @return      Previous state of bits in event group
 */
static EventBits_t os_ClearEvent( EventGroupHandle_t handle, const EventBits_t event_bits );

/**
 * @brief       Get event bits.
 * @param[in]   handle              Timer handle
 * @return      State of bits in event group
 */
static EventBits_t os_GetEvent( EventGroupHandle_t handle );

/**
 * @brief       Sync event bits.
 * @param[in]   handle              Timer handle
 * @param[in]   event_bits_set      Bits to set
 * @param[in]   event_bits_wait     Bits to wait for
 * @param[in]   timeout             Maximum time to wait (ms)
 * @return      Actual bits set
 */
static EventBits_t os_SyncEvent( const EventGroupHandle_t handle, const EventBits_t event_bits_set, const EventBits_t event_bits_wait, uint32_t timeout );

/***************************************************************************************************************************
 * Private prototypes
 */

#endif /* __OS_PRIV_H__ */

/**
 * @} os
 */

/**
 * @} Driver
 */
