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

#include "pulse.h"

#define TAG "RPM pulse"

#define GPIO_BIT_MASK (1ULL << RPM_PULSE_GPIO)

esp_timer_handle_t deb_timer;
esp_timer_handle_t period_limiter_timer;

static double current_coeff;
static int64_t prev_micros = 0;
bool *data_available;

/**
 * @brief The usual exponential rolling average for some smoothing.
 *
 * @param avg The current average.
 * @param new_sample The new input.
 * @return double The new average.
 */
double exp_roll_avg(double avg, double new_sample) {
  avg -= avg / 10;
  avg += new_sample / 10;

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
  *(double*)(arg) = 0;
  *data_available = false;
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
