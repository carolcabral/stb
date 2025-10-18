#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "led_strip.h"

#define CONFIG_LED_STRIP_QTD 30
#define CONFIG_LED_STRIP_ROWS 6
#define CONFIG_LED_STRIP_COLS 5

static const uint8_t matrix[CONFIG_LED_STRIP_ROWS][CONFIG_LED_STRIP_COLS] = {
    {0, 11, 12, 23, 24},
    {1, 10, 13, 22, 25},
    {2, 9, 14, 21, 26},
    {3, 8, 15, 20, 27},
    {4, 7, 16, 19, 28},
    {5, 6, 17, 18, 29},
};

extern led_strip_handle_t led_strip; // Glogal handle for led strip

void configure_leds(void);

#endif