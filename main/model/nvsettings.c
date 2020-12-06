/**
 * @file nvsettings.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Non-volatile settings system
 * @version 0.1
 * @date 2020-11-26
 *
 *
 */

#include "nvsettings.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "datastore.h"
#include "drivers/RPM/pulse.h"

#define TAG "NV Settings"

nvs_handle_t nv_handle;

// -------------------- GENERAL FUNCTIONS --------------------
void nv_init() {

  ESP_LOGI(TAG, "Initializing NVS.");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "Opening per_settings namespace.");
  esp_err_t err = nvs_open("per_settings", NVS_READWRITE, &nv_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize NVS for peripheral settings.");
    ESP_ERROR_CHECK(err);
  }
}

void nv_load_peripheral_settings() {
  ESP_LOGI(TAG, "Loading all non-volatile peripheral settings.");

#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
  // Engine RPM coefficient
  global_datastore.settings.rpm_coeff = nv_get_rpm_coefficient();
#endif
}

void nv_load_apply_peripheral_settings() {
  ESP_LOGI(TAG, "Loading and applying all non-volatile peripheral settings.");
  nv_load_peripheral_settings();

#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
  // Engine RPM coefficient
  engine_rpm_set_coeff(global_datastore.settings.rpm_coeff);
  ESP_LOGI(TAG, "Engine RPM coefficient applied to %.2f.",
           global_datastore.settings.rpm_coeff);
#endif
}

// -------------------- SETTINGS --------------------

#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
// Engine RPM coefficient
void nv_set_rpm_coefficient(double coeff) {
  ESP_LOGI(TAG, "Writing and applying engine RPM coefficient setting to %.2f.",
           coeff);
  global_datastore.settings.rpm_coeff = coeff;

  // Save
  esp_err_t err = nvs_set_i32(nv_handle, "rpm_coeff", (int)(coeff * 1000000));

  // Apply
  engine_rpm_set_coeff(coeff);
}

double nv_get_rpm_coefficient() {
  int32_t coeff_int = 1000000;

  esp_err_t err = nvs_get_i32(nv_handle, "rpm_coeff", &coeff_int);
  assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
  if (err == ESP_ERR_NOT_FOUND) {
    ESP_LOGW(TAG,
             "RPM coefficient key not found. Returning default value of 1.");
  }

  ESP_LOGI(TAG, "RPM coefficient read from NVS: %.2f.",
           (double)coeff_int / 1000000);

  return (double)coeff_int / 1000000;
}
#endif