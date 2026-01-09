#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// LED Pins
#define LED_PIN_RED 25
#define LED_PIN_YELLOW 26
#define LED_PIN_GREEN 27

    /**
 * Initialize LED GPIOs
 * @return ESP_OK on success
 */
    esp_err_t led_init(void);

    /**
 * Set Red LED state
 * @param on true=on, false=off
 */
    void led_set_red(bool on);

    /**
 * Set Yellow LED state
 * @param on true=on, false=off
 */
    void led_set_yellow(bool on);

    /**
 * Set Green LED state
 * @param on true=on, false=off
 */
    void led_set_green(bool on);

    /**
 * Set all LEDs at once
 */
    void led_set_all(bool red, bool yellow, bool green);

#ifdef __cplusplus
}
#endif

#endif // LED_MANAGER_H
