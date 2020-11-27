/**
 * @file nvsettings.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Non-volatile settings system
 * @version 0.1
 * @date 2020-11-26
 * 
 * 
 */

void nv_init();

void nv_load_peripheral_settings();

void nv_load_apply_peripheral_settings();

// SETTINGS
void nv_set_rpm_coefficient(double coeff);
double nv_get_rpm_coefficient();