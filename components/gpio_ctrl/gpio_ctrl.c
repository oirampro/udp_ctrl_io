/* GPIO Control Component Implementation
 * 
 * Component for managing GPIO configuration and operations
 */

#include "gpio_ctrl.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "gpio_ctrl";

// GPIO state structure
typedef struct {
    uint8_t config;        // 0 = output, 1 = input
    gpio_num_t gpio_num;   // Physical GPIO number
} gpio_state_t;

// GPIO states array
static gpio_state_t gpio_states[NUM_GPIOS] = {
    {GPIO_CONFIG_INPUT, GPIO_PIN_0},
    {GPIO_CONFIG_INPUT, GPIO_PIN_1},
    {GPIO_CONFIG_INPUT, GPIO_PIN_2},
    {GPIO_CONFIG_INPUT, GPIO_PIN_3}
};

int gpio_ctrl_init(void)
{
    ESP_LOGI(TAG, "Initializing GPIO control");
    
    // Configure all GPIOs as inputs by default
    for (int i = 0; i < NUM_GPIOS; i++) {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << gpio_states[i].gpio_num);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        
        esp_err_t ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure GPIO %d", gpio_states[i].gpio_num);
            return -1;
        }
        
        gpio_states[i].config = GPIO_CONFIG_INPUT;
        ESP_LOGI(TAG, "GPIO index %d (GPIO%d) configured as input", i, gpio_states[i].gpio_num);
    }
    
    return 0;
}

int gpio_ctrl_config(uint8_t num_bit, uint8_t config)
{
    // Validate parameters
    if (num_bit >= NUM_GPIOS) {
        ESP_LOGE(TAG, "Invalid GPIO index: %d", num_bit);
        return -1;
    }
    
    if (config > 1) {
        ESP_LOGE(TAG, "Invalid config value: %d", config);
        return -1;
    }
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << gpio_states[num_bit].gpio_num);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    if (config == GPIO_CONFIG_OUTPUT) {
        io_conf.mode = GPIO_MODE_OUTPUT;
    } else {
        io_conf.mode = GPIO_MODE_INPUT;
    }
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO index %d", num_bit);
        return -1;
    }
    
    gpio_states[num_bit].config = config;
    ESP_LOGI(TAG, "GPIO index %d (GPIO%d) configured as %s", 
             num_bit, gpio_states[num_bit].gpio_num, 
             config == GPIO_CONFIG_OUTPUT ? "output" : "input");
    
    return config;
}

int gpio_ctrl_write(uint8_t num_bit, uint8_t value)
{
    // Validate parameters
    if (num_bit >= NUM_GPIOS) {
        ESP_LOGE(TAG, "Invalid GPIO index: %d", num_bit);
        return -1;
    }
    
    if (value > 1) {
        ESP_LOGE(TAG, "Invalid value: %d", value);
        return -1;
    }
    
    // Check if GPIO is configured as output
    if (gpio_states[num_bit].config != GPIO_CONFIG_OUTPUT) {
        ESP_LOGE(TAG, "GPIO index %d is not configured as output", num_bit);
        return -1;
    }
    
    // Write value to GPIO
    esp_err_t ret = gpio_set_level(gpio_states[num_bit].gpio_num, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write to GPIO index %d", num_bit);
        return -1;
    }
    
    ESP_LOGI(TAG, "GPIO index %d (GPIO%d) set to %d", num_bit, gpio_states[num_bit].gpio_num, value);
    
    // Return the actual GPIO value
    return gpio_get_level(gpio_states[num_bit].gpio_num);
}

int gpio_ctrl_read(uint8_t num_bit)
{
    // Validate parameters
    if (num_bit >= NUM_GPIOS) {
        ESP_LOGE(TAG, "Invalid GPIO index: %d", num_bit);
        return -1;
    }
    
    // Read GPIO value
    int value = gpio_get_level(gpio_states[num_bit].gpio_num);
    
    ESP_LOGI(TAG, "GPIO index %d (GPIO%d) read value: %d", num_bit, gpio_states[num_bit].gpio_num, value);
    
    return value;
}

int gpio_ctrl_get_config(uint8_t num_bit)
{
    // Validate parameters
    if (num_bit >= NUM_GPIOS) {
        ESP_LOGE(TAG, "Invalid GPIO index: %d", num_bit);
        return -1;
    }
    
    return gpio_states[num_bit].config;
}