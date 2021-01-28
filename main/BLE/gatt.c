/**
 * @file gatt.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Generic Attribute server.
 * @version 0.1
 * @date 2020-09-26
 *
 *
 */

#include "ble.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "esp_ota_ops.h"
#include "esp_system.h"

#include "OTA/server.h"
#include "OTA/wifi.h"
#include "model/datastore.h"
#include "model/nvsettings.h"
#include "tk_uuid.h"

#define TAG "GATT"

static int tk_gatt_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service: Device information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &tk_id_device_info.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /*** Characteristic: Manufacturer name string */
                    .uuid = &tk_id_device_info_mfr_name_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: Model number string */
                    .uuid = &tk_id_device_info_model_number_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: Serial number string */
                    .uuid = &tk_id_device_info_sn_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: HW rev string */
                    .uuid = &tk_id_device_info_hw_rev_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: FW rev string */
                    .uuid = &tk_id_device_info_fw_rev_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: SW rev string */
                    .uuid = &tk_id_device_info_sw_rev_string.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    0, /* No more characteristics in this service. */
                },
            },
    },
    {
        /*** Service: OTA */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &tk_id_common_ota.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /*** Characteristic: Enable OTA access point */
                    .uuid = &tk_id_common_ota_ch_enable.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                },
                {
                    /*** Characteristic: OTA SSID */
                    .uuid = &tk_id_common_ota_ch_ssid.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: OTA password */
                    .uuid = &tk_id_common_ota_ch_password.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: Update URL */
                    .uuid = &tk_id_common_ota_ch_update_url.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    /*** Characteristic: Update progress */
                    .uuid = &tk_id_common_ota_ch_progress.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
    {
        /*** Service: Engine RPM */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &tk_id_engine_rpm.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /*** Characteristic: Engine RPM */
                    .uuid = &tk_id_engine_rpm_ch_rpm.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Engine RPM availability */
                    .uuid = &tk_id_engine_rpm_ch_rpm_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    /*** Characteristic: Engine RPM coefficient */
                    .uuid = &tk_id_engine_rpm_ch_coeff.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
#endif
#ifdef CONFIG_TK_ENGINE_THERM_ENABLE
    {
        /*** Service: Engine temperature */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &tk_id_engine_temperature.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /*** Characteristic: Engine temperature */
                    .uuid = &tk_id_engine_temperature_ch_engine.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Engine temperature availability */
                    .uuid = &tk_id_engine_temperature_ch_engine_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    /*** Characteristic: Air temperature */
                    .uuid = &tk_id_engine_temperature_ch_air.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Air temperature availability */
                    .uuid = &tk_id_engine_temperature_ch_air_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
#endif
#ifdef CONFIG_TK_GPS_ENABLE
    {
        /*** Service: Location */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &tk_id_location.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /*** Characteristic: Speed (km/h) */
                    .uuid = &tk_id_location_ch_speed_kph.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Timestamp */
                    .uuid = &tk_id_location_ch_timestamp.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ,

                },
                {
                    /*** Characteristic: GPS availability */
                    .uuid = &tk_id_location_ch_gps_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
#endif
    {
        0, /* No more services. */
    },
};

static int tk_gatt_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                         void *dst, uint16_t *len) {
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len) {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0) {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

static int tk_gatt_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  const ble_uuid_t *uuid;
  int rc;

  uuid = ctxt->chr->uuid;

  /* Determine which characteristic is being accessed by examining its UUID.
   */

  // ---------- Device information ----------
  // Manufacturer name string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_mfr_name_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    const char mfr[] = "OpenAgri";

    rc = os_mbuf_append(ctxt->om, mfr, sizeof mfr);

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Model number string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_model_number_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    const char name[] = CONFIG_TK_DEVICE_NAME;

    rc = os_mbuf_append(ctxt->om, name, sizeof name);

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // S/N string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_sn_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    uint8_t mac[6];
    char sn[13];
    esp_read_mac(mac, ESP_MAC_BT);
    sprintf(sn, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3],
            mac[4], mac[5]);

    rc = os_mbuf_append(ctxt->om, sn, sizeof sn);

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // HW rev string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_hw_rev_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    const char rev[] = "1.0";

    rc = os_mbuf_append(ctxt->om, rev, sizeof(char) * strlen(rev));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // FW rev string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_fw_rev_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    const esp_app_desc_t *desc = esp_ota_get_app_description();

    char rev[36];
    sprintf(rev, "IDF %s", desc->idf_ver);

    rc = os_mbuf_append(ctxt->om, rev, sizeof(char) * strlen(rev));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // SW rev string
  if (ble_uuid_cmp(uuid, &tk_id_device_info_sw_rev_string.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    const esp_app_desc_t *desc = esp_ota_get_app_description();

    char rev[32];
    sprintf(rev, "%s", desc->version);

    rc = os_mbuf_append(ctxt->om, rev, sizeof(char) * strlen(rev));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // ---------- OTA ----------

  // Enable OTA access point
  if (ble_uuid_cmp(uuid, &tk_id_common_ota_ch_enable.u) == 0) {
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
      rc = os_mbuf_append(ctxt->om, &(global_datastore.wifi_settings.ap_enable),
                          sizeof global_datastore.wifi_settings.ap_enable);
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:;

      bool val;
      rc = tk_gatt_write(ctxt->om, sizeof val, sizeof val, &val, NULL);

      if (val != global_datastore.wifi_settings.ap_enable) {
        if (val)
          wifi_init_softap(&OTA_server);
        else
          wifi_stop_softap(NULL);
      }

      return rc;

    default:
      assert(0);
      return BLE_ATT_ERR_UNLIKELY;
    }
  }

  // OTA SSID
  if (ble_uuid_cmp(uuid, &tk_id_common_ota_ch_ssid.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, global_datastore.wifi_settings.ssid,
                        sizeof(char) *
                            strlen(global_datastore.wifi_settings.ssid));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // OTA password
  if (ble_uuid_cmp(uuid, &tk_id_common_ota_ch_password.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, global_datastore.wifi_settings.password,
                        sizeof(char) *
                            strlen(global_datastore.wifi_settings.password));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // ---------- Engine data ----------

#ifdef CONFIG_TK_ENGINE_THERM_ENABLE
  // Engine temperature
  if (ble_uuid_cmp(uuid, &tk_id_engine_temperature_ch_engine.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.engine_data.temp_c),
                        sizeof global_datastore.engine_data.temp_c);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
#endif

#ifdef CONFIG_TK_ENGINE_RPM_ENABLE
  // Engine RPM
  if (ble_uuid_cmp(uuid, &tk_id_engine_rpm_ch_rpm.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.engine_data.rpm),
                        sizeof global_datastore.engine_data.rpm);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Engine RPM availability
  if (ble_uuid_cmp(uuid, &tk_id_engine_rpm_ch_rpm_avail.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    rc = os_mbuf_append(ctxt->om, &(global_datastore.engine_data.rpm_available),
                        sizeof global_datastore.engine_data.rpm_available);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Engine RPM coefficient
  if (ble_uuid_cmp(uuid, &tk_id_engine_rpm_ch_coeff.u) == 0) {
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
      rc = os_mbuf_append(ctxt->om, &(global_datastore.settings.rpm_coeff),
                          sizeof global_datastore.settings.rpm_coeff);
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:;

      double val;
      rc = tk_gatt_write(ctxt->om, sizeof val, sizeof val, &val, NULL);

      nv_set_rpm_coefficient(val);

      return rc;

    default:
      assert(0);
      return BLE_ATT_ERR_UNLIKELY;
    }
  }
#endif

  // ---------- Location data ----------

#ifdef CONFIG_TK_GPS_ENABLE
  // Speed
  if (ble_uuid_cmp(uuid, &tk_id_location_ch_speed_kph.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.location_data.speed_kph),
                        sizeof global_datastore.location_data.speed_kph);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Epoch
  if (ble_uuid_cmp(uuid, &tk_id_location_ch_timestamp.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.location_data.epoch),
                        sizeof global_datastore.location_data.epoch);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // GPS availability
  if (ble_uuid_cmp(uuid, &tk_id_location_ch_gps_avail.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.location_data.gps_available),
                        sizeof global_datastore.location_data.gps_available);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
#endif

  // if (ble_uuid_cmp(uuid, &gatt_svr_chr_sec_test_static_uuid.u) == 0) {
  //   switch (ctxt->op) {
  //   case BLE_GATT_ACCESS_OP_READ_CHR:
  //     rc = os_mbuf_append(ctxt->om, &gatt_svr_sec_test_static_val,
  //                         sizeof gatt_svr_sec_test_static_val);
  //     return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

  //   case BLE_GATT_ACCESS_OP_WRITE_CHR:
  //     rc = gatt_svr_chr_write(ctxt->om, sizeof gatt_svr_sec_test_static_val,
  //                             sizeof gatt_svr_sec_test_static_val,
  //                             &gatt_svr_sec_test_static_val, NULL);
  //     return rc;

  //   default:
  //     assert(0);
  //     return BLE_ATT_ERR_UNLIKELY;
  //   }
  // }

  // Undefined chr
  return BLE_ATT_ERR_UNLIKELY;
}

void tk_gatt_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
  char buf[BLE_UUID_STR_LEN];

  switch (ctxt->op) {
  case BLE_GATT_REGISTER_OP_SVC:
    ESP_LOGI(TAG, "Registered service %s with handle=%d",
             ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
    break;

  case BLE_GATT_REGISTER_OP_CHR:
    ESP_LOGI(TAG,
             "Registering characteristic %s with "
             "def_handle=%d val_handle=%d",
             ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
             ctxt->chr.def_handle, ctxt->chr.val_handle);
    break;

  case BLE_GATT_REGISTER_OP_DSC:
    ESP_LOGI(TAG, "Registering descriptor %s with handle=%d",
             ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf), ctxt->dsc.handle);
    break;

  default:
    assert(0);
    break;
  }
}

/**
 * @brief Initializes the GATT server.
 *
 * @return int 0 on success, error code otherwise.
 */
int tk_gatt_init(void) {

  int rc;

  ble_svc_gap_init(); // NO: We want a dynamic name!
  ble_svc_gatt_init();

  rc = ble_gatts_count_cfg(gatt_svr_svcs);
  if (rc) {
    ESP_LOGE(TAG, "Services array contains an invalid definition (error %d).",
             rc);
    return rc;
  }

  rc = ble_gatts_add_svcs(gatt_svr_svcs);
  if (rc) {
    ESP_LOGE(TAG,
             "Unable to add GATT services. Might be out of memory (error %d).",
             rc);
    return rc;
  }

  return 0;
}