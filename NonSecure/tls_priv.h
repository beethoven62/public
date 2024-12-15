/** @file tls_priv.h
 *
 * @brief       Secure TLS layer functions (private header).
 * @author      Najib Bubong
 * @author      Johnas Cukier
 * @date        May 2023
 *
 */

/**
 * @addtogroup tls
 * @{
 */

#ifndef __TLS_PRIV_H__
#define __TLS_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "tls.h"

/***************************************************************************************************************************
 * Private constants and macros
 */
#define ARRAY_SIZE( arr )    ( sizeof( arr ) / sizeof( ( arr )[ 0 ] ) )
#define SCRATCH_SIZE         ( 1500 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */
typedef enum {
    cred_type_ca_chain,
    cred_type_public_cert,
    cred_type_private_key,
    cred_type_psk,
    cred_type_identity
} tls_cred_type_t;

/* Set up TLS peer verification */
typedef enum
{
    ver_none                = 0,
    ver_optional            = 1,
    ver_required            = 2,
} tls_ver_t;

/* Cipher suite structure */
typedef struct {
    char *name;
    uint16_t value;
    bool supported;
} cipher_suite_t;

typedef struct
{
    bool                        is_init;
    bool                        always_provision;
} tls_obj_t;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */
/**
 * @brief       Initialize the modem with TLS credentials.
 * @details     Must be called before LTE modem is activated (CFUN=0)
 * @return      Error code.
 */
static error_code_module_t tls_Init( void );

/**
 * @brief       TLS/mTLS handshake setup.
 * @param[in]   socket       Socket file descriptor.
 * @param[in]   host_name    Host-name of the server.
 * @param[in]   tag          Security tag (RootCA, public cert, private key).
 * @return:     x == 0 , no error
 *              x != 0 , error number
 */
static int32_t tls_Setup( int32_t socket, const char *host_name, tls_tag_t tag );

/**
 * @brief       TLS/mTLS certificates dump.
 * @return:     x == 0 , no error
 *              x != 0 , error number
 */
static int32_t tls_Dump( void );

/***************************************************************************************************************************
 * Private prototypes
 */
/**
 * @brief       Enable/Disable the use of the final result code
 * @param[in]   enable      Enable/Disable (true = enable, false = disable)
 * @return      Error
 */
static int32_t tls_Cmee( bool enable );

/**
 * @brief       Check if credentials exist in modem
 * @param[in]   tag         Credential security tag
 * @param[in]   type        Credential type
 * @param[out]  exists      Credential exists? (true = yes, false = no)
 */
static int32_t tls_KeyExists( tls_tag_t tag,
                              tls_cred_type_t type,
                              bool *exists );

static int32_t tls_WriteKey( tls_tag_t tag,
                             tls_cred_type_t type,
                             const void *buf,
                             size_t len );

static int32_t tls_EraseKey( tls_tag_t tag,
                             tls_cred_type_t type );

static int32_t tls_ReadKey( tls_tag_t tag,
                            tls_cred_type_t type,
                            void *buf,
                            size_t len );

static void tls_PrintCredential( char *buf, size_t len );

#endif /* __TLS_PRIV_H__ */

/**
 * @} tls
 */
