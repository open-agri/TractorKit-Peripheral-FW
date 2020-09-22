/**
 * @file led_onboard.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Manages the onboard LED.
 * @version 0.1
 * @date 2020-09-22
 * 
 * 
 */

// #define CONFIG_ONBOARD_LED_ENABLE 1

#ifdef CONFIG_ONBOARD_LED_ENABLE

#pragma once

#define LED_ONBOARD_GPIO CONFIG_ONBOARD_LED_GPIO

typedef enum {
    LED_ONBOARD_MODE_OFF,
    LED_ONBOARD_MODE_MANUAL,
    LED_ONBOARD_MODE_CURVE
} led_onboard_mode_t;

/**
 * @brief A function that generates an output value for the LED, given its progress.
 * Domain: [0, 1] -> [0, 1]
 * 
 */
typedef double (*LEDCurveFunction)(double progress);

LEDCurveFunction led_onboard_curve_breathe;          // Sleep mode
LEDCurveFunction led_onboard_curve_flash_and_fade;   // Connected
LEDCurveFunction led_onboard_curve_fast_flash;       // Awake, not connected

/**
 * @brief Initializes the I/O and timers of the onboard LED.
 * 
 */
void led_onboard_init(void);

/**
 * @brief Sets the LED power. Mode will be set to manual.
 * 
 * @param power The LED's power (0~1).
 */
void led_onboard_set_power(double power);

/**
 * @brief Sets the onboard LED mode.
 * 
 * @param mode The mode to use: off, manual, curve.
 */
void led_onboard_set_mode(led_onboard_mode_t mode);

led_onboard_mode_t led_onboard_get_mode(void);

/**
 * @brief Sets the onboard LED's curve and puts the LED in curve mode.
 * 
 * @param curve 
 * @param period 
 */
void led_onboard_set_curve(LEDCurveFunction curve, unsigned int period);

#endif