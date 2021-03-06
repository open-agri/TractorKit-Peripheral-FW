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
#include "nvs_flash.h"
#include <stdio.h>

#include "drivers/LED/led_onboard.h"
#include "drivers/RPM/pulse.h"
#include "drivers/GPS/gps.h"
#include "drivers/thermo/thermo.h"

#include "model/datastore.h"
#include "model/nvsettings.h"

#include "BLE/ble.h"
#include "BLE/tk_uuid.h"

#define TAG "Main"

void app_main(void) {
  esp_log_level_set("*", ESP_LOG_INFO);

  const esp_app_desc_t *desc = esp_ota_get_app_description();

  ESP_LOGI(
      TAG,
      "Peripheral boot. %s %s for ESP32 (compiled on %s %s) with ESP-IDF %s.",
      desc->project_name, desc->version, desc->date, desc->time, desc->idf_ver);

  // Initialize settings
  nv_init();
  nv_load_peripheral_settings();

#if CONFIG_TK_ONBOARD_LED_ENABLE
  led_onboard_init();
  led_onboard_set_curve(led_onboard_curve_breathe, 6000);
  // led_onboard_set_curve(led_onboard_curve_flash_and_fade, 2000);
  // led_onboard_set_curve(led_onboard_curve_fast_flash, 2000);
#endif

  tk_ble_init();

#if CONFIG_TK_GPS_ENABLE
  tk_gps_init();
#endif

#if CONFIG_TK_ENGINE_THERM_ENABLE
  tk_thermo_init(&(global_datastore.engine_data.temp_c), &(tk_id_engine_temperature.u), &(tk_id_engine_temperature_ch_engine.u));
#endif

#if CONFIG_TK_ENGINE_RPM_ENABLE
  engine_rpm_pulse_init(&(global_datastore.engine_data.rpm),
                        &(global_datastore.engine_data.rpm_available),
                        global_datastore.settings.rpm_coeff);
#endif

  while (1) {
    vTaskDelay(100 / portTICK_RATE_MS);
    // ESP_LOGW(TAG, "RPM = %.4f%s.", global_datastore.engine_data.rpm,
    // global_datastore.engine_data.rpm_available ? "" : " [N/A]");
  }
}
