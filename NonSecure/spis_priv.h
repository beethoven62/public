/** @file spis_priv.h
 *
 * @brief       Driver for Serial Peripheral Interface slave module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup spis
 * @{
 */

#ifndef __SPIS_PRIV_H__
#define __SPIS_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "spis.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define SPIS_DEFAULT_CONFIG_IRQ_PRIORITY    7
#define SPIS_IRQn                           8

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
    bool                        rx_done;
    nrfx_spis_t                 spis;
    nrfx_spis_config_t          config;
    uint32_t                    rxbuf_nbytes;
    uint8_t                     *txbuf;
    uint8_t                     *rxbuf;
    size_t                      len;
    uint8_t                     int_pin;
} spis_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the SPIS interface driver thread.
 * @param[in]   id           SPIS interface id number.
 * @param[in]   clk_pin      Clock pin.
 * @param[in]   mosi_pin     Master out/Slave in pin.
 * @param[in]   miso_pin     Master in/Slave out pin.
 * @param[in]   ss_pin       Slave select pin.
 * @param[in]   int_pin      Interrupt output pin (asserted low).
 * @return      Error code.
 */
static error_code_module_t spis_Init( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, uint8_t int_pin );

/**
 * @brief       Get SPI receive buffer status.
 * @return      Received number of bytes in buffer.
 */
static uint32_t spis_NRxBytes( void );

/**
 * @brief       Get SPI receive buffer full.
 * @return      SPI receive buffer full status.
 */
static bool spis_RxDone( void );

/**
 * @brief       Set SPI Tx/Rx buffer addresses and assert SPI interrupt line.
 * @param[in]   txbuf       Transmit buffer address.
 * @param[in]   rxbuf       Receive buffer address.
 * @param[in]   len         Size of each buffer.
 */
static void spis_BufSet( uint8_t *txbuf, uint8_t *rxbuf, size_t len );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       SPI master interface callback function.
 * @param[in]   *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 */
static void spis_EventHandler( nrfx_spis_evt_t const *p_event, void *p_context );

#endif /* __SPIS_PRIV_H__ */

/**
 * @}
 */
