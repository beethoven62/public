/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      spis.h
 * @brief     Driver for serial peripheral interface slave module (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup spis
 * @{
 */
#ifndef __SPIS_H__
#define __SPIS_H__

/***************************************************************************************************************************
 * Includes
 */

#include <nrfx_spis.h>
#include "gpio.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the SPI master interface driver.
 */
typedef struct
{
    error_code_module_t ( *Init )( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, uint8_t int_pin );
    uint32_t ( *NRxBytes )( void );
    bool ( *RxDone )( void );
    void ( *BufSet )( uint8_t *txbuf, uint8_t *rxbuf, size_t len );
} const spis_driver_t;

/***************************************************************************************************************************
 * Public variables
 */

extern spis_driver_t spis;

#endif /* __SPIS_H__ */

/**
 * @} spis
 */

/**
 * @} Driver
 */
