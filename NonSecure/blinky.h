/**
 * @addtogroup Application Application
 * @{
 *
 * @file      blinky.h
 * @brief     Blinking LED demo module (public header).
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup Blinky
 * @{
 */
#ifndef __BLINKY_H__
#define __BLINKY_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdint.h>
#include <stdbool.h>
#include "os.h"
#include "eelcodes.h"
#include "board.h"
#include "gpio.h"
#include "app.h"
#include "log.h"
#include "twdt.h"
#include "mqtt.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the blinky demo task.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    void ( *Configure )( uint32_t time_ms, uint32_t n_leds );
    void ( *EnableMQTT )( bool enable );
} const blinky_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern blinky_interface_t blinky;

#endif /* __BLINKY_H__ */

/**
 * @} Blinky
 */

/**
 * @} Applicaton
 */
