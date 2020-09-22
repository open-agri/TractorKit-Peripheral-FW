/**
 * @file led_onboard.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Manages the onboard LED.
 * @version 0.1
 * @date 2020-09-22
 *
 *
 */

#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include "sys/time.h"

#include "led_onboard.h"

#define TAG "Onboard LED"

static ledc_channel_config_t ch_config;
static led_onboard_mode_t mode_int;
TaskHandle_t curve_task_handle;

// Curve
static LEDCurveFunction current_curve;
static unsigned int current_period;

// PRIVATE FUNCTIONS
void __led_onboard_set_power_internal(double power) {
  uint32_t duty = power * 1023;
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch_config.channel, duty));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch_config.channel));
}

static double __led_curve_fast_flash(double progress) {
  if (progress < 0.25) {
    int state = progress / 0.025;
    if (state % 4 == 0)
      return 1;
    else
      return 0;
  }
  return 0;
}

static double __led_curve_flash_and_fade(double progress) {
  if (progress < 0.5)
    return pow(2 * progress - 1, 4);
  else
    return 0;
}

static double __led_curve_breathe(double progress) {
  return (exp(sin(2 * M_PI * progress)) / M_E - 0.13) * 0.7;
}

void LEDCurveTask(void *pvParameters) {
  while (true) {
    if (mode_int == LED_ONBOARD_MODE_CURVE) {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      unsigned long long milliseconds_epoch =
          (unsigned long long)(tv.tv_sec) * 1000 +
          (unsigned long long)(tv.tv_usec) / 1000;

      double progress =
          ((double)(milliseconds_epoch % current_period)) / current_period;
      __led_onboard_set_power_internal(current_curve(progress));
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// PUBLIC FUNCTIONS

/**
 * @brief Initializes the I/O and timers of the onboard LED.
 *
 */
void led_onboard_init(void) {

  ESP_LOGI(TAG, "Initializing driver.");

  ledc_timer_config_t timer_config = {.duty_resolution = LEDC_TIMER_10_BIT,
                                      .freq_hz = 8000,
                                      .speed_mode = LEDC_LOW_SPEED_MODE,
                                      .timer_num = LEDC_TIMER_0,
                                      .clk_cfg = LEDC_AUTO_CLK};

  ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

  ch_config = (ledc_channel_config_t){.channel = LEDC_CHANNEL_0,
                                      .duty = 0,
                                      .gpio_num = LED_ONBOARD_GPIO,
                                      .speed_mode = timer_config.speed_mode,
                                      .hpoint = 0,
                                      .timer_sel = timer_config.timer_num};

  ESP_ERROR_CHECK(ledc_channel_config(&ch_config));

  led_onboard_set_mode(LED_ONBOARD_MODE_OFF);

  // Curves
  led_onboard_curve_fast_flash = __led_curve_fast_flash;
  led_onboard_curve_flash_and_fade = __led_curve_flash_and_fade;
  led_onboard_curve_breathe = __led_curve_breathe;
  current_curve = led_onboard_curve_breathe;
  current_period = 7000;

  // Curve task
  xTaskCreate(LEDCurveTask, "Curve task", 4096, NULL, 1, &curve_task_handle);
  configASSERT(curve_task_handle);
  ESP_LOGI(TAG, "Default LED curves set successfully.");

  ESP_LOGI(TAG, "Driver initialized successfully.");
}

/**
 * @brief Sets the LED power. Mode will be set to manual.
 *
 * @param power The LED's power (0~1).
 */
void led_onboard_set_power(double power) {
  if (mode_int == LED_ONBOARD_MODE_OFF) {
    __led_onboard_set_power_internal(0);
    ESP_LOGW(TAG, "LED is set to off, can't update value. Set to manual or "
                  "curve before setting power again.");
    return;
  }

  led_onboard_set_mode(LED_ONBOARD_MODE_MANUAL);
  __led_onboard_set_power_internal(power);
}

/**
 * @brief Sets the onboard LED mode.
 *
 * @param mode The mode to use: off, manual, curve.
 */
void led_onboard_set_mode(led_onboard_mode_t mode) {
  switch (mode) {
  case LED_ONBOARD_MODE_OFF:
    ESP_LOGI(TAG, "Setting LED mode to off.");
    __led_onboard_set_power_internal(0);
    break;
  case LED_ONBOARD_MODE_MANUAL:
    ESP_LOGI(TAG, "Setting LED mode to manual.");
    break;
  case LED_ONBOARD_MODE_CURVE:
    ESP_LOGI(TAG, "Setting LED mode to curve.");
    break;
  }
  mode_int = mode;
}

led_onboard_mode_t led_onboard_get_mode(void) { return mode_int; }

/**
 * @brief Sets the onboard LED's curve and puts the LED in curve mode.
 *
 * @param curve
 * @param period
 */
void led_onboard_set_curve(LEDCurveFunction curve, unsigned int period) {
  current_curve = curve;
  current_period = period;
  ESP_LOGI(TAG, "Setting LED curve function to %p.", curve);
  led_onboard_set_mode(LED_ONBOARD_MODE_CURVE);
}