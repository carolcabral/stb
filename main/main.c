#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "sdkconfig.h"
#include "nvs_flash.h"

#include "interrupts.h"
#include "bleprph.h"
#include "timer.h"
#include "lights.h"

void app_main(void)
{
    // init_nvs();

    configure_interrupts();

    configure_ble();

    configure_timer(&water_timer);

    configure_leds();

    printf("Minimum free heap size: %lu bytes\n", esp_get_minimum_free_heap_size());
}
