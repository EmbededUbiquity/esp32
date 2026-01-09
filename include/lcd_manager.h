#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

// LCD dimensions
#define LCD_COLS 16
#define LCD_ROWS 2

  /**
 * Initialize the LCD display
 * Must be called before any other LCD functions
 * 
 * @return ESP_OK on success, error code on failure
 */
  esp_err_t lcd_init(void);

  /**
 * Clear the LCD display
 */
  void lcd_clear(void);

  /**
 * Move cursor to home position (0,0)
 */
  void lcd_home(void);

  /**
 * Set cursor position
 * 
 * @param col Column (0-15)
 * @param row Row (0-1)
 */
  void lcd_set_cursor(uint8_t col, uint8_t row);

  /**
 * Print a string at the current cursor position
 * 
 * @param str String to print (null-terminated)
 */
  void lcd_print(const char *str);

  /**
 * Print a string at a specific position
 * 
 * @param col Column (0-15)
 * @param row Row (0-1)
 * @param str String to print (null-terminated)
 */
  void lcd_print_at(uint8_t col, uint8_t row, const char *str);

  /**
 * Print formatted text (like printf) at current cursor position
 * 
 * @param fmt Format string
 * @param ... Arguments
 */
  void lcd_printf(const char *fmt, ...);

  /**
 * Print formatted text at a specific position
 * 
 * @param col Column (0-15)
 * @param row Row (0-1)
 * @param fmt Format string
 * @param ... Arguments
 */
  void lcd_printf_at(uint8_t col, uint8_t row, const char *fmt, ...);

  /**
 * Display a message on both lines (convenience function)
 * 
 * @param line1 Text for first line (can be NULL)
 * @param line2 Text for second line (can be NULL)
 */
  void lcd_show_message(const char *line1, const char *line2);

  /**
 * Turn display on/off
 * 
 * @param on true to turn on, false to turn off
 */
  void lcd_display_on(bool on);

  /**
 * Show/hide cursor
 * 
 * @param show true to show cursor, false to hide
 */
  void lcd_cursor_on(bool show);

  /**
 * Enable/disable cursor blinking
 * 
 * @param blink true to enable blinking, false to disable
 */
  void lcd_blink_on(bool blink);

#ifdef __cplusplus
}
#endif

#endif // LCD_MANAGER_H
