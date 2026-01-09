#include "lcd_manager.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "hd44780.h"
#include "esp_log.h"

static const char *TAG = "LCD";

// LCD Pin definitions
#define LCD_RS GPIO_NUM_19
#define LCD_E GPIO_NUM_18
#define LCD_D4 GPIO_NUM_17
#define LCD_D5 GPIO_NUM_16
#define LCD_D6 GPIO_NUM_4
#define LCD_D7 GPIO_NUM_21

// LCD state
static hd44780_t lcd_dev;
static bool lcd_initialized = false;
static bool display_on = true;
static bool cursor_on = false;
static bool blink_on = false;

// Update display control settings
static void update_display_control(void)
{
    if (lcd_initialized)
    {
        hd44780_control(&lcd_dev, display_on, cursor_on, blink_on);
    }
}

esp_err_t lcd_init(void)
{
    if (lcd_initialized)
    {
        ESP_LOGW(TAG, "LCD already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing LCD...");
    ESP_LOGI(TAG, "  RS=GPIO%d, E=GPIO%d", LCD_RS, LCD_E);
    ESP_LOGI(TAG, "  D4=GPIO%d, D5=GPIO%d, D6=GPIO%d, D7=GPIO%d",
             LCD_D4, LCD_D5, LCD_D6, LCD_D7);

    // Configure LCD structure
    memset(&lcd_dev, 0, sizeof(lcd_dev));
    lcd_dev.write_cb = NULL; // Use GPIO mode, not I2C
    lcd_dev.pins.rs = LCD_RS;
    lcd_dev.pins.e = LCD_E;
    lcd_dev.pins.d4 = LCD_D4;
    lcd_dev.pins.d5 = LCD_D5;
    lcd_dev.pins.d6 = LCD_D6;
    lcd_dev.pins.d7 = LCD_D7;
    lcd_dev.pins.bl = HD44780_NOT_USED; // No GPIO backlight control
    lcd_dev.font = HD44780_FONT_5X8;
    lcd_dev.lines = LCD_ROWS;
    lcd_dev.backlight = true;

    esp_err_t err = hd44780_init(&lcd_dev);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize LCD: %s", esp_err_to_name(err));
        return err;
    }

    lcd_initialized = true;

    // Set default display state: on, no cursor, no blink
    display_on = true;
    cursor_on = false;
    blink_on = false;
    update_display_control();

    ESP_LOGI(TAG, "LCD initialized successfully");
    return ESP_OK;
}

void lcd_clear(void)
{
    if (!lcd_initialized)
        return;
    hd44780_clear(&lcd_dev);
}

void lcd_home(void)
{
    if (!lcd_initialized)
        return;
    hd44780_gotoxy(&lcd_dev, 0, 0);
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
    if (!lcd_initialized)
        return;
    if (col >= LCD_COLS)
        col = LCD_COLS - 1;
    if (row >= LCD_ROWS)
        row = LCD_ROWS - 1;
    hd44780_gotoxy(&lcd_dev, col, row);
}

void lcd_print(const char *str)
{
    if (!lcd_initialized || !str)
        return;
    hd44780_puts(&lcd_dev, str);
}

void lcd_print_at(uint8_t col, uint8_t row, const char *str)
{
    lcd_set_cursor(col, row);
    lcd_print(str);
}

void lcd_printf(const char *fmt, ...)
{
    if (!lcd_initialized || !fmt)
        return;

    char buffer[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    lcd_print(buffer);
}

void lcd_printf_at(uint8_t col, uint8_t row, const char *fmt, ...)
{
    if (!lcd_initialized || !fmt)
        return;

    char buffer[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    lcd_print_at(col, row, buffer);
}

void lcd_show_message(const char *line1, const char *line2)
{
    lcd_clear();
    if (line1)
    {
        lcd_print_at(0, 0, line1);
    }
    if (line2)
    {
        lcd_print_at(0, 1, line2);
    }
}

void lcd_display_on(bool on)
{
    display_on = on;
    update_display_control();
}

void lcd_cursor_on(bool show)
{
    cursor_on = show;
    update_display_control();
}

void lcd_blink_on(bool blink)
{
    blink_on = blink;
    update_display_control();
}
