#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"
#include "mqtt_client.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MQTT_BROKER_URL "mqtt://10.42.0.1"

#define MQTT_TOPIC_STATUS "game/status"
#define MQTT_TOPIC_DISPLAY "game/display"
#define MQTT_TOPIC_SOUND "game/sound"

#define MQTT_TOPIC_BUTTON "base/button"

   /**
     * Callback function type for incoming MQTT messages
     * @param topic The topic the message was received on
     * @param payload The message payload (null-terminated)
     * @param payload_len Length of payload
     */
   typedef void (*mqtt_message_callback_t)(const char *topic, const char *payload, int payload_len);

   /**
     * Initialize MQTT client and start connection
     * @return ESP_OK on success
     */
   esp_err_t mqtt_manager_init(void);

   /**
     * Set callback for incoming MQTT messages
     * @param callback Function to call when message is received
     */
   void mqtt_set_message_callback(mqtt_message_callback_t callback);

   /**
     * Publish a button press event
     * @param player_id Player identifier (e.g., "meeple_1")
     * @param button Button number (1, 2, or 3)
     * @param timestamp_ms Timestamp in milliseconds
     * @return ESP_OK on success
     */
   esp_err_t mqtt_publish_button(const char *player_id, uint8_t button, int64_t timestamp_ms);

   /**
     * Publish ACK for display message
     */
   void mqtt_publish_ack(void);

   /**
     * Check if MQTT is connected
     * @return true if connected
     */
   bool mqtt_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif // MQTT_MANAGER_H