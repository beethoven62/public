/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      slm.h
 * @details   LTE modem on the nRF9160DK and the Thingy:91 demo boards.
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup Modem
 * @{
 */
#ifndef __SLM_H__
#define __SLM_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "dmm.h"
#include "embedded_cli.h"
#include "app.h"
#include "log.h"
#include "cli.h"
#include "modem.h"
#include "tls.h"
#include "mqtt.h"
#include "twdt.h"
#include "blinky.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */
#define POLL_TIMEOUT_MS         ( 10000 )
#define SLM_BUFFER_SIZE         ( 512 )
#define NTP_PACKET_SIZE         ( 48 )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the SLM driver.
 */
typedef struct
{
    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
    // li.   Two bits.   Leap indicator.
    // vn.   Three bits. Version number of the protocol.
    // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet_t;              // Total: 384 bits or 48 bytes.

typedef struct
{
    error_code_module_t ( *Init )( void );
    void ( *Command )( char *args );
    EmbeddedCli* ( *Bindings )( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer );
    void ( *Writechar )( EmbeddedCli *embedded_cli, char c );
} const slm_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern slm_interface_t slm;

#endif /* __SLM_H__ */

/**
 * @} Modem
 */

/**
 * @} Driver
 */
