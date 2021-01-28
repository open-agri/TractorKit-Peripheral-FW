/**
 * @file engine.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Engine data model.
 * @version 0.1
 * @date 2020-09-07
 *
 *
 */

#pragma once

#include <stdbool.h>

/**
 * @brief A struct for representing data related to the engine.
 *
 */
typedef struct {

#if CONFIG_TK_ENGINE_RPM_ENABLE
  /**
   * @brief The engine's RPM.
   *
   */
  double rpm;
  bool rpm_available;
#endif

#if CONFIG_TK_ENGINE_THERM_ENABLE
  /**
   * @brief The engine's external temperature in Celsius degrees.
   *
   */
  float temp_c;
  bool temp_c_available;
#endif

} tk_engine_data_t;
