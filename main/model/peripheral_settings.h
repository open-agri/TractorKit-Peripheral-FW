/**
 * @file peripheral_settings.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Settings that can be set via BLE for peripherals.
 * @version 0.1
 * @date 2020-09-23
 * 
 * 
 */

#include <stdbool.h>

typedef struct {
    #ifdef CONFIG_TK_ENGINE_RPM_ENABLE
        double rpm_coeff;
        unsigned int 
    #endif
} tk_peripheral_settings_t;