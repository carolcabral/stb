#include <stdio.h>
#include "esp_log.h"

#include "lights.h"
#include "animations.h"
#include "timer.h"

#define CONFIG_LED_STRIP_GPIO GPIO_NUM_12

TaskHandle_t animations_task_handle;
led_strip_handle_t led_strip;

extern const anim_ctx_t special_love_animation[], special_water_animation[], general_animations[];
extern uint8_t num_water, num_love, num_animations;

static void configure_led_strip(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_LED_STRIP_GPIO,
        .max_leds = CONFIG_LED_STRIP_QTD,
    };
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };

    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    led_strip_clear(led_strip);
    ESP_LOGI(__FUNCTION__, "LED Strip with %d configured at pin %d\n", CONFIG_LED_STRIP_QTD, CONFIG_LED_STRIP_GPIO);
}

static void run_requested_animations(const anim_ctx_t *anim, uint8_t num, timer_ctx_t *timer)
{
    int index = 0, frame = 0;
    double current_time = 0;

    while (index < num)
    {
        current_time = get_current_time(timer) + 1;
        anim[index].func(led_strip, frame++);

        if ((int)current_time % ANIMATION_INTERVAL_IN_SEC_CONTROLLED == 0)
        {
            clear_timer(timer);
            // ESP_LOGI("", "%d: %d / %d: %s\n", (int)current_time, index, num, anim[index].name);
            index++;
            frame = 0;
        }

        vTaskDelay(anim[index].delay / portTICK_PERIOD_MS);
    }
}

static void handle_request_event(uint32_t value, timer_ctx_t *timer)
{
    switch (value)
    {
    case REQUEST_ANIM_LOVE:
        run_requested_animations(special_love_animation, num_love, timer);

        break;
    case REQUEST_ANIM_WATER:
        run_requested_animations(special_water_animation, num_water, timer);
        break;

    default:
        if (value > num_animations - 1)
        {
            ESP_LOGE("", "Unknown request: %ld", value);
            break;
        }
        // RUn one single requested animation
        run_requested_animations(&general_animations[value], 1, timer);
        break;
    }
}
static void animations_task(void *arg)
{
    int frame = 0, index = 0;

    // Receive notification from BLE or timer
    uint32_t ulNotifiedValue = 0;

    // Creates a timer to change between animations
    double current_time = 0;
    timer_ctx_t animations_timer = {NULL, 0, 0};
    configure_timer(&animations_timer);

    while (1)
    {
        // Uma animação está sendo requisitada (via BLE ou timer), rode em modo blocking e depois retorne ao comportamento padrão
        // TODO: Problema - Fica bloqueado para receber notificações consecutivas
        if (xTaskNotifyWait(0, 0xFF, &ulNotifiedValue, pdMS_TO_TICKS(1)) == pdPASS)
        {
            ESP_LOGW("", "Notification received: %ld\n", ulNotifiedValue);
            handle_request_event(ulNotifiedValue, &animations_timer);
            clear_timer(&animations_timer);
        }

        current_time = get_current_time(&animations_timer) + 1;

        // Default behavior
        if ((int)current_time % ANIMATION_INTERVAL_IN_SEC_DEFAULT == 0)
        {
            index++;
            index = index % num_animations;
            clear_timer(&animations_timer);
            frame = 0;
        }
        // Roda animação
        // printf("%d: %d / %d: %s\n", (int)current_time, index, max_animations, general_animations[index].name);
        general_animations[index].func(led_strip, frame++);
        vTaskDelay(general_animations[index].delay / portTICK_PERIOD_MS);
    }
}

/* Configures LED strip and creates animation task */
void configure_leds()
{
    configure_led_strip();

    // General animations task
    xTaskCreate(animations_task, "animations_task", 8 * 1024, NULL, 2, &animations_task_handle);
}
