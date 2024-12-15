/**
 * @addtogroup Application Application
 * @{
 *
 * @file      twdt.h
 * @brief     Task watchdog timer module (public header).
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup TWDT
 * @{
 */
#ifndef __TWDT_H__
#define __TWDT_H__

/***************************************************************************************************************************
 * Includes
 */
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "eelcodes.h"
#include "app.h"
#include "log.h"

/***************************************************************************************************************************
 * Public constants and macros
 */
#define TWDT_TIMEOUT                    ( 30000 )
#define TWDT_KICK_TIME                  ( TWDT_TIMEOUT / 10 )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the watchdog monitor.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    void ( *Update )( void );
    void ( *Configure )( uint32_t timeout );
    void ( *SetTimeout )( uint32_t timeout );
    void ( *Report )( void );
    void ( *List )( void );
} const twdt_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern twdt_interface_t twdt;

#endif /* __TWDT_H__ */

/**
 * @} TWDT
 */

/**
 * @} Applicaton
 */
