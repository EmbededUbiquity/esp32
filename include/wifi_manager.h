#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WIFI_SSID "Pixel_8983"
#define WIFI_PASS "#12345678"
#define MAXIMUM_RETRY 5

  // WiFi connection status
  typedef enum
  {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_FAILED
  } wifi_status_t;

  /**
 * Initialize and connect to WiFi
 * @return ESP_OK on success
 */
  esp_err_t wifi_init_sta(void);

  /**
 * Get current WiFi connection status
 * @return Current WiFi status
 */
  wifi_status_t wifi_get_status(void);

  /**
 * Disconnect from WiFi
 * @return ESP_OK on success
 */
  esp_err_t wifi_disconnect(void);

  /**
 * Reconnect to WiFi
 * @return ESP_OK on success
 */
  esp_err_t wifi_reconnect(void);

  /**
     * Get the assigned IP address as a string
     * @param buffer Buffer to store the IP string
     * @param len Length of the buffer
     * @return ESP_OK on success
     */
  esp_err_t wifi_get_ip(char *buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif
