#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "esp_err.h"
#include "driver/gpio.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Button Pins
#define BUTTON1_PIN GPIO_NUM_22
#define BUTTON2_PIN GPIO_NUM_23
#define BUTTON3_PIN GPIO_NUM_32

  /**
 * Initialize button GPIOs and interrupts
 * @return ESP_OK on success
 */
  esp_err_t button_init(void);

  /**
 * Check if a button was pressed and retrieve the event.
 * Should be called in a loop.
 * 
 * @param button_out Pointer to store the pressed button number (1, 2, or 3)
 * @param wait_ms Time to wait for a button press in milliseconds
 * @return true if a button press was detected
 */
  bool button_get_event(uint8_t *button_out, uint32_t wait_ms);

  /**
 * Play the tone associated with a specific button
 * @param btn Button ID (1, 2, 3)
 */
  void button_play_tone(uint8_t btn);

  /**
   * Button Mask Enum
   */
  typedef enum
  {
    BUTTON_MASK_NONE = 0,
    BUTTON_MASK_1 = (1 << 0),
    BUTTON_MASK_2 = (1 << 1),
    BUTTON_MASK_3 = (1 << 2),
    BUTTON_MASK_ALL = 0xFF
  } button_active_mask_t;

  /**
 * Set the mask of active buttons
 * @param mask Bitmask of active buttons (Use button_active_mask_t values)
 */
  void button_set_active_mask(button_active_mask_t mask);

  /**
   * Set debounce time in milliseconds
   * @param debounce_ms Debounce time (default 200ms)
   */
  void button_set_debounce_time(uint32_t debounce_ms);

  /**
   * Flush all pending events from the button queue
   */
  void button_flush_queue(void);

#ifdef __cplusplus
}
#endif

#endif // BUTTON_MANAGER_H
