/**
 * @file main.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The app main.
 * @version 0.1
 * @date 2020-09-22
 * 
 * 
 */

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

void app_main(void) {
  esp_log_level_set("*", ESP_LOG_INFO);

  const esp_app_desc_t *desc = esp_ota_get_app_description();

  ESP_LOGI(
      TAG,
      "Peripheral boot. %s %s for ESP32 (compiled on %s %s) with ESP-IDF %s.",
      desc->project_name, desc->version, desc->date, desc->time, desc->idf_ver);
}
