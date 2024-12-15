/** @file mqtt_priv.h
 *
 * @brief       MQTT client module (private header).
 * @author      Johnas Cukier
 * @date        May 2023
 *
 */

/**
 * @addtogroup Mqtt
 * @{
 */

#ifndef __MQTT_PRIV_H__
#define __MQTT_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "mqtt.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define MQTT_TIMEOUT            ( 500 )
#define MQTT_ENDPOINT           "a3kaq5feq0kj3v-ats.iot.us-east-1.amazonaws.com"
#define MQTT_PORT               ( 8883 )
#define MQTT_KEEP_ALIVE         ( 60 )
#ifdef TARGET_DEVICE_NRF9160DK
#define MQTT_ID                 "TestDevice-nRF9160DK"
#elifdef TARGET_DEVICE_THINGY91
#define MQTT_ID                 "TestDevice-Thingy91"
#else
#ERROR "Device ID undefined"
#endif
#define MQTT_ID_LENGTH          ( sizeof( MQTT_ID ) - 1 )
#define MQTT_USER               MQTT_ENDPOINT "/" MQTT_ID "/?api-version=2018-06-30"
#define MQTT_USER_LENGTH        ( sizeof( MQTT_USER ) - 1 )
#define MQTT_PASS               NULL
#define MQTT_PASS_LENGTH        ( 0 )
#define MQTT_TOPIC_COUNT        ( 1 )
#define MQTT_TOPIC              "test"
#define MQTT_TOPIC_LENGTH       ( ( sizeof( MQTT_TOPIC ) - 1 ) )
#define MQTT_MESSAGE_EXAMPLE    "Hello World!"

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

struct NetworkContext
{
    int32_t socket;
};

typedef struct
{
    MQTTContext_t               context;
    MQTTConnectInfo_t           connection_info;
    MQTTPublishInfo_t           publish_info;
    MQTTSubscribeInfo_t         subscribe_info[ MQTT_TOPIC_COUNT ];
    char                        topic[ SHORT_MSG_MAX ];
} mqtt_session_t;

typedef struct
{
    bool                        is_init;
    bool                        is_subscribed;
    uint32_t                    sequence_number;
    NetworkContext_t            net_context;
    mqtt_session_t              session;
    MQTTFixedBuffer_t           buffer;
    TransportInterface_t        transport;
    SemaphoreHandle_t           mutex_handle;
    TimerHandle_t               timer_handle;
} mqtt_obj_t ;

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
static error_code_module_t mqtt_Init( void );
static MQTTStatus_t mqtt_Send( char *topic, char *msg );
static MQTTStatus_t mqtt_Subscribe( char *topic );
static MQTTStatus_t mqtt_Unsubscribe( void );
static void mqtt_Status( void );
static bool mqtt_isInit( void );

static void mqtt_Callback( MQTTContext_t *mqtt_context, MQTTPacketInfo_t *packet_info, MQTTDeserializedInfo_t *deserialized_info );
static void mqtt_ProcessResponse( MQTTPacketInfo_t *packet_info, uint16_t packet_identifier );
static void mqtt_ProcessIncomingPublish( MQTTPublishInfo_t *publish_info );
static void mqtt_Timeout( TimerHandle_t handle );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       MQTT client thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static int32_t mqtt_Receive( NetworkContext_t *context, void *buffer, size_t size );
static int32_t mqtt_Transmit( NetworkContext_t *context, const void * buffer, size_t size );

#endif /* __MQTT_PRIV_H__ */

/**
 * @}
 */
