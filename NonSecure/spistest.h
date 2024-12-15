/**
 * @addtogroup Application Application
 * @{
 *
 * @file      spistest.h
 * @brief     SPI slave test module (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup spistest
 * @{
 */
#ifndef __SPISTEST_H__
#define __SPISTEST_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "eelcodes.h"
#include "board.h"
#include "app.h"
#include "spis.h"
#include "log.h"
#include "twdt.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the SPI slave demo task.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
} const spistest_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern spistest_interface_t spistest;

#endif /* __SPISTEST_H__ */

/**
 * @} spistest
 */

/**
 * @} Applicaton
 */
