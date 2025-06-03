#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "interrupts.h"
#include "animations.h"
#include "timer.h"

#define GPIO_LOVE_MASK 255
#define GPIO_COLOR_MASK 254
#define GPIO_WHITE_MASK 253
#define GPIO_DEFAULT_MASK GPIO_COLOR_MASK

QueueHandle_t evt_queue = NULL;
static uint8_t mask = GPIO_DEFAULT_MASK;

static void get_interrupt_task(void *arg)
{
    interrupt_source_t it_source;

    for (;;)
    {
        if (xQueueReceive(evt_queue, &it_source, portMAX_DELAY))
        {
            printf("Queue received \n");

            if (it_source.is_ble)
            {
                mask = it_source.mask_value;

                printf("BLE\n");
                start_timer();
                switch (mask)
                {
                case GPIO_LOVE_MASK:
                    global_current_animation = I_LOVE_START;
                    break;
                case GPIO_COLOR_MASK:
                    global_current_animation = COLORFUL_START;
                    break;
                case GPIO_WHITE_MASK:
                    global_current_animation = WHITE_START;
                    break;

                default:
                    global_current_animation = mask % ANIMATIONS_MAX;
                    stop_timer(); // Disable timer
                    break;
                }

                clear_timer();

                // printf("Going to %s!\n", animations_name[global_current_animation]);
                // printf("GPIO[%ld] intr, val: %d | %s \n", io_num, mask, animations_name[global_current_animation]);
            }

            if (it_source.is_timer)
            {
                printf("TIMER! %d\n", mask);
                global_current_animation = (global_current_animation + 1) % ANIMATIONS_MAX;

                switch (mask)
                {
                case GPIO_LOVE_MASK:
                    printf("Love mask!\n");
                    if (global_current_animation == I_LOVE_END)
                        global_current_animation = I_LOVE_START; // Loop do amor
                    break;

                case GPIO_WHITE_MASK:
                    printf("White mask!\n");

                    if (global_current_animation == WHITE_END)
                        global_current_animation = WHITE_START;
                    break;

                case GPIO_COLOR_MASK:
                    printf("Color mask!\n");

                    if (global_current_animation == COLORFUL_END)
                        global_current_animation = COLORFUL_START;
                    break;
                default:
                    break;
                }
            }

            printf("Going to %s!\n\n", animations_name[global_current_animation]);
            // gpio_intr_enable(it_source.gpio_number);
        }
    }
}

void configure_interrupts()
{
    // create a queue to handle gpio event from isr
    evt_queue = xQueueCreate(1, sizeof(interrupt_source_t));
    if (!evt_queue)
    {
        ESP_LOGE(__FUNCTION__, "Creating queue failed");
        return;
    }

    // start gpio task
    xTaskCreate(get_interrupt_task, "get_interrupt_task", 2048, NULL, 10, NULL);
}