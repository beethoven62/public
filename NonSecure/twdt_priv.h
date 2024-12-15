/** @file twdt_priv.h
 *
 * @brief       Task watchdog timer module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup TWDT
 * @{
 */

#ifndef __TWDT_PRIV_H__
#define __TWDT_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "twdt.h"

/***************************************************************************************************************************
 * Private constants and macros
 */


/***************************************************************************************************************************
 * Private data structures and typedefs
 */

/**
 * @brief Type of watchdog function to perform
 */
typedef enum
{
    twdttype_configure,         /*!< Register and configure task with watchdog timeout (0 = timer disabled) */
    twdttype_kickwdt,           /*!< Kick watchdog timer before timeout */
    twdttype_settimeout,        /*!< Set timeout to new time */
    twdttype_report,            /*!< Report registered tasks */
    twdttype_list,              /*!< Task list sent to CLI task */
} twdttype_t;

typedef struct
{
    twdttype_t type;
    task_status_t status;
} twdt_msg_t;

typedef struct
{
    bool                        is_init;
} twdt_obj_t ;

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
static error_code_module_t twdt_Init( void );

/**
 * @brief       Ping the watchdog monitor.
 * @return      Error code.
 */
static void twdt_Update( void );

/**
 * @brief       Register a thread as a client of the watchdog monitor.
 * @param[in]   timeout         Maximum ping timeout, in milliseconds (0 = disable).
 * @return      Error code.
 */
static void twdt_Configure( uint32_t timeout );

/**
 * @brief       Re-set the timeout for a thread.
 * @param[in]   timeout         Maximum ping timeout, in milliseconds (0 = disable).
 * @return      Error code.
 */
static void twdt_SetTimeout( uint32_t timeout );

/**
 * @brief       Print out task watchdog report.
 * @return      Error code.
 */
static void twdt_Report( void );

/**
 * @brief       Request list of registered tasks.
 * @return      Error code.
 */
static void twdt_List( void );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       Task watchdog timer thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr   Initial parameters passed into thread at start of thread.
 */
static void twdt_Thread( void * parameter_ptr );

#endif /* __TWDT_PRIV_H__ */

/**
 * @}
 */
