#include <stdio.h>
#include "esp_random.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "interrupts.h"
#include "animations.h"
#include "timer.h"

QueueHandle_t evt_queue = NULL;
extern TaskHandle_t animations_task_handle, love_task_handle;

static void get_interrupt_task(void *arg)
{
    interrupt_ctx_t it;

    uint32_t ulNotifiedValue = 0;
    while (1)
    {
        if (xQueueReceive(evt_queue, &it, portMAX_DELAY))
        {
            ESP_LOGW("", "Queue received! Source %s", (it.source == IT_SOURCE_TIMER) ? "TIMER" : "BLE");

            if (it.source == IT_SOURCE_TIMER)
            {
                ulNotifiedValue = REQUEST_ANIM_WATER;

                /* // If multiple timer events:
                gptimer_handle_t source_timer = *(gptimer_handle_t *)it.mask_value;
                printf("Queue received! Source: %d Value %p\n,", it.source, source_timer);
                if (source_timer == water_timer.handle) {}
                */
            }
            /* Can choose one of the general animations or chose any special ones */
            if (it.source == IT_SOURCE_BLE)
                ulNotifiedValue = (uint32_t)*(uint8_t *)it.mask_value;

            if (xTaskNotify(animations_task_handle, ulNotifiedValue, eSetBits) != pdPASS)
                ESP_LOGE(__FUNCTION__, "Unable to send notification!\n");
        }
    }
}
void configure_interrupts()
{
    /* Create a queue to handle events from:
     *     o GPIO event from ISR
     *     o Timer interrupt event
     *     o Message from BLE
     */

    evt_queue = xQueueCreate(10, sizeof(interrupt_ctx_t));
    if (!evt_queue)
    {
        ESP_LOGE(__FUNCTION__, "Creating queue failed");
        return;
    }

    xTaskCreate(get_interrupt_task, "get_interrupt_task", 2048, NULL, 10, NULL);
}