/**
 * @file pulse.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Hall/pulse sensor driver.
 * @version 0.1
 * @date 2020-09-22
 *
 *
 */

#pragma once

#if CONFIG_TK_ENGINE_RPM_ENABLE

#define RPM_PULSE_GPIO CONFIG_TK_ENGINE_RPM_GPIO
#define RPM_PULSE_DEB CONFIG_TK_ENGINE_RPM_DEBOUNCE_US        // us
#define RPM_MAX_PERIOD CONFIG_TK_ENGINE_RPM_MAX_PERIOD * 1000 // us

void engine_rpm_pulse_init(double *output, bool *output_available,
                           double coeff);
                           
void engine_rpm_set_coeff(double coeff);

#endif