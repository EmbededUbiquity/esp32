# ESP32 Project CMakeLists

idf_component_register(SRCS "main.cpp" "lcd_manager.cpp" "wifi_manager.cpp" "mqtt_manager.cpp" "buzzer_manager.cpp" "button_manager.cpp" "led_manager.cpp"
                        REQUIRES driver esp-idf-lib__hd44780 nvs_flash esp_wifi esp_event esp_netif mqtt)
