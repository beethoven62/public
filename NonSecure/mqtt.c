/**
 * @addtogroup Application Application
 * @{
 * @file      mqtt.c
 * @brief     MQTT client module
 * @details   Connects to an MQTT broker and exchanges data.
 * @author    Johnas Cukier
 * @date      May 2023
 */

/**
 * @defgroup Mqtt MQTT client
 * @brief     MQTT client task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "mqtt_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

mqtt_interface_t mqtt =
{
    .Init               = &mqtt_Init,
    .Subscribe          = &mqtt_Subscribe,
    .Unsubscribe        = &mqtt_Unsubscribe,
    .Send               = &mqtt_Send,
    .Status             = &mqtt_Status,
};

mqtt_obj_t mqtt_obj =
{
    .is_init            = false,
    .is_subscribed      = false,
    .sequence_number    = 0,
    .net_context.socket = -1,
    .session            =
    {
        .context =
        {
            .networkBuffer =
            {
                .pBuffer = NULL,
                .size = 0,
            },
        },
        .connection_info =
        {
            .cleanSession = true,
            .pClientIdentifier = MQTT_ID,
            .clientIdentifierLength = MQTT_ID_LENGTH,
            .keepAliveSeconds = MQTT_KEEP_ALIVE,
            .pUserName = MQTT_USER,
            .userNameLength = MQTT_USER_LENGTH,
            .pPassword = MQTT_PASS,
            .passwordLength = MQTT_PASS_LENGTH,
        },
        .publish_info =
        {
            .qos = MQTTQoS1,
            .retain = false,
            .pTopicName = mqtt_obj.session.topic,
            .topicNameLength = MQTT_TOPIC_LENGTH,
            .pPayload = "",
            .payloadLength = 0,
        },
        .subscribe_info =
        {
            [ 0 ] =
            {
                .qos = MQTTQoS1,
                .pTopicFilter = mqtt_obj.session.topic,
                .topicFilterLength = MQTT_TOPIC_LENGTH,
            },
        },
    },
    .buffer             =
    {
        .pBuffer = NULL,
        .size = 0,
    },
    .transport          =
    {
        .pNetworkContext = &mqtt_obj.net_context,
        .recv = mqtt_Receive,
        .send = mqtt_Transmit,
    },
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t mqtt_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( mqtt_obj.is_init == false )
    {
        /* init local RAM objects */
        strcpy( mqtt_obj.session.topic, MQTT_TOPIC );
        mqtt_obj.timer_handle = os.CreateTimer( "MQTT_Timer", MQTT_KEEP_ALIVE * 500, true, NULL, mqtt_Timeout );
        mqtt_obj.mutex_handle = os.CreateMutex();
        mqtt_obj.is_init = true;
    }
    else
    {
        error = ERROR_MQTT_ALREADY_INIT;
    }

    return error;
}

void mqtt_Callback( MQTTContext_t *mqtt_context, MQTTPacketInfo_t *packet_info, MQTTDeserializedInfo_t *deserialized_info )
{
    if ( ( packet_info->type & 0xf0 ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        mqtt_ProcessIncomingPublish( deserialized_info->pPublishInfo );
    }
    else
    {
        mqtt_ProcessResponse( packet_info, deserialized_info->packetIdentifier );
    }
}

void mqtt_Timeout( TimerHandle_t handle )
{
    if ( mqtt_obj.is_subscribed && os.TakeSemaphore( mqtt_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        MQTT_Ping( &mqtt_obj.session.context );
        MQTT_ProcessLoop( &mqtt_obj.session.context, MQTT_TIMEOUT );
        os.GiveSemaphore( mqtt_obj.mutex_handle );
    }
    Log.InfoPrint( "MQTT Time out" );
}

void mqtt_ProcessIncomingPublish( MQTTPublishInfo_t *publish_info )
{
    char temp;
    char *ptr;

    if ( modem.StriStr( publish_info->pTopicName, mqtt_obj.session.topic ) != NULL )
    {
        ptr = ( char * )publish_info->pTopicName;
        temp = ptr[ publish_info->topicNameLength ];
        ptr[ publish_info->topicNameLength ] = 0;
        Log.InfoPrint( "Subscribed topic: %s", ptr );
        ptr[ publish_info->topicNameLength ] = temp;
        ptr = ( char * )publish_info->pPayload;
        temp = ptr[ publish_info->payloadLength ];
        ptr[ publish_info->payloadLength ] = 0;
        Log.InfoPrint( "Data: %s", ptr );
        ptr[ publish_info->payloadLength ] = temp;
    }
    else
    {
        Log.DebugPrint( "Topic: %s", publish_info->pTopicName );
    }
}

void mqtt_ProcessResponse( MQTTPacketInfo_t *packet_info, uint16_t packet_identifier )
{
    uint8_t *payload = NULL;
    size_t size = 0;
    MQTTStatus_t mqtt_status;

    switch( packet_info->type )
    {
    case MQTT_PACKET_TYPE_PUBACK:
        Log.DebugPrint( "PUBACK received for packet Id %u.", packet_identifier );
        break;

    case MQTT_PACKET_TYPE_SUBACK:
        /* A SUBACK from the broker, containing the server response to our subscription request, has been received.
         * It contains the status code indicating server approval/rejection for the subscription to the single topic
         * requested. The SUBACK will be parsed to obtain the status code, and this status code will be stored in global
         * variable #xTopicFilterContext. */
        mqtt_status = MQTT_GetSubAckStatusCodes( packet_info, &payload, &size );
        if ( mqtt_status != MQTTSuccess )
        {
            Log.ErrorPrint( "SUBACK failure: %d", mqtt_status );
        }
        break;

    case MQTT_PACKET_TYPE_UNSUBACK:
        Log.DebugPrint( "Unsubscribed from the topic %s.", mqtt_obj.session.topic );
        break;

    case MQTT_PACKET_TYPE_PINGRESP:
        /* Nothing to be done from application as library handles
         * PINGRESP with the use of MQTT_ProcessLoop API function. */
        Log.DebugPrint( "PINGRESP should not be handled by the application callback when using MQTT_ProcessLoop." );
        break;

    /* Any other packet type is invalid. */
    default:
        Log.ErrorPrint( "mqtt_ProcessResponse() called with unknown packet type:(%02x).", packet_info->type );
    }
}

MQTTStatus_t mqtt_Subscribe( char *topic )
{
    bool mqtt_session_present;
    MQTTStatus_t mqtt_status = MQTTSuccess;

    if ( mqtt_obj.is_init == true && os.TakeSemaphore( mqtt_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        os.TimerStart( mqtt_obj.timer_handle, QUEUE_WAIT_TIME );
        if ( mqtt_obj.buffer.pBuffer == NULL )
        {
            if ( ( mqtt_obj.buffer.pBuffer = malloc( LONG_MSG_MAX ) ) != NULL )
            {
                // Set up MQTT buffer
                mqtt_obj.buffer.size = LONG_MSG_MAX;
                mqtt_status = MQTTSuccess;
            }
            else
            {
                mqtt_status = MQTTNoMemory;
            }

        }

        // Connect to remote server
        if ( mqtt_status == MQTTSuccess && !mqtt_obj.is_subscribed )
        {
            mqtt_obj.net_context.socket = modem.Connect( "tls", MQTT_ENDPOINT, MQTT_PORT, 5000, 5000 );
            if ( mqtt_obj.net_context.socket >= 0 )
            {
                // Initialize MQTT client
                mqtt_status = MQTT_Init( &mqtt_obj.session.context,
                                         &mqtt_obj.transport,
                                         os.GetTickCountMs,
                                         mqtt_Callback,
                                         &mqtt_obj.buffer );
                // Connect to MQTT broker
                if ( mqtt_status == MQTTSuccess )
                {
                    mqtt_status = MQTT_Connect( &mqtt_obj.session.context,
                                                &mqtt_obj.session.connection_info,
                                                NULL,
                                                MQTT_TIMEOUT << 1,
                                                &mqtt_session_present );
                    Log.DebugPrint( "MQTT username: %s", mqtt_obj.session.connection_info.pUserName );
                    Log.InfoPrint( "MQTT connection established with %s.", MQTT_ENDPOINT );
                }
            }
            else
            {
                mqtt_status = MQTTServerRefused;
            }

            // Subscribe to MQTT topic
            if ( mqtt_status == MQTTSuccess )
            {
                if ( topic != NULL )
                {
                    strcpy( mqtt_obj.session.topic, topic );
                    mqtt_obj.session.subscribe_info[ 0 ].topicFilterLength = strlen( topic );
                }
                mqtt_status = MQTT_Subscribe( &mqtt_obj.session.context,
                                              mqtt_obj.session.subscribe_info,
                                              MQTT_TOPIC_COUNT,
                                              MQTT_GetPacketId( &mqtt_obj.session.context ) );
                Log.InfoPrint( "Subscribed topic: %s", topic );
            }
            if ( mqtt_status == MQTTSuccess )
            {
                mqtt_status = MQTT_ProcessLoop( &mqtt_obj.session.context, MQTT_TIMEOUT );
            }
            if ( mqtt_status == MQTTSuccess )
            {
                mqtt_obj.is_subscribed = true;
            }
        }
        os.GiveSemaphore( mqtt_obj.mutex_handle );
    }
    else
    {
        mqtt_status = MQTTIllegalState;
    }

    return mqtt_status;
}

MQTTStatus_t mqtt_Unsubscribe( void )
{
    MQTTStatus_t mqtt_status = MQTTSuccess;

    if ( mqtt_obj.is_init == true && os.TakeSemaphore( mqtt_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        os.TimerStop( mqtt_obj.timer_handle, QUEUE_WAIT_TIME );
        if ( mqtt_obj.is_subscribed )
        {
            // Unsubscribe from MQTT topic
            mqtt_status = MQTT_Unsubscribe( &mqtt_obj.session.context,
                                            mqtt_obj.session.subscribe_info,
                                            MQTT_TOPIC_COUNT,
                                            MQTT_GetPacketId( &mqtt_obj.session.context ) );
            Log.InfoPrint( "Unsubscribed topic: %s", mqtt_obj.session.topic );
            if ( mqtt_status == MQTTSuccess )
            {
                mqtt_status = MQTT_ProcessLoop( &mqtt_obj.session.context, MQTT_TIMEOUT );
            }
            // Disconnect from MQTT broker
            if ( mqtt_status == MQTTSuccess )
            {
                MQTT_Disconnect( &mqtt_obj.session.context );
                Log.InfoPrint( "Disconnect from %s.", MQTT_ENDPOINT );
            }

            // Log any errors
            if ( mqtt_status != MQTTSuccess )
            {
                Log.ErrorPrint( "MQTT transaction failed: %d", mqtt_status );
            }

            // Disconnect from remote server
            if ( modem.Disconnect( mqtt_obj.net_context.socket ) != NO_ERROR )
            {
                Log.ErrorPrint( "Disconnect failure" );
            }
            else
            {
                mqtt_obj.net_context.socket = -1;
            }

            mqtt_obj.is_subscribed = false;
        }

        // Free MQTT buffer
        if ( mqtt_obj.buffer.pBuffer != NULL )
        {
            free( mqtt_obj.buffer.pBuffer );
            mqtt_obj.buffer.pBuffer = NULL;
            mqtt_obj.buffer.size = 0;
        }
        os.GiveSemaphore( mqtt_obj.mutex_handle );
    }
    else
    {
        mqtt_status = MQTTIllegalState;
    }

    return mqtt_status;
}

int32_t mqtt_Receive( NetworkContext_t *context, void *buffer, size_t size )
{
    return modem.Receive( context->socket, buffer, size );
}

int32_t mqtt_Transmit( NetworkContext_t *context, const void * buffer, size_t size )
{
    return modem.Send( context->socket, ( uint8_t *)buffer, size );
}

MQTTStatus_t mqtt_Send( char *topic, char *msg )
{
    bool mqtt_session_present, stay_in_session = false;
    char publish_message[ SHORT_MSG_MAX ];
    MQTTStatus_t mqtt_status;

    if ( mqtt_obj.is_init == true && os.TakeSemaphore( mqtt_obj.mutex_handle, QUEUE_WAIT_TIME ) )
    {
        os.TimerReset( mqtt_obj.timer_handle, QUEUE_WAIT_TIME );
        if ( topic != NULL )
        {
            strcpy( mqtt_obj.session.topic, topic );
            mqtt_obj.session.subscribe_info[ 0 ].topicFilterLength = strlen( topic );
            mqtt_obj.session.publish_info.topicNameLength = strlen( topic );
        }
        else
        {
            mqtt_obj.session.subscribe_info[ 0 ].topicFilterLength = strlen( mqtt_obj.session.subscribe_info[ 0 ].pTopicFilter );
            mqtt_obj.session.publish_info.topicNameLength = strlen( mqtt_obj.session.publish_info.pTopicName );
        }

        if ( mqtt_obj.buffer.pBuffer == NULL )
        {
            if ( ( mqtt_obj.buffer.pBuffer = malloc( LONG_MSG_MAX ) ) != NULL )
            {
                // Set up MQTT buffer
                mqtt_obj.buffer.size = LONG_MSG_MAX;
                mqtt_status = MQTTSuccess;
            }
            else
            {
                mqtt_status = MQTTNoMemory;
            }

            // Subscribe
            if ( mqtt_status == MQTTSuccess )
            {
                mqtt_status = mqtt_Subscribe( ( char * )mqtt_obj.session.subscribe_info[ 0 ].pTopicFilter );
            }

            stay_in_session = false;
        }
        else
        {
            stay_in_session = true;
            mqtt_status = MQTTSuccess;
        }

        // Publish to MQTT topic
        if ( mqtt_status == MQTTSuccess )
        {
            sprintf( publish_message, "\"%s: %d\"", msg, mqtt_obj.sequence_number++ );
            mqtt_obj.session.publish_info.pPayload = publish_message;
            mqtt_obj.session.publish_info.payloadLength = strlen( publish_message );
            mqtt_status = MQTT_Publish( &mqtt_obj.session.context,
                                        &mqtt_obj.session.publish_info,
                                        MQTT_GetPacketId( &mqtt_obj.session.context ) );
            Log.InfoPrint( "Publish message: %s", publish_message );
        }
        if ( mqtt_status == MQTTSuccess )
        {
            mqtt_status = MQTT_ProcessLoop( &mqtt_obj.session.context, MQTT_TIMEOUT );
        }
        if ( !stay_in_session )
        {
            if ( mqtt_status == MQTTSuccess )
            {
                mqtt_status = mqtt_Unsubscribe();
            }

            // Free MQTT buffer
            if ( mqtt_obj.buffer.pBuffer != NULL )
            {
                free( mqtt_obj.buffer.pBuffer );
                mqtt_obj.buffer.pBuffer = NULL;
                mqtt_obj.buffer.size = 0;
            }
        }

        // Log any errors
        if ( mqtt_status != MQTTSuccess )
        {
            Log.ErrorPrint( "MQTT transaction failed: %s", MQTT_Status_strerror( mqtt_status ) );
        }
        os.GiveSemaphore( mqtt_obj.mutex_handle );
    }
    else
    {
        mqtt_status = MQTTIllegalState;
    }

    return mqtt_status;
}

void mqtt_Status( void )
{
    if ( mqtt_obj.is_subscribed )
    {
        Log.Print( "MQTT is subscribed.\r\n" );
        Log.Print( "MQTT username: %s\r\n", mqtt_obj.session.connection_info.pUserName );
        Log.Print( "MQTT connection established with %s.\r\n", MQTT_ENDPOINT );
        Log.Print( "Subscribed topic: %s\r\n", mqtt_obj.session.topic );
    }
    else
    {
        Log.Print( "MQTT is not subscribed.\r\n" );
    }
}

bool mqtt_isInit( void )
{
    return mqtt_obj.is_init && modem.Registered();
}

/**
 * @} Mqtt
 */

/**
 * @} Application
 */
