/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      board.h
 * @brief     Board setup module (public header).
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup Board
 * @{
 */
#ifndef __BOARD_H__
#define __BOARD_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdbool.h>
#include "eelcodes.h"
#include "twim.h"
#include "uart.h"
#include "gpio.h"
#if NRFX_SPIM_ENABLED
#include "spim.h"
#endif
#if NRFX_SPIS_ENABLED
#include "spis.h"
#endif

/***************************************************************************************************************************
 * Public constants and macros
 */

#if TARGET_DEVICE_NRF9160DK
#define NUM_LEDS                        ( 4 )
#define LED1_PIN                        ( 2 )
#elif TARGET_DEVICE_THINGY91
#define NUM_LEDS                        ( 3 )
#define LED1_PIN                        ( 0 )
#else
#define NUM_LEDS                        ( 0 )
#define LED1_PIN                        ( 0 )
#endif

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

typedef struct
{
    error_code_module_t ( *Init )( void );
} const board_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern board_interface_t board;

#endif /* __BOARD_H__ */

/**
 * @} Board
 */

/**
 * @} Driver
 */
