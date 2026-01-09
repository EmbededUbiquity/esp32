#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "mqtt_manager";
static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool is_connected = false;
static mqtt_message_callback_t s_message_callback = NULL;

/**
 * Subscribe to game topics
 */
static void subscribe_to_topics(void)
{
    if (mqtt_client == NULL)
        return;

    esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_STATUS, 1);
    esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_DISPLAY, 1);
    esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_SOUND, 1);

    ESP_LOGI(TAG, "Subscribed to game topics: %s, %s, %s",
             MQTT_TOPIC_STATUS, MQTT_TOPIC_DISPLAY, MQTT_TOPIC_SOUND);
}

/**
 * Used to handle MQTT events
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected to broker");
        is_connected = true;
        esp_mqtt_client_publish(mqtt_client, "game/connection", "CONNECTED", 0, 1, 1);
        subscribe_to_topics();
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT disconnected from broker");
        is_connected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Subscribed, msg_id=%d", event->msg_id);
        break;

        if (event->topic_len > 0 && event->data_len > 0)
        {
            char topic[64] = {0};
            char payload[256] = {0};

            int topic_len = event->topic_len < 63 ? event->topic_len : 63;
            int data_len = event->data_len < 255 ? event->data_len : 255;

            strncpy(topic, event->topic, topic_len);
            strncpy(payload, event->data, data_len);

            ESP_LOGI(TAG, "Received: topic=%s, payload=%s", topic, payload);

            if (s_message_callback != NULL)
            {
                int actual_len = strlen(payload);
                s_message_callback(topic, payload, actual_len);
            }
        }
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "Message published, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error occurred");
        break;

    default:
        break;
    }
}

/**
 * Initialize MQTT client and start connection
 * @return ESP_OK on success
 */
esp_err_t mqtt_manager_init(void)
{
    // Boiler plate MQTT configuration
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = MQTT_BROKER_URL;

    // LWT Configuration
    mqtt_cfg.session.last_will.topic = "game/connection";
    mqtt_cfg.session.last_will.msg = "DISCONNECTED";
    mqtt_cfg.session.last_will.qos = 1;
    mqtt_cfg.session.last_will.retain = 0;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "MQTT client initialized and started");
    return ESP_OK;
}

/**
 * Set callback for incoming MQTT messages
 */
void mqtt_set_message_callback(mqtt_message_callback_t callback)
{
    s_message_callback = callback;
}

/**
 * Publish a button press event
 */
esp_err_t mqtt_publish_button(const char *player_id, uint8_t button, int64_t timestamp_ms)
{
    if (!is_connected)
    {
        ESP_LOGW(TAG, "MQTT not connected, cannot publish button event");
        return ESP_ERR_INVALID_STATE;
    }

    if (player_id == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // Format JSON payload
    char payload[128];
    // Payload: {"player":"<id>","button":<id>,"timestamp":<ts>}
    snprintf(payload, sizeof(payload),
             "{\"player\":\"%s\",\"button\":%d,\"timestamp\":%lld}",
             player_id, button, timestamp_ms);

    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_BUTTON, payload, 0, 0, 0);
    if (msg_id >= 0)
    {
        ESP_LOGI(TAG, "Published button event: %s", payload);
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to publish button event");
        return ESP_FAIL;
    }
}

/**
 * Publish ACK for display message
 */
void mqtt_publish_ack(void)
{
    if (mqtt_client && is_connected)
    {
        esp_mqtt_client_publish(mqtt_client, "game/ack", "OK", 0, 1, 0);
    }
}

/**
 * Check if MQTT is connected
 * @return true if connected
 */
bool mqtt_is_connected(void)
{
    return is_connected;
}