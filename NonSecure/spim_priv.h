/** @file spim_priv.h
 *
 * @brief       Driver for Serial Peripheral Interface master module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup spim
 * @{
 */

#ifndef __SPIM_PRIV_H__
#define __SPIM_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "spim.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define SPIM_DEFAULT_CONFIG_IRQ_PRIORITY    7
#define SPIM_IRQn                           8

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
    nrfx_spim_t                 spim;
    nrfx_spim_config_t          config;
    nrfx_spim_xfer_desc_t       desc;
    uint32_t                    rxbuf_nbytes;
} spim_obj_t ;

typedef void ( *spim_event_handler_t )( nrfx_spim_evt_t const *p_event, void *p_context );

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the SPIM interface driver thread.
 * @param[in]   id           SPIM interface id number.
 * @param[in]   clk_pin      Clock pin.
 * @param[in]   mosi_pin     Master out/Slave in pin.
 * @param[in]   miso_pin     Master in/Slave out pin.
 * @param[in]   ss_pin       Slave select pin.
 * @param[in]   freq         Bus frequency.
 * @return      Error code.
 */
static error_code_module_t spim_Init( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, nrf_spim_frequency_t freq );

/**
 * @brief       SPI master interface transfer function.
 * @param[in]   *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 */
static void spim_Xfer( uint8_t *tx_data, uint8_t *rx_data, size_t len );

/**
 * @brief       Get SPI receive buffer status.
 * @return      Received number of bytes in buffer.
 */
static uint32_t spim_NRxBytes( void );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       SPI master interface callback function.
 * @param[in]   *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 */
static void spim_EventHandler( nrfx_spim_evt_t const *p_event, void *p_context );

#endif /* __SPIM_PRIV_H__ */

/**
 * @}
 */
