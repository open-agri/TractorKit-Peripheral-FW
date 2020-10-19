/**
 * @file tk_uuid.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Contains default UUIDs for various services/characteristics.
 * @version 0.1
 * @date 2020-09-19
 *
 *
 */

#pragma once

#include "host/ble_uuid.h"

/* ----- Device information service ----- */
// 180A
static const ble_uuid16_t tk_id_device_info = BLE_UUID16_INIT(0x180A);

// 2A29
static const ble_uuid16_t tk_id_device_info_mfr_name_string = BLE_UUID16_INIT(0x2A29);

// 2A24
static const ble_uuid16_t tk_id_device_info_model_number_string = BLE_UUID16_INIT(0x2A24);

// 2A25
static const ble_uuid16_t tk_id_device_info_sn_string = BLE_UUID16_INIT(0x2A25);

// 2A27
static const ble_uuid16_t tk_id_device_info_hw_rev_string = BLE_UUID16_INIT(0x2A27);

// 2A26
static const ble_uuid16_t tk_id_device_info_fw_rev_string = BLE_UUID16_INIT(0x2A26);

// 2A28
static const ble_uuid16_t tk_id_device_info_sw_rev_string = BLE_UUID16_INIT(0x2A28);

/* ----- Engine RPM service ----- */

// 5AAA2412-111F-2400-0AA1-13025D240000
static const ble_uuid128_t tk_id_engine_rpm =
    BLE_UUID128_INIT(0x00, 0x00, 0x24, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D240001
static const ble_uuid128_t tk_id_engine_rpm_ch_rpm =
    BLE_UUID128_INIT(0x01, 0x00, 0x24, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D240002
static const ble_uuid128_t tk_id_engine_rpm_ch_rpm_avail =
    BLE_UUID128_INIT(0x02, 0x00, 0x24, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D240003
static const ble_uuid128_t tk_id_engine_rpm_ch_coeff =
    BLE_UUID128_INIT(0x03, 0x00, 0x24, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

/* ----- Engine temperature service ----- */

// 5AAA2412-111F-2400-0AA1-13025D250000
static const ble_uuid128_t tk_id_engine_temperature =
    BLE_UUID128_INIT(0x00, 0x00, 0x25, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D250001
static const ble_uuid128_t tk_id_engine_temperature_ch_engine =
    BLE_UUID128_INIT(0x01, 0x00, 0x25, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D250002
static const ble_uuid128_t tk_id_engine_temperature_ch_engine_avail =
    BLE_UUID128_INIT(0x02, 0x00, 0x25, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D250003
static const ble_uuid128_t tk_id_engine_temperature_ch_air =
    BLE_UUID128_INIT(0x03, 0x00, 0x25, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);

// 5AAA2412-111F-2400-0AA1-13025D250004
static const ble_uuid128_t tk_id_engine_temperature_ch_air_avail =
    BLE_UUID128_INIT(0x04, 0x00, 0x25, 0x5d, 0x02, 0x13, 0xa1, 0x0a, 0x00, 0x24,
                     0x1f, 0x11, 0x12, 0x24, 0xaa, 0x5a);
