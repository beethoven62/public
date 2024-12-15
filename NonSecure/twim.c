/**
 * @addtogroup Driver
 * @{
 * @file      twim.c
 * @brief     Driver for two-wire interface (I2C) master module
 * @details   Implements an I2C master.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup twim Two-wire master driver module
 * @brief     Two-wire (I2C) interface driver
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "twim_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

twim_driver_t twim =
{
    .Init               = &twim_Init,
    .Transmit           = &twim_Transmit,
    .Receive            = &twim_Receive,
};

twim_obj_t twim_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t twim_Init( uint8_t id, uint8_t scl_pin, uint8_t sda_pin )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( twim_obj.is_init == false )
    {
        /* Initialize TWI id */
        switch ( id )
        {
        case 0:
#if NRFX_TWIM0_ENABLED
            twim_driver.p_twim = NRF_TWIM0;
            twim_driver.drv_inst_idx = NRFX_TWIM0_INST_IDX;
#endif
            break;
        case 1:
#if NRFX_TWIM1_ENABLED
            twim_driver.p_twim = NRF_TWIM1;
            twim_driver.drv_inst_idx = NRFX_TWIM1_INST_IDX;
#endif
            break;
        case 2:
#if NRFX_TWIM2_ENABLED
            twim_driver.p_twim = NRF_TWIM2;
            twim_driver.drv_inst_idx = NRFX_TWIM2_INST_IDX;
#endif
            break;
        case 3:
#if NRFX_TWIM3_ENABLED
            twim_driver.p_twim = NRF_TWIM3;
            twim_driver.drv_inst_idx = NRFX_TWIM3_INST_IDX;
#endif
            break;
        default:
            error = ERROR_TWIM_NOT_INIT;
            break;
        }

        if ( error == NO_ERROR )
        {
            twim_driver_config.scl                = scl_pin;
            twim_driver_config.sda                = sda_pin;
            twim_driver_config.frequency          = NRF_TWIM_FREQ_100K;
            twim_driver_config.interrupt_priority = NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY;
            twim_driver_config.hold_bus_uninit    = false;
            nrfx_twim_init( &twim_driver, &twim_driver_config, NULL, NULL );
            nrfx_twim_enable( &twim_driver );
        }

        /* init local RAM objects */
        twim_obj.is_init = true;
    }
    else
    {
        error = ERROR_TWIM_ALREADY_INIT;
    }

    return error;
}

void twim_Transmit( uint8_t addr, uint8_t *data, uint8_t len )
{
    nrfx_twim_xfer_desc_t twim_xfer = NRFX_TWIM_XFER_DESC_TX( addr, data, len );
    nrfx_twim_xfer( &twim_driver, &twim_xfer, 0 );
}

void twim_Receive( uint8_t addr, uint8_t *data, uint8_t len )
{
    nrfx_twim_xfer_desc_t twim_xfer = NRFX_TWIM_XFER_DESC_RX( addr, data, len );
    nrfx_twim_xfer( &twim_driver, &twim_xfer, 0 );
}

/**
 * @} twim
 */

/**
 * @} Driver
 */
