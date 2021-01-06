/**
 * @file pulse.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Hall/pulse sensor driver.
 * @version 0.1
 * @date 2020-09-22
 *
 *
 */

#include "driver/gpio.h"
#include "sys/time.h"

#include "esp_log.h"
#include "esp_timer.h"

#include "BLE/tk_uuid.h"
#include "host/ble_hs.h"

#include "pulse.h"

#define TAG "RPM pulse"

#define GPIO_BIT_MASK (1ULL << RPM_PULSE_GPIO)

esp_timer_handle_t deb_timer;
esp_timer_handle_t period_limiter_timer;
esp_timer_handle_t gatt_refresh_timer;

static double current_coeff;
static int64_t prev_micros = 0;
bool *data_available;

// GATT handles
uint16_t rpm_chr_val_handle = 0;
uint16_t rpm_avail_chr_val_handle = 0;

/**
 * @brief The usual exponential rolling average for some smoothing.
 *
 * @param avg The current average.
 * @param new_sample The new input.
 * @return double The new average.
 */
double exp_roll_avg(double avg, double new_sample) {
  avg -= avg / 4;
  avg += new_sample / 4;

  return avg;
}

/**
 * @brief The pulse interrupt handler.
 *
 * @param arg Unused
 */
IRAM_ATTR void engine_rpm_pulse_handler(void *arg) {
  esp_timer_stop(deb_timer); // Don't check for error
  ESP_ERROR_CHECK(esp_timer_start_once(deb_timer, RPM_PULSE_DEB));
}

/**
 * @brief The debounced pulse handler.
 *
 * @param arg Pointer to double output.
 */
void engine_rpm_pulse_after_deb(void *arg) {
  if (gpio_get_level(RPM_PULSE_GPIO))
    return;

  int64_t micros = esp_timer_get_time();
  int64_t delta = micros - prev_micros;
  prev_micros = micros;

  double rpm = (1000000.0 / (double)delta) * 60 * current_coeff;
  double avg = *(double *)(arg);

  // Set passed output pointer to value
  avg = exp_roll_avg(avg, rpm);
  *(double *)(arg) = avg;

  // Reset limiting timer
  esp_timer_stop(period_limiter_timer); // Don't check for errors
  ESP_ERROR_CHECK(esp_timer_start_once(period_limiter_timer, RPM_MAX_PERIOD));

  // Reset flag
  *data_available = true;
}

/**
 * @brief Invalidate output data if the period limiting timer expires.
 *
 * @param arg The output RPM value.
 */
void engine_rpm_max_period_reached(void *arg) {
  *(double *)(arg) = 0;
  *data_available = false;

  // TODO: Update availability chr on GATT
}

void gatt_refresh_timer_cb(void *arg) {
  if (!rpm_chr_val_handle || !rpm_avail_chr_val_handle)
    engine_rpm_update_gatt_handles();

  // Notify
  if (rpm_chr_val_handle) {
    ble_gatts_chr_updated(rpm_chr_val_handle);
  }

  // TODO!
}

/**
 * @brief Initializes the RPM pulse driver.
 *
 * @param output The output pointer (double *).
 * @param coeff The pulse to RPM coefficient.
 */
void engine_rpm_pulse_init(double *output, bool *output_available,
                           double coeff) {

  ESP_LOGI(TAG, "Initializing driver.");

  // Boolean flag for N/A
  data_available = output_available;

  // Debounce timer
  esp_timer_create_args_t deb_timer_args = {.callback =
                                                engine_rpm_pulse_after_deb,
                                            .name = "engine_rpm_debounce",
                                            .arg = (void *)output};

  ESP_ERROR_CHECK(esp_timer_create(&deb_timer_args, &deb_timer));

  // Period limit timer
  esp_timer_create_args_t period_limiter_timer_args = {
      .callback = engine_rpm_max_period_reached,
      .name = "engine_rpm_period_limiter",
      .arg = (void *)output};

  ESP_ERROR_CHECK(
      esp_timer_create(&period_limiter_timer_args, &period_limiter_timer));

  // Start timer
  ESP_ERROR_CHECK(esp_timer_start_once(period_limiter_timer, RPM_MAX_PERIOD));

  // Handle refresh timer
  esp_timer_create_args_t gatt_refresh_timer_args = {
      .callback = gatt_refresh_timer_cb,
      .name = "engine_rpm_gatt_refresh",
      .arg = NULL};

  ESP_ERROR_CHECK(esp_timer_create(&gatt_refresh_timer_args,
                                   &gatt_refresh_timer));

  // Start timer (30ms)
  ESP_ERROR_CHECK(
      esp_timer_start_periodic(gatt_refresh_timer, 30 * 1000));

  // Prevent first reading error
  prev_micros = esp_timer_get_time();
  engine_rpm_set_coeff(coeff);

  // Set up GPIO
  gpio_config_t conf = {.intr_type = GPIO_INTR_NEGEDGE,
                        .mode = GPIO_MODE_DEF_INPUT,
                        .pin_bit_mask = GPIO_BIT_MASK,
                        .pull_down_en = 0,
                        .pull_up_en = 1};

  gpio_config(&conf);
  gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
  gpio_isr_handler_add(RPM_PULSE_GPIO, engine_rpm_pulse_handler,
                       (void *)output);

  ESP_LOGI(TAG, "Driver initialized on pin %u.", RPM_PULSE_GPIO);
}

void engine_rpm_set_coeff(double coeff) {
  ESP_LOGI(TAG, "Setting coefficient to %.2f.", coeff);
  current_coeff = coeff;
}

/**
 * @brief Updates the GATT value handles.
 *
 */
void engine_rpm_update_gatt_handles() {

  ESP_LOGI(TAG, "Updating GATT value handles.");

  int rc;

  // Get RPM handle
  rc = ble_gatts_find_chr(&tk_id_engine_rpm, &tk_id_engine_rpm_ch_rpm,
                          NULL, &rpm_chr_val_handle);

  if (rc) {
    ESP_LOGW(TAG,
             "Unable to get GATT RPM characteristic value handle (error %x).",
             rc);
    rpm_chr_val_handle = 0;
  }

  // Get RPM availability handle
  rc = ble_gatts_find_chr(&tk_id_engine_rpm, &tk_id_engine_rpm_ch_rpm_avail,
                          NULL, &rpm_avail_chr_val_handle);

  if (rc) {
    ESP_LOGW(TAG,
             "Unable to get GATT RPM availability characteristic value handle "
             "(error %x).",
             rc);
    rpm_avail_chr_val_handle = 0;
  }
}
