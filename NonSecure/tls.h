/**
 * @addtogroup Application Application
 * @{
 *
 * @file      tls.h
 * @brief     Secure TLS layer functions
 * @author    Najib Bubong
 * @author    Johnas Cukier
 * @date      May 2023
 */

/**
 * @addtogroup tls
 * @{
 */
#ifndef __TLS_H__
#define __TLS_H__

/***************************************************************************************************************************
 * Includes
 */

#include "os.h"
#include "eelcodes.h"
#include "dmm.h"
#include "log.h"
#include "modem.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

typedef enum
{
    tag_tls                 = 0x01010002,       // 16842754,
    tag_mtls                = 0x00bad5d8,       // 12244440,
} tls_tag_t;

typedef struct
{
    error_code_module_t ( *Init )( void );
    int32_t ( *Setup )( int32_t socket, const char *host_name, tls_tag_t tag );
    int32_t ( *Dump )( void );
} const tls_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern tls_interface_t tls;

#endif /* __TLS_H__ */