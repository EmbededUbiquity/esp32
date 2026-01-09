#include "led_manager.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "LED";

static void configure_led_pin(int pin)
{
    gpio_reset_pin((gpio_num_t)pin);
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)pin, 0);
}

extern "C"
{

    esp_err_t led_init(void)
    {
        ESP_LOGI(TAG, "Initializing LEDs (R:%d, Y:%d, G:%d)",
                 LED_PIN_RED, LED_PIN_YELLOW, LED_PIN_GREEN);

        configure_led_pin(LED_PIN_RED);
        configure_led_pin(LED_PIN_YELLOW);
        configure_led_pin(LED_PIN_GREEN);

        return ESP_OK;
    }

    void led_set_red(bool on)
    {
        gpio_set_level((gpio_num_t)LED_PIN_RED, on ? 1 : 0);
    }

    void led_set_yellow(bool on)
    {
        gpio_set_level((gpio_num_t)LED_PIN_YELLOW, on ? 1 : 0);
    }

    void led_set_green(bool on)
    {
        gpio_set_level((gpio_num_t)LED_PIN_GREEN, on ? 1 : 0);
    }

    void led_set_all(bool red, bool yellow, bool green)
    {
        led_set_red(red);
        led_set_yellow(yellow);
        led_set_green(green);
    }

} // extern "C"
