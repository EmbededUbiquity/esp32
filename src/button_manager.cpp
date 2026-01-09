#include "button_manager.h"
#include "buzzer_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "BUTTON";

typedef struct
{
    uint8_t pin;
    uint32_t tick;
} button_event_t;

static QueueHandle_t s_button_queue = NULL;

static uint32_t s_debounce_ticks = pdMS_TO_TICKS(200);
static uint32_t s_last_press_tick_1 = 0;
static uint32_t s_last_press_tick_2 = 0;
static uint32_t s_last_press_tick_3 = 0;

static const uint32_t ISR_DEBOUNCE_TICKS = pdMS_TO_TICKS(50);
static uint32_t s_last_isr_tick_1 = 0;
static uint32_t s_last_isr_tick_2 = 0;
static uint32_t s_last_isr_tick_3 = 0;

static uint8_t s_button_mask = 0xFF;

static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    uint32_t now = xTaskGetTickCountFromISR();

    uint32_t *last_isr_tick = NULL;
    if (gpio_num == BUTTON1_PIN)
    {
        last_isr_tick = &s_last_isr_tick_1;
    }
    else if (gpio_num == BUTTON2_PIN)
    {
        last_isr_tick = &s_last_isr_tick_2;
    }
    else if (gpio_num == BUTTON3_PIN)
    {
        last_isr_tick = &s_last_isr_tick_3;
    }

    if (last_isr_tick != NULL)
    {
        if ((now - *last_isr_tick) < ISR_DEBOUNCE_TICKS)
        {
            return;
        }
        *last_isr_tick = now;
    }

    button_event_t evt;
    evt.pin = (uint8_t)gpio_num;
    evt.tick = now;

    xQueueSendFromISR(s_button_queue, &evt, NULL);
}

static esp_err_t configure_button_gpio(gpio_num_t pin)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK)
        return err;

    return gpio_isr_handler_add(pin, button_isr_handler, (void *)pin);
}

esp_err_t button_init(void)
{
    ESP_LOGI(TAG, "Initializing Buttons...");

    s_button_queue = xQueueCreate(20, sizeof(button_event_t));
    if (s_button_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create queue");
        return ESP_FAIL;
    }

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    // Configure Buttons
    ESP_ERROR_CHECK(configure_button_gpio(BUTTON1_PIN));
    ESP_ERROR_CHECK(configure_button_gpio(BUTTON2_PIN));
    ESP_ERROR_CHECK(configure_button_gpio(BUTTON3_PIN));

    ESP_LOGI(TAG, "Buttons Initialized on GPIO %d, %d, %d", BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN);
    return ESP_OK;
}

bool button_get_event(uint8_t *button_out, uint32_t wait_ms)
{
    button_event_t evt;
    TickType_t wait_ticks = (wait_ms == 0) ? 0 : pdMS_TO_TICKS(wait_ms);
    if (wait_ms > 0 && wait_ticks == 0)
        wait_ticks = 1;

    if (xQueueReceive(s_button_queue, &evt, wait_ticks) == pdTRUE)
    {
        uint32_t *last_tick_ptr = NULL;
        uint8_t btn_id = 0;

        if (evt.pin == BUTTON1_PIN)
        {
            last_tick_ptr = &s_last_press_tick_1;
            btn_id = 1;
        }
        else if (evt.pin == BUTTON2_PIN)
        {
            last_tick_ptr = &s_last_press_tick_2;
            btn_id = 2;
        }
        else if (evt.pin == BUTTON3_PIN)
        {
            last_tick_ptr = &s_last_press_tick_3;
            btn_id = 3;
        }
        else
        {
            return false;
        }

        ESP_LOGI(TAG, "RAW Event: ID=%d, Tick=%lu", btn_id, (unsigned long)evt.tick);

        // Check Mask
        if (!((s_button_mask >> (btn_id - 1)) & 0x01))
        {
            ESP_LOGW(TAG, "Button %d ignored (Mask: 0x%02X)", btn_id, s_button_mask);
            return false;
        }

        // Debounce Check
        if ((evt.tick - *last_tick_ptr) > s_debounce_ticks)
        {
            *last_tick_ptr = evt.tick;
            if (button_out)
                *button_out = btn_id;
            return true;
        }
        else
        {
            ESP_LOGD(TAG, "Button %d debounce bounce (Delta: %lu, Threshold: %lu)",
                     btn_id, (unsigned long)(evt.tick - *last_tick_ptr), (unsigned long)s_debounce_ticks);
            // Bounce detected - ignore this event and retry immediately
            return button_get_event(button_out, 0);
        }
    }

    return false;
}

void button_play_tone(uint8_t btn)
{
    switch (btn)
    {
    case 1:
        buzzer_play_tone_player_1();
        break;
    case 2:
        buzzer_play_tone_player_2();
        break;
    case 3:
        buzzer_play_tone_player_3();
        break;
    default:
        // ignore
        break;
    }
}

void button_set_active_mask(button_active_mask_t mask)
{
    s_button_mask = (uint8_t)mask;
    ESP_LOGI(TAG, "Button mask updated to: 0x%02X", s_button_mask);
}

void button_set_debounce_time(uint32_t debounce_ms)
{
    s_debounce_ticks = pdMS_TO_TICKS(debounce_ms);
    ESP_LOGI(TAG, "Debounce set to %lu ms", debounce_ms);
}

void button_flush_queue(void)
{
    button_event_t evt;
    // Consume all pending events with 0 wait
    while (xQueueReceive(s_button_queue, &evt, 0) == pdTRUE)
    {
        // Discard
    }
    ESP_LOGI(TAG, "Button Queue Flushed");
}
