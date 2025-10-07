/* GPIO Control Component Header
 * 
 * Component for managing GPIO configuration and operations
 */

#ifndef GPIO_CTRL_H
#define GPIO_CTRL_H

#include <stdint.h>
#include "driver/gpio.h"

// GPIO pin mapping (user index to physical GPIO)
#define GPIO_PIN_0  GPIO_NUM_18
#define GPIO_PIN_1  GPIO_NUM_19
#define GPIO_PIN_2  GPIO_NUM_21
#define GPIO_PIN_3  GPIO_NUM_22

// GPIO configuration values
#define GPIO_CONFIG_OUTPUT  0
#define GPIO_CONFIG_INPUT   1

// Number of managed GPIOs
#define NUM_GPIOS 4

/**
 * @brief Initialize GPIO control component
 * 
 * Initializes all GPIOs as inputs (default configuration)
 * 
 * @return 0 on success, -1 on error
 */
int gpio_ctrl_init(void);

/**
 * @brief Configure GPIO as input or output
 * 
 * @param num_bit GPIO index (0-3)
 * @param config 0 = output, 1 = input
 * @return Configured value on success, -1 on error
 */
int gpio_ctrl_config(uint8_t num_bit, uint8_t config);

/**
 * @brief Write value to GPIO
 * 
 * @param num_bit GPIO index (0-3)
 * @param value Value to write (0 or 1)
 * @return Current GPIO value on success, -1 on error
 */
int gpio_ctrl_write(uint8_t num_bit, uint8_t value);

/**
 * @brief Read GPIO value
 * 
 * @param num_bit GPIO index (0-3)
 * @return GPIO value (0 or 1) on success, -1 on error
 */
int gpio_ctrl_read(uint8_t num_bit);

/**
 * @brief Get GPIO configuration
 * 
 * @param num_bit GPIO index (0-3)
 * @return Configuration (0 = output, 1 = input), -1 on error
 */
int gpio_ctrl_get_config(uint8_t num_bit);

#endif // GPIO_CTRL_H