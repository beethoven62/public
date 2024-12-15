/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      spim.h
 * @brief     Driver for serial peripheral interface master module
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup spim
 * @{
 */
#ifndef __SPIM_H__
#define __SPIM_H__

/***************************************************************************************************************************
 * Includes
 */

#include <nrfx_spim.h>
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
    error_code_module_t ( *Init )( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, nrf_spim_frequency_t freq );
    void ( *Xfer )( uint8_t *tx_data, uint8_t *rx_data, size_t len );
    uint32_t ( *NRxBytes )( void );
} const spim_driver_t;

/***************************************************************************************************************************
 * Public variables
 */

extern spim_driver_t spim;

#endif /* __SPIM_H__ */

/**
 * @} spim
 */

/**
 * @} Driver
 */
