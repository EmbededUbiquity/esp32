#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_timer.h"
#include "lcd_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "buzzer_manager.h"
#include "button_manager.h"
#include "led_manager.h"

static const char *TAG = "MAIN";

void perform_traffic_light_countdown(void);
void countdown_task_wrapper(void *pvParameters);

static TaskHandle_t s_countdown_task_handle = NULL;

//------------------------------------------------------------------------------
// JSON Handler
//------------------------------------------------------------------------------
static bool s_has_received_display = false;

void handle_display_message(const char *payload)
{
    s_has_received_display = true;

    cJSON *root = cJSON_Parse(payload);
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON Display Message");
        return;
    }

    cJSON *line1_item = cJSON_GetObjectItem(root, "line1");
    cJSON *line2_item = cJSON_GetObjectItem(root, "line2");

    if (line1_item || line2_item)
    {
        const char *l1 = (line1_item && cJSON_IsString(line1_item)) ? line1_item->valuestring : "";
        const char *l2 = (line2_item && cJSON_IsString(line2_item)) ? line2_item->valuestring : "";
        lcd_show_message(l1, l2);
    }
    else
    {
        cJSON *msg_item = cJSON_GetObjectItem(root, "message");
        if (!msg_item || !cJSON_IsString(msg_item))
        {
            msg_item = cJSON_GetObjectItem(root, "msg");
        }
        if (msg_item && cJSON_IsString(msg_item))
        {
            lcd_show_message(msg_item->valuestring, "");
        }
    }

    cJSON *btns_item = cJSON_GetObjectItem(root, "buttons");
    if (btns_item && cJSON_IsArray(btns_item))
    {
        uint8_t mask = BUTTON_MASK_NONE;
        int count = cJSON_GetArraySize(btns_item);
        for (int i = 0; i < count; i++)
        {
            cJSON *btn = cJSON_GetArrayItem(btns_item, i);
            if (btn && cJSON_IsNumber(btn))
            {
                int btn_id = btn->valueint;
                if (btn_id == 1)
                    mask |= BUTTON_MASK_1;
                else if (btn_id == 2)
                    mask |= BUTTON_MASK_2;
                else if (btn_id == 3)
                    mask |= BUTTON_MASK_3;
            }
        }
        button_set_active_mask((button_active_mask_t)mask);
    }

    cJSON_Delete(root);
    mqtt_publish_ack();
}

//------------------------------------------------------------------------------
// MQTT Callback
//------------------------------------------------------------------------------

static bool s_minigame_active = false;

//------------------------------------------------------------------------------
// MQTT Callback
//------------------------------------------------------------------------------
void on_mqtt_message(const char *topic, const char *payload, int len)
{
    ESP_LOGI(TAG, "MQTT Message: %s -> %s", topic, payload);

    if (strcmp(topic, MQTT_TOPIC_DISPLAY) == 0)
    {
        handle_display_message(payload);
    }
    else if (strcmp(topic, MQTT_TOPIC_SOUND) == 0)
    {
        if (strcmp(payload, "WIN") == 0)
            buzzer_play_game_finish();
        else if (strcmp(payload, "LOSE") == 0)
            buzzer_play_damage();
        else if (strcmp(payload, "ROLL") == 0)
            buzzer_play_dice_roll();
        else if (strcmp(payload, "START") == 0)
            buzzer_play_game_start();
        else if (strcmp(payload, "MOVE") == 0)
            buzzer_play_move();
        else if (strcmp(payload, "MOVE") == 0)
            buzzer_play_move();
        else if (strcmp(payload, "HEAL") == 0)
            buzzer_play_minigame_finish();
        else if (strcmp(payload, "ERROR") == 0)
            buzzer_play_error();
        else if (strcmp(payload, "DAMAGE") == 0)
            buzzer_play_damage();
        else if (strcmp(payload, "SIGNAL") == 0)
            buzzer_play_reaction_signal();
        else if (strcmp(payload, "MINIGAME_START") == 0)
        {
            if (s_countdown_task_handle == NULL)
            {
                xTaskCreate(countdown_task_wrapper, "countdown", 2048, NULL, 5, &s_countdown_task_handle);
            }
        }
        else
        {
            if (s_countdown_task_handle == NULL)
            {
                xTaskCreate(countdown_task_wrapper, "countdown", 2048, NULL, 5, &s_countdown_task_handle);
            }
        }
    }
    else if (strcmp(topic, MQTT_TOPIC_STATUS) == 0)
    {
        ESP_LOGI(TAG, "Game Status: %s", payload);

        if (strcmp(payload, "MINIGAME") == 0)
        {
            s_minigame_active = true;
            button_set_debounce_time(50);
            ESP_LOGI(TAG, "Minigame Mode: ON (Debounce 50ms, Sound Muted)");
        }
        else
        {
            if (s_minigame_active)
            {
                button_flush_queue();
                ESP_LOGI(TAG, "Exiting Minigame: Queue Flushed");
            }
            s_minigame_active = false;

            if (strcmp(payload, "WAITING") == 0)
            {
                ESP_LOGI(TAG, "Game Waiting - Playing Tune");
                button_set_debounce_time(200);
                buzzer_play_waiting();
            }
            else
            {
                button_set_debounce_time(200);
                ESP_LOGI(TAG, "Debounce set to 200ms (Standard)");
            }
        }
    }
}

//------------------------------------------------------------------------------
// Sequences
//------------------------------------------------------------------------------
#define SHOW_DEBUG_UI false

//------------------------------------------------------------------------------
// Sequences
//------------------------------------------------------------------------------
void perform_traffic_light_countdown(void)
{
    lcd_show_message("Get Ready...", "3");
    led_set_red(true);
    buzzer_tone(NOTE_C5, 100);
    vTaskDelay(pdMS_TO_TICKS(900));
    led_set_red(false);

    lcd_show_message("Get Ready...", "2");
    led_set_yellow(true);
    buzzer_tone(NOTE_C5, 100);
    vTaskDelay(pdMS_TO_TICKS(900));
    led_set_yellow(false);

    lcd_show_message("Get Ready...", "1");
    led_set_green(true);
    buzzer_tone(NOTE_C5, 100);
    vTaskDelay(pdMS_TO_TICKS(900));
    led_set_green(false);

    lcd_show_message("GO!", "");
    led_set_all(true, true, true);
    buzzer_tone(NOTE_C6, 500);
    led_set_all(false, false, false);
}

//------------------------------------------------------------------------------
// Non-blocking countdown task wrapper
//------------------------------------------------------------------------------
void countdown_task_wrapper(void *pvParameters)
{
    perform_traffic_light_countdown();
    s_countdown_task_handle = NULL;
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting Application...");

    lcd_init();
    buzzer_init();
    button_init();
    led_init();

    lcd_show_message("Connecting to", "WiFi...");
    wifi_init_sta();

    if (wifi_get_status() == WIFI_STATUS_CONNECTED)
    {
        char ip[16];
        wifi_get_ip(ip, sizeof(ip));
        lcd_show_message("WiFi Connected!", ip);
    }
    else
    {
        lcd_show_message("WiFi Error", "Offline Mode");
    }

    vTaskDelay(pdMS_TO_TICKS(1500));

    lcd_show_message("Connecting to", "MQTT Broker...");

    mqtt_set_message_callback(on_mqtt_message);
    mqtt_manager_init();

    int retries = 0;
    while (!mqtt_is_connected() && retries < 10)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        retries++;
    }

    if (mqtt_is_connected())
    {
        lcd_show_message("MQTT Connected!", "Ready...");
        buzzer_play_minigame_start();
    }
    else
    {
        lcd_show_message("MQTT Timeout", "Check Broker IP");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    lcd_show_message("Meeple's Gambit", "Press Button!");

    int64_t last_anim_time = 0;
    int anim_frame = 0;

    while (1)
    {
        if (mqtt_is_connected() && !s_has_received_display)
        {
            int64_t now = esp_timer_get_time() / 1000;
            if (now - last_anim_time > 500)
            {
                last_anim_time = now;
                anim_frame = (anim_frame + 1) % 4;
                const char *dots = (anim_frame == 0) ? "" : (anim_frame == 1) ? "."
                                                        : (anim_frame == 2)   ? ".."
                                                                              : "...";
                lcd_show_message("Meeple's Gambit", dots);
            }
        }
        else if (!mqtt_is_connected())
        {
            int64_t now = esp_timer_get_time() / 1000;
            if (now - last_anim_time > 500)
            {
                last_anim_time = now;
                anim_frame = (anim_frame + 1) % 4;
                const char *dots = (anim_frame == 0) ? "" : (anim_frame == 1) ? "."
                                                        : (anim_frame == 2)   ? ".."
                                                                              : "...";
                lcd_show_message("Reconnecting", dots);
            }
            s_has_received_display = false;
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        uint8_t btn;
        if (button_get_event(&btn, 100))
        {
            ESP_LOGI(TAG, "Button %d Pressed", btn);

            if (!s_minigame_active)
            {
                button_play_tone(btn);
            }

// 2. UI Feedback
#if SHOW_DEBUG_UI
            lcd_show_message("Button Pressed!", "Sending...");
#endif

            if (mqtt_is_connected())
            {
                const char *player_id = "unknown";
                if (btn == 1)
                    player_id = "meeple_1";
                else if (btn == 2)
                    player_id = "meeple_2";
                else if (btn == 3)
                    player_id = "meeple_3";

                int64_t ts = esp_timer_get_time() / 1000;

                esp_err_t res = mqtt_publish_button(player_id, btn, ts);
                if (res != ESP_OK)
                {
                    lcd_show_message("Send Failed", "Error");
                }
            }
            else
            {
                lcd_show_message("Offline", "Not Sent");
            }
        }
    }
}
