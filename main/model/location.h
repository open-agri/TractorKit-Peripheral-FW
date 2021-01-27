/**
 * @file location.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Location data.
 * @version 0.1
 * @date 2021-01-26
 *
 *
 */

#pragma once

#include <stdbool.h>
#include <time.h>

/**
 * @brief A struct for representing data related to GPS location.
 *
 */
typedef struct {

    double speed_kph;
    time_t epoch;
    bool gps_available;
} tk_location_data_t;
