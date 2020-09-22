/**
 * @file main.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The app main.
 * @version 0.1
 * @date 2020-09-22
 *
 *
 */

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#include "drivers/LED/led_onboard.h"

#define TAG "Main"

void app_main(void) {
  esp_log_level_set("*", ESP_LOG_INFO);

  const esp_app_desc_t *desc = esp_ota_get_app_description();

  ESP_LOGI(
      TAG,
      "Peripheral boot. %s %s for ESP32 (compiled on %s %s) with ESP-IDF %s.",
      desc->project_name, desc->version, desc->date, desc->time, desc->idf_ver);

#ifdef CONFIG_ONBOARD_LED_ENABLE
  led_onboard_init();
  led_onboard_set_curve(led_onboard_curve_breathe, 6000);
  // led_onboard_set_curve(led_onboard_curve_flash_and_fade, 2000);
  // led_onboard_set_curve(led_onboard_curve_fast_flash, 2000);
#endif
}
