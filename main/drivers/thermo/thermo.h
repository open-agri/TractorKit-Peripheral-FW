/**
 * @file thermo.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Thermometer manager.
 * @version 0.1
 * @date 2021-01-28
 * 
 * 
 */

#include "host/ble_hs.h"

void tk_thermo_init(double *output, ble_uuid_t *svc_uuid, ble_uuid_t *chr_uuid);