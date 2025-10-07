/* PWM Control Component Header
 * 
 * Component for managing PWM (LEDC) configuration and operations
 */

#ifndef PWM_CTRL_H
#define PWM_CTRL_H

#include <stdint.h>
#include <inttypes.h>

/**
 * @brief Initialize PWM control component
 * 
 * Initializes PWM with:
 * - Frequency: ~1 kHz
 * - Duty cycle: 0% (off)
 * - Resolution: 13 bits
 * 
 * @return 0 on success, -1 on error
 */
int pwm_ctrl_init(void);

/**
 * @brief Set PWM duty cycle percentage
 * 
 * @param percent Duty cycle percentage (0-100)
 * @return Set percentage on success, -1 on error
 */
int pwm_ctrl_set_duty(uint8_t percent);

/**
 * @brief Read current PWM duty cycle percentage
 * 
 * @return Current duty cycle percentage (0-100), -1 on error
 */
int pwm_ctrl_read_duty(void);

#endif // PWM_CTRL_H