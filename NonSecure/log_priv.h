/** @file log_priv.h
 *
 * @brief       Log module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup Log
 * @{
 */

#ifndef __LOG_PRIV_H__
#define __LOG_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "log.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
    log_level_t                 log_level;
    log_tasklist_t              task_list;
} log_obj_t ;

typedef struct
{
    log_level_t log_level;
    TaskHandle_t handle;
    uint32_t ticks;
    char data[ SHORT_MSG_MAX ];
    log_tasklist_t task_list;
} log_qmessage_t;

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
static error_code_module_t log_Init( void );

/**
 * @brief       Prints out raw log message.
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_Print( const char *fmt, ... );

/**
 * @brief       Prints out log message with timestamp (see definition of log_qmessage_t).
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_ErrorPrint( const char *fmt, ... );

/**
 * @brief       Prints out log message with timestamp (see definition of log_qmessage_t).
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_InfoPrint( const char *fmt, ... );

/**
 * @brief       Prints out log message with timestamp (see definition of log_qmessage_t).
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_DebugPrint( const char *fmt, ... );

/**
 * @brief       Prints out log message with timestamp (see definition of log_qmessage_t).
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_InsanePrint( const char *fmt, ... );

/**
 * @brief       Returns the timestamp as a string.
 * @details     String format: [+]hh:mm:ss.ms
 *              +: appears only if derived from ticks since reset
 *              hh: hours in 24 hour format
 *              mm: minutes
 *              ss: seconds
 *              ms: milliseconds
 * @param[in]   tick_time   Time in ticks
 * @param[out]  buf         Time as a string.
 */
static void log_GetTimestamp( TickType_t tick_time, char* buf );

/**
 * @brief       Set the log level (see log_level_t enumeration).
 * @param[in]   log_level       Log level.
 */
static void log_SetLevel( log_level_t level );

/**
 * @brief       Get the log level (see log_level_t enumeration).
 * @return      Log level.
 */
static log_level_t log_GetLevel( void );

/**
 * @brief       Sets the log task list (see definition of log_tasklist_t).
 * @param[in]   Task list
 */
static void log_SetListType( log_tasklist_t task_list );

/**
 * @brief       Prints out a character.
 * @param[in]   c Character to print
 */
static void log_Putchar( char c );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       Log output thread function. Executes and responds to requests.
 * @details     Prints out thread name, timestamp (from start up), and message in the following format:\n
 *              [thread_name] <Time hours:minutes:seconds.thousanths of seconds>: Message
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static void log_Thread( void * parameter_ptr );

/**
 * @brief       Prints out log message with timestamp (see definition of log_qmessage_t).
 * @param[in]   fmt     Log message with format (see C printf formats)
 * @param[in]   ...     Log message parameters (variable number or parameters).
 */
static void log_LevelPrint( log_level_t log_level, const char *fmt, va_list args );

/**
 * @brief       Show or hide log message from task
 * @param[in]   msg     Log message that contains the task that is logging
 * @return      True to show log for task x, False to not show log for task x
 */
static bool log_Show( log_qmessage_t msg );

/**
 * @brief       Get network time
 * @param[out]  buf     Buffer for network time string (format: HH:MM:SS:mmm)
 * @return      No error = true
 */
static bool log_GetNetTime( void *buf );

#endif /* __LOG_PRIV_H__ */

/**
 * @}
 */
