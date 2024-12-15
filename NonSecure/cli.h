/**
 * @addtogroup Application Application
 * @{
 *
 * @file      cli.h
 * @brief     Command line interface module (public header).
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup CLI
 * @{
 */
#ifndef __CLI_H__
#define __CLI_H__

/***************************************************************************************************************************
 * Includes
 */

#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "embedded_cli.h"
#include "os.h"
#include "eelcodes.h"
#include "board.h"
#include "app.h"
#include "log.h"
#include "twdt.h"
#include "modem.h"
#include "blinky.h"
#include "slm.h"

/***************************************************************************************************************************
 * Public constants and macros
 */
#define CLI_BUFFER_SIZE         ( 512 )
#define CLI_WAIT_TIME           ( 50 )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the watchdog monitor.
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
    void ( *Enable )( bool state );
    EmbeddedCli* ( *Bindings )( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer );
    void ( *Writechar )( EmbeddedCli *embedded_cli, char c );
} const cli_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern cli_interface_t cli;

#endif /* __CLI_H__ */

/**
 * @} CLI
 */

/**
 * @} Applicaton
 */
