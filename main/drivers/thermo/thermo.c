/**
 * @file thermo.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Thermometer manager.
 * @version 0.1
 * @date 2021-01-28
 *
 *
 */

#include <math.h>
#include <stdbool.h>

#include "MLX90614_SMBus_Driver.h"
#include "MLX90614_API.h"
#include "esp_timer.h"
#include "host/ble_hs.h"

#define TAG "Thermo"

esp_timer_handle_t gatt_refresh_timer;
uint16_t thermo_chr_val_handle;

void gatt_thermo_refresh_timer_cb(void *arg) {
  float *output = (float *)arg;
  float reading;
  if (!MLX90614_GetTo(0x5A, &reading)) {
    // Success
    ESP_LOGI(TAG, "Read temperature: %.2f ºC", reading);
  } else {
    // Failure
    reading = 0.0/0.0;
    ESP_LOGW(TAG, "Error while reading from MLX90614.");
  }

  // Indicate change
  if (reading != *output) {
    *output = reading;
    if (thermo_chr_val_handle != 0)
      ble_gatts_chr_updated(thermo_chr_val_handle);
  }
}

void tk_thermo_init(float *output, ble_uuid_t *svc_uuid, ble_uuid_t *chr_uuid) {

  ESP_LOGI(TAG, "Initializing driver.");

  // IIC driver
  MLX90614_SMBusInit(CONFIG_TK_ENGINE_THERM_SDA_PIN,
                     CONFIG_TK_ENGINE_THERM_SCL_PIN,
                     50000); // sda scl and 50kHz

  // Find chr handle
  int rc = ble_gatts_find_chr(svc_uuid, chr_uuid, NULL, &thermo_chr_val_handle);

  if (rc) {
    ESP_LOGW(TAG,
             "Unable to get GATT thermometer characteristic value handle "
             "(error %x).",
             rc);
    thermo_chr_val_handle = 0;

    ESP_LOGI(TAG, "Driver setup failure.");
    return;
  }

  // Create timer
  esp_timer_create_args_t gatt_refresh_timer_args = {
      .callback = gatt_thermo_refresh_timer_cb,
      .name = "thermo_gatt_refresh",
      .arg = (void *)output};

  ESP_ERROR_CHECK(
      esp_timer_create(&gatt_refresh_timer_args, &gatt_refresh_timer));

  // Start timer (100ms)
  ESP_ERROR_CHECK(esp_timer_start_periodic(gatt_refresh_timer, 100 * 1000));
}