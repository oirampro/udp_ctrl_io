idf_component_register(SRCS "pwm_ctrl.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_gpio esp_driver_ledc)
