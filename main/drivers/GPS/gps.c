/**
 * @file gps.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief UART NMEA GPS driver.
 * @version 0.1
 * @date 2020-12-15
 *
 *
 */

#define TAG "GPS"

#include <stdio.h>
#include <time.h>

#include "BLE/tk_uuid.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "model/datastore.h"
#include "nmea_parser.h"

#define UART_NUM UART_NUM_1

// GATT handles
uint16_t spd_chr_val_handle = 0;
uint16_t gps_avail_chr_val_handle = 0;

void get_gps_gatt_chr_handles() {

  ESP_LOGI(TAG, "Updating GATT value handles.");

  int rc;

  // Get speed handle
  rc = ble_gatts_find_chr(&tk_id_location, &tk_id_location_ch_speed_kph,
                          NULL, &spd_chr_val_handle);

  if (rc) {
    ESP_LOGW(TAG,
             "Unable to get GATT speed characteristic value handle (error %x).",
             rc);
    spd_chr_val_handle = 0;
  }

  // Get GPS availability handle
  rc = ble_gatts_find_chr(&tk_id_location, &tk_id_location_ch_gps_avail, NULL,
                          &gps_avail_chr_val_handle);

  if (rc) {
    ESP_LOGW(TAG,
             "Unable to get GATT GPS availability characteristic value handle "
             "(error %x).",
             rc);
    gps_avail_chr_val_handle = 0;
  }
}

void notify_gps_gatt_chrs() {
  if (spd_chr_val_handle == 0 || gps_avail_chr_val_handle == 0) {
    ESP_LOGI(TAG, "Getting GPS GATT characteristic handles.");
    get_gps_gatt_chr_handles();
  }

  if (spd_chr_val_handle != 0 && gps_avail_chr_val_handle != 0) {
    ESP_LOGI(TAG, "Indicating characteristic value change.");
    ble_gatts_chr_updated(spd_chr_val_handle);
    ble_gatts_chr_updated(gps_avail_chr_val_handle);
  }
}

static void gps_event_handler(void *event_handler_arg,
                              esp_event_base_t event_base, int32_t event_id,
                              void *event_data) {
  gps_t *gps = NULL;
  switch (event_id) {
  case GPS_UPDATE:
    gps = (gps_t *)event_data;
    /* print information parsed from GPS statements */
    ESP_LOGI(TAG,
             "%d/%d/%d %d:%d:%d => \r\n"
             "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
             "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
             "\t\t\t\t\t\taltitude   = %.02fm\r\n"
             "\t\t\t\t\t\tspeed      = %fm/s",
             gps->date.year, gps->date.month, gps->date.day, gps->tim.hour,
             gps->tim.minute, gps->tim.second, gps->latitude, gps->longitude,
             gps->altitude, gps->speed);

    // Handle time update
    struct tm tm;
    char timestamp[60];
    sprintf(timestamp, "%d-%d-%d %d:%d:%d", gps->date.year, gps->date.month,
            gps->date.day, gps->tim.hour, gps->tim.minute, gps->tim.second);
    if (strptime(timestamp, "%y-%m-%d %H:%M:%S", &tm) != NULL) {
      global_datastore.location_data.epoch = mktime(&tm);
      ESP_LOGI(TAG, "Epoch: %ld.", global_datastore.location_data.epoch);
    } else {
      ESP_LOGW(TAG, "Unable to parse time data.");
    }

    // Speed update
    global_datastore.location_data.speed_kph = (gps->speed) * 3.6;

    // Availability
    global_datastore.location_data.gps_available =
        (gps->fix != GPS_FIX_INVALID);

    // Note: Time is not notified, it's read only.
    notify_gps_gatt_chrs();

    break;
  case GPS_UNKNOWN:
    /* print unknown statements */
    ESP_LOGW(TAG, "Unknown statement: \"%s\".", (char *)event_data);
    break;
  default:
    break;
  }
}

void tk_gps_init() {
  ESP_LOGI(TAG, "Initializing GPS.");
  /* NMEA parser configuration */
  nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
  /* init NMEA parser library */
  nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
  /* register event handler for NMEA parser library */
  nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);
}