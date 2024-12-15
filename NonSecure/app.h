/**
 * @addtogroup Application Application
 * @{
 * @file      app.h
 * @brief     Application module (public header).
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
* @addtogroup App
* @{
*/

#ifndef __APP_H__
#define __APP_H__

/***************************************************************************************************************************
 * Includes
 */

#include "os.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

#define QUEUE_WAIT_TIME                 ( 100 )
#define QUEUE_LEN                       ( 5 )
#define QUEUE_LEN_LONG                  ( 10 )
#define SHORT_MSG_MAX                   ( 128 )
#define MEDIUM_MSG_MAX                  ( 256 )
#define LONG_MSG_MAX                    ( 1024 )
#define APP_HEAP_SIZE                   ( 8192 )
#define TASK_HIGH_PRIORITY              ( tskIDLE_PRIORITY + 1 )
#define TASK_LOW_PRIORITY               ( tskIDLE_PRIORITY )
#define MAX_TASKS                       ( 16 )
#define SHAREDMEM_SIZE                  ( 512 + APP_HEAP_SIZE )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */
typedef struct
{
    TaskHandle_t handle;
    TickType_t last_tick_update;
    TickType_t tick_timeout;
    TickType_t avg_tick_time;
} task_status_t;

typedef struct
{
    uint8_t heap[ APP_HEAP_SIZE ];
    QueueHandle_t log_queue;
    QueueHandle_t cli_queue;
    QueueHandle_t slm_queue;
    QueueHandle_t fs_queue;
    QueueHandle_t mqtt_queue;
    task_status_t task_status[ MAX_TASKS ];
    uint32_t time_led;
    uint32_t num_led;
    bool mqtt_enable;
} shared_struct_t __attribute__( ( aligned( 32 ) ) );

/**
 * Specifies the public interface functions of the watchdog monitor.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    QueueHandle_t ( *GetLogHandle )( void );
    QueueHandle_t ( *GetCliQHandle )( void );
    QueueHandle_t ( *GetSlmQHandle )( void );
    QueueHandle_t ( *GetFsQHandle )( void );
    QueueHandle_t ( *GetMqttQHandle )( void );
} const app_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern app_interface_t app;
extern uint8_t shared_mem[];

#endif /* __APP_H__ */

/**
 * @} App
 */

/**
 * @} Applicaton
 */
