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

#include "model/engine.h"

typedef struct {
#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
  tk_engine_data_t engine_data;
#endif

} tk_datastore_t;

tk_datastore_t global_datastore;