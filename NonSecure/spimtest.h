/**
 * @addtogroup Application Application
 * @{
 *
 * @file      spimtest.h
 * @brief     SPI master test module (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup spimtest
 * @{
 */
#ifndef __SPIMTEST_H__
#define __SPIMTEST_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "eelcodes.h"
#include "board.h"
#include "spim.h"
#include "app.h"
#include "log.h"
#include "twdt.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the SPI master demo task.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
} const spimtest_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern spimtest_interface_t spimtest;

#endif /* __SPIMTEST_H__ */

/**
 * @} spimtest
 */

/**
 * @} Applicaton
 */
