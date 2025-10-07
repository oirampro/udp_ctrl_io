/* PWM Control Component Implementation
 * 
 * Component for managing PWM (LEDC) configuration and operations
 */

#include "pwm_ctrl.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "pwm_ctrl";

// PWM Configuration
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5)                    // GPIO5 for PWM output
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT      // 13-bit resolution
#define LEDC_FREQUENCY          (1000)                  // 1 kHz frequency
#define LEDC_MAX_DUTY           ((1 << LEDC_DUTY_RES) - 1)  // Maximum duty value (8191)

// Current duty cycle percentage
static uint8_t current_duty_percent = 0;

int pwm_ctrl_init(void)
{
    ESP_LOGI(TAG, "Initializing PWM control");
    
    // Prepare and apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer");
        return -1;
    }
    
    // Prepare and apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,    // Start with 0% duty cycle
        .hpoint         = 0
    };
    
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel");
        return -1;
    }
    
    current_duty_percent = 0;
    
    ESP_LOGI(TAG, "PWM initialized: frequency=%d Hz, duty=0%%", LEDC_FREQUENCY);
    
    return 0;
}

int pwm_ctrl_set_duty(uint8_t percent)
{
    // Validate percentage
    if (percent > 100) {
        ESP_LOGE(TAG, "Invalid duty cycle percentage: %d", percent);
        return -1;
    }
    
    // Calculate duty value from percentage
    // duty = (percent * max_duty) / 100
    uint32_t duty = (percent * LEDC_MAX_DUTY) / 100;
    
    // Set duty cycle
    esp_err_t ret = ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty cycle");
        return -1;
    }
    
    // Update duty to apply the new value
    ret = ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty cycle");
        return -1;
    }
    
    current_duty_percent = percent;
    
    ESP_LOGI(TAG, "PWM duty cycle set to %d%% (duty value: %lu)", percent, duty);
    
    return percent;
}

int pwm_ctrl_read_duty(void)
{
    ESP_LOGI(TAG, "PWM duty cycle read: %d%%", current_duty_percent);
    return current_duty_percent;
}