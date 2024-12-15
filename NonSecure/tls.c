/**
 * @addtogroup Application
 * @{
 * @file      tls.c
 * @brief     Secure TLS layer functions
 * @author    Najib Bubong
 * @author    Johnas Cukier
 * @date      May 2023
 */

/**
 * @defgroup  tls TLS network security module
 * @brief     Implements TLS v1.2 using built-in LTE modem firmware
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "tls_priv.h"

/***************************************************************************************************************************
 * Global variables
 */
tls_interface_t tls =
{
    .Init               = &tls_Init,
    .Setup              = &tls_Setup,
    .Dump               = &tls_Dump,
};

tls_obj_t tls_obj =
{
    .is_init            = false,
    .always_provision   = true,    // TODO: remove later and do not expose all certificates and keys
};

/* Taken from the IANA register */
static cipher_suite_t cipher_suites[] =
{
    { "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384", 0xC024 },
    { "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA", 0xC00A },
    { "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256", 0xC023 },
    { "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA", 0xC009 },
    { "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA", 0xC014 },
    { "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256", 0xC027 }, //
    { "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA", 0xC013 },
    { "TLS_PSK_WITH_AES_256_CBC_SHA", 0x008D },
    { "TLS_PSK_WITH_AES_128_CBC_SHA256", 0x00AE },
    { "TLS_PSK_WITH_AES_128_CBC_SHA", 0x008C },
    { "TLS_PSK_WITH_AES_128_CCM_8", 0xC0A8 },
    { "TLS_EMPTY_RENEGOTIATIONINFO_SCSV", 0x00FF },
#if 1 // CONFIG_EXTENDED_CIPHERSUITE_LIST
    { "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256", 0xC02B },
    { "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384", 0xC030 },
    { "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256", 0xC02F },
#endif
};

static const char rootCA[] =
{
#include "AmazonRootCA1.pem"
};

static const char client_cert[] =
{
#ifdef TARGET_DEVICE_NRF9160DK
#include "TestDevice-nRF9160DK.cert.pem"
#elifdef TARGET_DEVICE_THINGY91
#include "TestDevice-Thingy91.cert.pem"
#else
#ERROR "Certificate undefined"
#endif
};

static const char private_key[] =
{
#ifdef TARGET_DEVICE_NRF9160DK
#include "TestDevice-nRF9160DK.private.key"
#elifdef TARGET_DEVICE_THINGY91
#include "TestDevice-Thingy91.private.key"
#else
#ERROR "Private key undefined"
#endif
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t tls_Init( void )
{
    bool exists = false;
    int32_t err = tls_KeyExists( tag_tls, cred_type_ca_chain, &exists );

    modem.Stop();
    Log.DebugPrint( "Provisioning certificates" );
    if ( !exists || tls_obj.always_provision )
    {
#if 0
        if ( ( err = tls_WriteKey( tag_tls, cred_type_ca_chain, rootCA, sizeof( rootCA ) ) ) )
        {
            Log.ErrorPrint( "TLS CA Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_EraseKey( tag_mtls, cred_type_ca_chain ) ) )
        {
            Log.ErrorPrint( "mTLS CA Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_WriteKey( tag_mtls, cred_type_ca_chain, rootCA, sizeof( rootCA ) ) ) )
        {
            Log.ErrorPrint( "mTLS CA Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_EraseKey( tag_mtls, cred_type_public_cert ) ) )
        {
            Log.ErrorPrint( "mTLS Public Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_WriteKey( tag_mtls, cred_type_public_cert, client_cert, sizeof( client_cert ) ) ) )
        {
            Log.ErrorPrint( "mTLS Public Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_EraseKey( tag_mtls, cred_type_private_key ) ) )
        {
            Log.ErrorPrint( "mTLS Private Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_WriteKey( tag_mtls, cred_type_private_key, private_key, sizeof( private_key ) ) ) )
        {
            Log.ErrorPrint( "mTLS Private Cert failed, err=%d, errno=%d", err, errno );
        }
#else
        if ( ( err = tls_WriteKey( tag_tls, cred_type_ca_chain, rootCA, sizeof( rootCA ) ) ) )
        {
            Log.ErrorPrint( "TLS CA Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_WriteKey( tag_tls, cred_type_public_cert, client_cert, sizeof( client_cert ) ) ) )
        {
            Log.ErrorPrint( "mTLS Public Cert failed, err=%d, errno=%d", err, errno );
        }
        else if ( ( err = tls_WriteKey( tag_tls, cred_type_private_key, private_key, sizeof( private_key ) ) ) )
        {
            Log.ErrorPrint( "mTLS Private Cert failed, err=%d, errno=%d", err, errno );
        }
#endif
    }

    if ( err == 0 )
    {
        tls_obj.is_init = true;
        Log.InfoPrint( "TLS provisioning complete" );
    }
    return err;
}

int32_t tls_Setup( int32_t socket, const char *host_name, tls_tag_t tag )
{
    int32_t err = 0;
    int32_t parm;
    //nrf_sec_cipher_t ciphersuite_list[12];
    nrf_sec_cipher_t ciphersuite_list[1];

    Log.DebugPrint( "Setting up TLS for socket=%d, host=%s, tag=%u", socket, host_name, tag );

#if 1
    ciphersuite_list[ 0 ] = cipher_suites[ 5 ].value;
#else
    uint32_t i;
    for ( i = 0; i < 12; i++ )
    {
        ciphersuite_list[ i ] = xCiphersuites[ i ].value;
    }
#endif

    if ( err == 0 )
    {
        parm = ver_optional; //ver_required;
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_PEER_VERIFY), verify=%d", parm );
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_PEER_VERIFY, &parm, sizeof( nrf_sec_peer_verify_t ) );
    }

    if ( err == 0 )
    {
        parm = tag;
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_TAG_LIST)" );
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_TAG_LIST, &parm, sizeof( nrf_sec_tag_t ) );
    }

    if ( err == 0 )
    {
        parm = 1;
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_SESSION_CACHE)" );
        err = nrf_setsockopt(socket, NRF_SOL_SECURE, NRF_SO_SEC_SESSION_CACHE, &parm, sizeof( nrf_sec_session_cache_t ) );
    }

    if ( err == 0 )
    {
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_HOSTNAME)" );
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_HOSTNAME, host_name, strlen( host_name ) );
    }

    if ( err == 0 )
    {
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_CIPHERSUITE_LIST), len=%d", sizeof( ciphersuite_list ));
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_CIPHERSUITE_LIST, ciphersuite_list, sizeof( ciphersuite_list ) );
    }

    if ( err == 0 )
    {
        parm = 0;
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_SESSION_CACHE)" );
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_SESSION_CACHE, &parm, sizeof( nrf_sec_session_cache_t ) );
    }

    if ( err == 0 )
    {
        parm = 0;
        Log.DebugPrint( "nrf_setsockopt(NRF_SO_SEC_ROLE)" );
        err = nrf_setsockopt( socket, NRF_SOL_SECURE, NRF_SO_SEC_ROLE, &parm, sizeof( nrf_sec_role_t ) );
    }

    if ( err != 0 )
    {
        Log.ErrorPrint( "Failed to setup TLS/mTLS session, err=%d, errno=%d", err, errno );
    }

    return err;
}

int32_t tls_Dump( void )
{
    size_t len = 2048;
    char *buf = malloc( len );
    int32_t err = 0;

    if ( buf == NULL )
    {
        err = -EINVAL;
    }

    if ( err == 0 )
    {
        Log.Print( "Read tag: %d\r\n", tag_tls );
        err = tls_ReadKey( tag_tls, cred_type_ca_chain, buf, len );
        tls_PrintCredential( buf, len );
    }

    if ( err == 0 )
    {
        Log.Print( "Read tag: %d\r\n", tag_mtls );
        err = tls_ReadKey( tag_mtls, cred_type_ca_chain, buf, len );
        tls_PrintCredential( buf, len );
    }

    free( buf );
    return err;
}

void tls_PrintCredential( char *buf, size_t len )
{
    uint32_t i, line_len = 65;
    char *cert = modem.StriStr( buf, "-----" );
    char *start, *end;
    char line[ 67 ];

    start = cert;
    end = modem.StriStr( start, "\n" );
    for ( i = 0; end != 0 && i < len; i++)
    {
        line_len = ( uint32_t )( end - start + 1 );
        memcpy( line, start, line_len );
        start = end + 1;
        end = modem.StriStr( start, "\n" );
        line[ line_len ] = '\r';
        line[ line_len + 1 ] = 0;
        Log.Print( line );
        os.Delay( 10 );
    }
}

int32_t tls_Cmee( bool enable )
{
    int err =  nrf_modem_at_printf("AT+CMEE=%d", enable ? 1 : 0 );
    if ( err < 0 )
    {
        Log.ErrorPrint( "Failed prvCMEEControl, err=%d errno=%d", err, errno );
    }

    return err;
}

int32_t tls_WriteKey( tls_tag_t tag, tls_cred_type_t type, const void *buf, size_t len )
{
    int32_t err = 0;

    if ( buf == NULL || len == 0 )
    {
        return -EINVAL;
    }

    tls_Cmee( true );
    err = nrf_modem_at_printf( "AT%%CMNG=0,%d,%d,\"\n%s\"", tag, type, (const char *)buf );
    tls_Cmee( false );

    return err;
}

int32_t tls_EraseKey( tls_tag_t tag, tls_cred_type_t type )
{
    int32_t err;

    tls_Cmee( true );
    err = nrf_modem_at_printf( "AT%%CMNG=3,%d,%d", tag, type );
    tls_Cmee( false );

    return err;
}

int32_t tls_ReadKey( tls_tag_t tag, tls_cred_type_t type, void *buf, size_t len )
{
    int32_t err = 0;

    if ( buf == NULL || len == 0 )
    {
        return -EINVAL;
    }

    tls_Cmee( true );
    err = nrf_modem_at_cmd( ( char *)buf, len, "AT%%CMNG=2,%d,%d", tag, type );
    tls_Cmee( false );

    return err;
}

int tls_KeyExists( tls_tag_t tag, tls_cred_type_t type, bool *exists )
{
    const size_t size = 128;
    int err = 0;
    char str [ 32 ];
    char *resp;

    if ( exists == NULL )
    {
        err = -EINVAL;
    }

    if ( err == 0 )
    {
        resp = malloc( size );
        *exists = false;

        sprintf( str, "AT%%CMNG=2,%d,%d", tag, type );
        err = nrf_modem_at_cmd( resp, size, "%s", str );

        /* Expected response, sample: "%CMNG: 16842754,0,"760EB917DCF13F71B491D6DE8B04F79D638E1AEC40578384A1BE8BF" ... */
        sprintf( str, "CMNG: %d,%d,\"", tag, type );
        resp[ strlen( str ) + 4 ] = '\0';       // Evaluating some parts only, good enough to validate the existince of the CERT
        if ( modem.StriStr( resp, str ) != NULL )
        {
            *exists = true;
        }

        free( resp );
    }

    return err;
}

/**
 * @} tls
 */

/**
 * @} Application
 */
