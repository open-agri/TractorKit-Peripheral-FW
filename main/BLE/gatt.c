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

#include "model/datastore.h"
#include "tk_uuid.h"

#define TAG "GATT"

static int tk_gatt_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
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
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Engine RPM availability */
                    .uuid = &tk_id_engine_rpm_ch_rpm_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    /*** Characteristic: Engine RPM coefficient */
                    .uuid = &tk_id_engine_rpm_ch_coeff.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,

                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
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
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Engine temperature availability */
                    .uuid = &tk_id_engine_temperature_ch_engine_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    /*** Characteristic: Air temperature */
                    .uuid = &tk_id_engine_temperature_ch_air.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    /*** Characteristic: Air temperature availability */
                    .uuid = &tk_id_engine_temperature_ch_air_avail.u,
                    .access_cb = tk_gatt_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_NOTIFY,

                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },
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

  /* Determine which characteristic is being accessed by examining its
   * 128-bit UUID.
   */

  // Engine RPM
  if (ble_uuid_cmp(uuid, &tk_id_engine_rpm_ch_rpm.u) == 0) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

    rc = os_mbuf_append(ctxt->om, &(global_datastore.engine_data.rpm),
                        sizeof global_datastore.engine_data.rpm);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

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