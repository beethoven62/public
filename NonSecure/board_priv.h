/** @file board_priv.h
 *
 * @brief       Board setup module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup Board
 * @{
 */

#ifndef __BOARD_PRIV_H__
#define __BOARD_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "board.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/**
 * @brief Pinout for peripherals
 */
#if TARGET_DEVICE_NRF9160DK
#define UART1_TX_PIN                    ( 29 )
#define UART1_RX_PIN                    ( 28 )
#define UART2_TX_PIN                    ( 1 )
#define UART2_RX_PIN                    ( 0 )
#define UART2_CTS_PIN                   ( 15 )
#define UART2_RTS_PIN                   ( 14 )
#define SPI0_CLK_PIN                    ( 16 )
#define SPI0_MOSI_PIN                   ( 17 )
#define SPI0_MISO_PIN                   ( 18 )
#define SPI0_SS_PIN                     ( 19 )
#define SPI0_INT_PIN                    ( 20 )
#else
#define UART1_TX_PIN                    ( 22 )
#define UART1_RX_PIN                    ( 23 )
#define UART1_RTS_PIN                   ( 24 )
#define UART1_CTS_PIN                   ( 25 )
#define UART2_TX_PIN                    ( 18 )
#define UART2_RX_PIN                    ( 19 )
#define UART2_RTS_PIN                   ( 20 )
#define UART2_CTS_PIN                   ( 21 )
#define SPI0_CLK_PIN                    ( 10 )
#define SPI0_MOSI_PIN                   ( 11 )
#define SPI0_MISO_PIN                   ( 12 )
#define SPI0_SS_PIN                     ( 13 )
#define SPI0_INT_PIN                    ( 20 )
#endif

#define TWIM0_SCL_PIN                   ( 12 )
#define TWIM0_SDA_PIN                   ( 11 )
#define ADP5360_ADDR                    ( 0x46 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} board_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @return      Error code.
 */
static error_code_module_t board_Init( void );

/***************************************************************************************************************************
 * Private prototypes
 */

#endif /* __BOARD_PRIV_H__ */

/**
 * @}
 */
