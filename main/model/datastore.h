/**
 * @file datastore.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Data store for containing all the latest data.
 * @version 0.1
 * @date 2020-09-14
 *
 *
 */

#pragma once

#include <stdbool.h>

#include "engine.h"
#include "peripheral_settings.h"
#include "wifi_settings.h"
#include "location.h"

typedef struct {
  tk_engine_data_t engine_data;
  tk_location_data_t location_data;
  tk_peripheral_settings_t settings;
  tk_wifi_settings_t wifi_settings;
} tk_datastore_t;

tk_datastore_t global_datastore;