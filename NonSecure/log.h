/**
 * @addtogroup Application Application
 * @{
 * @file      log.h
 * @brief     Log module (public header).
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @addtogroup Log
 * @{
 */

#ifndef __LOG_H__
#define __LOG_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <nrf.h>
#include "os.h"
#include "eelcodes.h"
#include "app.h"
#include "twdt.h"
#include "modem.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

#define LOG_MAX_LIST                    ( 3 )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * @brief Show/hide logs from particular tasks only (helps filter out noise in logs)
 */
typedef enum
{
    logtask_all,            /*!< Show logs from all tasks */
    logtask_show,           /*!< Show logs from particular tasks (up to 3) */
    logtask_hide,           /*!< Hide logs from particular tasks (up to 3) */
} log_list_type_t;

typedef struct
{
    log_list_type_t             type;
    TaskHandle_t                handle[ LOG_MAX_LIST ];
} log_tasklist_t;

/**
 * @brief Log levels for various types of logs.
 */
typedef enum
{
    loglevel_char = -2,     /*!< Print single character (avoid printf overhead) */
    loglevel_force = -1,    /*!< Force log print (raw string output) */
    loglevel_none = 0,      /*!< No logs produced */
    loglevel_error,         /*!< Error logs only */
    loglevel_info,          /*!< Information and error logs */
    loglevel_debug,         /*!< Debug, information, and error logs */
    loglevel_insane,        /*!< Verbose debug, debug, information, and error logs */
    loglevel_task,          /*!< Set/get current log level */
} log_level_t;

/**
 * Specifies the public interface functions of the log task.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    void ( *Print )( const char *fmt, ... );
    void ( *ErrorPrint )( const char *fmt, ... );
    void ( *InfoPrint )( const char *fmt, ... );
    void ( *DebugPrint )( const char *fmt, ... );
    void ( *InsanePrint )( const char *fmt, ... );
    void ( *GetTimestamp )( TickType_t tick_time, char* buf );
    void ( *SetLevel )( log_level_t log_level );
    log_level_t ( *GetLevel )( void );
    void ( *SetListType )( log_tasklist_t task_list );
    void ( *Putchar )( char c );
} const log_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern log_interface_t Log;

#endif /* __LOG_H__ */

/**
 * @} Log
 */

/**
 * @} Applicaton
 */
