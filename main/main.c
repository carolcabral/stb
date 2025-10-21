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
#include "storage.h"

#include <string.h>
void app_main(void)
{
    init_nvs();

    // char *message = "Hello from ESP!";
    // init_spiffs();
    // read_file("/spiffs/teste.txt");
    // write_file("/spiffs/teste.txt", message, strlen(message));

    // read_file("/spiffs/teste.txt");

    // close_spiffs();

    configure_interrupts();

    configure_ble();

    configure_timer(&water_timer);

    configure_leds();

    printf("Minimum free heap size: %lu bytes\n", esp_get_minimum_free_heap_size());
}
