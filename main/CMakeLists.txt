idf_component_register(SRCS "animation.c" "timer.c" "blink_example_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_gptimer esp_driver_gpio)
