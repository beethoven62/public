/**
 * @addtogroup Application Application
 * @{
 *
 * @file      mqtt.h
 * @brief     MQTT client module (public header).
 * @author    Johnas Cukier
 * @date      May 2023
 */

/**
 * @addtogroup Mqtt
 * @{
 */
#ifndef __MQTT_H__
#define __MQTT_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdint.h>
#include <stdbool.h>
#include "core_mqtt.h"
#include "transport_interface.h"
#include "os.h"
#include "dmm.h"
#include "app.h"
#include "log.h"
#include "twdt.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the MQTT task.
 */
typedef enum
{
    mqtt_send,
    mqtt_subscribe,
    mqtt_unsubscribe,
    mqtt_stat,
} mqtt_type_t;

typedef struct
{
    error_code_module_t ( *Init )( void );
    MQTTStatus_t ( *Subscribe )( char *topic );
    MQTTStatus_t ( *Unsubscribe )( void );
    MQTTStatus_t ( *Send )( char* topic, char *msg );
    void ( *Status )( void );
    bool ( *isInit )( void );
} const mqtt_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern mqtt_interface_t mqtt;

#endif /* __MQTT_H__ */

/**
 * @} Mqtt
 */

/**
 * @} Applicaton
 */
