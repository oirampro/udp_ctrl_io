/* ADC Control Component Implementation
 * 
 * Component for managing ADC (Analog to Digital Converter) operations
 */

#include "adc_ctrl.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "adc_ctrl";

// ADC Configuration
#define ADC_UNIT            ADC_UNIT_1
#define ADC_CHANNEL         ADC_CHANNEL_0      // ADC1 channel 0 -> GPIO36 on ESP32
#define ADC_ATTEN           ADC_ATTEN_DB_11    // Range for 0..3.3V
#define ADC_BITWIDTH        ADC_BITWIDTH_DEFAULT

// ADC handles
static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_cali_handle_t cali_handle = NULL;
static bool do_calibration = false;

// Calibration functions prototypes
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

int adc_ctrl_init(void)
{
    ESP_LOGI(TAG, "Initializing ADC control");
    
    // Create ADC oneshot unit
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    
    esp_err_t ret = adc_oneshot_new_unit(&init_cfg, &adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create ADC unit: %s", esp_err_to_name(ret));
        return -1;
    }
    
    // Configure ADC channel
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTEN,
    };
    
    ret = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &chan_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC channel: %s", esp_err_to_name(ret));
        adc_oneshot_del_unit(adc_handle);
        return -1;
    }
    
    // Initialize calibration (if supported)
    do_calibration = adc_calibration_init(ADC_UNIT, ADC_CHANNEL, ADC_ATTEN, &cali_handle);
    if (do_calibration) {
        ESP_LOGI(TAG, "ADC calibration enabled");
    } else {
        ESP_LOGW(TAG, "ADC calibration not available");
    }
    
    ESP_LOGI(TAG, "ADC initialized: Unit=%d, Channel=%d, GPIO36", ADC_UNIT, ADC_CHANNEL);
    
    return 0;
}

int adc_ctrl_read(void)
{
    if (adc_handle == NULL) {
        ESP_LOGE(TAG, "ADC not initialized");
        return -1;
    }
    
    int raw = 0;
    esp_err_t ret = adc_oneshot_read(adc_handle, ADC_CHANNEL, &raw);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(ret));
        return -1;
    }
    
    ESP_LOGI(TAG, "ADC raw value: %d (0x%03X)", raw, raw);
    
    return raw;
}

void adc_ctrl_deinit(void)
{
    if (adc_handle != NULL) {
        adc_oneshot_del_unit(adc_handle);
        adc_handle = NULL;
    }
    
    if (do_calibration && cali_handle != NULL) {
        adc_calibration_deinit(cali_handle);
        cali_handle = NULL;
    }
    
    ESP_LOGI(TAG, "ADC deinitialized");
}

/* ---------- Calibration functions (adapted from official example) ---------- */
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        adc_cali_curve_fitting_config_t cfg = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cfg, &handle);
        if (ret == ESP_OK) calibrated = true;
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        adc_cali_line_fitting_config_t cfg = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cfg, &handle);
        if (ret == ESP_OK) calibrated = true;
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt or scheme not supported, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Calibration init failed (no memory or invalid arg)");
    }
    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_delete_scheme_curve_fitting(handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_delete_scheme_line_fitting(handle);
#endif
}