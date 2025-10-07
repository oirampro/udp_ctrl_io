/* ADC Control Component Header
 * 
 * Component for managing ADC (Analog to Digital Converter) operations
 */

#ifndef ADC_CTRL_H
#define ADC_CTRL_H

#include <stdint.h>

/**
 * @brief Initialize ADC control component
 * 
 * Initializes ADC1 Channel 0 (GPIO36) with:
 * - Unit: ADC_UNIT_1
 * - Channel: ADC_CHANNEL_0 (GPIO36)
 * - Attenuation: ADC_ATTEN_DB_11 (0-3.3V range)
 * - Bitwidth: 12 bits (default)
 * - Calibration enabled if supported
 * 
 * @return 0 on success, -1 on error
 */
int adc_ctrl_init(void);

/**
 * @brief Read ADC raw value
 * 
 * @return ADC raw value (0-4095 for 12-bit) on success, -1 on error
 */
int adc_ctrl_read(void);

/**
 * @brief Deinitialize ADC control component
 * 
 * Cleanup ADC resources
 */
void adc_ctrl_deinit(void);

#endif // ADC_CTRL_H