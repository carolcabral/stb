#include <stdio.h>
#include "esp_log.h"

#include "lights.h"
#include "animations.h"
#include "timer.h"

#define CONFIG_LED_STRIP_GPIO GPIO_NUM_12

TaskHandle_t animations_task_handle;
// TaskHandle_t love_task_handle;

led_strip_handle_t led_strip;
extern const anim_ctx_t special_love_animation[], special_water_animation[];

static void configure_led_strip(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_LED_STRIP_GPIO,
        .max_leds = CONFIG_LED_STRIP_QTD, // at least one LED on board
    };
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };

    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    led_strip_clear(led_strip);
    ESP_LOGI(__FUNCTION__, "LED Strip with %d configured at pin %d\n", CONFIG_LED_STRIP_QTD, CONFIG_LED_STRIP_GPIO);
}

void love_animation_task(timer_ctx_t *timer);
void run_requested_animations(const anim_ctx_t *anim, uint8_t num, timer_ctx_t *timer);
extern uint8_t num_water, num_love;
static void animations_task(void *arg)
{
    int frame = 0, index = 0;

    const anim_ctx_t general_animations[] =
        {
            // Entre 10 e 20
            {"RAINBOW_SCROLL", &rainbow_scroll, 10},

            // Entre 10 e 20
            {"COLOR_PULSE", &color_pulse, 10},

            // {"AUDIO_SPECTRUM", &audio_spectrum, 100},
            {"TINKLE_STAR_SINGLE_COLOR", &tinkle_star_single_color, 20},
            {"TINKLE_START_MULTI_COLOR", &tinkle_star_rainbow, 20},
            {"SPIRAL_WAVE", &anim_spiral_wave, 50},
            // {"FIRE", &anim_fire_by_frame, 100},

            // {"PINEAPPLE", &anim_pineapple, 100},
            {"MOVING_PINEAPLE", &anim_pineapple_move, 200},

            // sky_of_stars(led_strip);

            {"LINE_SINGLE_COLOR_CW", &rotating_cross_cw, 10},
            {"LINE_SINGLE_COLOR_CCW", &rotating_cross_ccw, 10},

            // "LINE_SINGLE_COLOR", &rotating_cross_single_color(led_strip, frame, frame % 2);

            // LINE_MULTI_COLOR_CW, &rotating_cross_rainbow_with_trail(led_strip, frame, 1);
            // LINE_MULTI_COLOR_CCW &rotating_cross_rainbow_with_trail(led_strip, frame, 0);
            // LINE_MULTI_COLOR_, &rotating_cross_rainbow_with_trail(led_strip, frame, frame % 2);

        };

    uint8_t max_animations = sizeof(general_animations) / sizeof(anim_ctx_t);

    // Receive notification from BLE or timer
    uint32_t ulNotifiedValue = 0;

    // Creates a timer to change between animations
    double current_time = 0;
    timer_ctx_t animations_timer = {NULL, 0, 0};
    configure_timer(&animations_timer);

    while (1)
    {
        // Uma animação está sendo requisitada (via BLE ou timer), rode em modo blocking e depois retorne ao comportamento padrão
        if (xTaskNotifyWait(0, 0xFF, &ulNotifiedValue, pdMS_TO_TICKS(1)) == pdPASS)
        {
            ESP_LOGW("", "Notification received: %ld\n", ulNotifiedValue);

            switch (ulNotifiedValue)
            {
            case REQUEST_ANIM_LOVE:
                ESP_LOGW("", "Special Notification received!");
                run_requested_animations(special_love_animation, num_love, &animations_timer);

                break;
            case REQUEST_ANIM_WATER:
                ESP_LOGW("", "waterrr!");

                run_requested_animations(special_water_animation, num_water, &animations_timer);

                break;

            default:
                // RUn one single requested animation
                run_requested_animations(&general_animations[ulNotifiedValue], 1, &animations_timer);
                break;
            }

            clear_timer(&animations_timer);
        }

        current_time = get_current_time(&animations_timer) + 1;

        // Default behavior
        if ((int)current_time % ANIMATION_INTERVAL_IN_SEC_DEFAULT == 0)
        {
            index++;
            index = index % max_animations;
            clear_timer(&animations_timer);
        }
        // Roda animação
        // printf("%d: %d / %d: %s\n", (int)current_time, index, max_animations, general_animations[index].name);
        general_animations[index].func(led_strip, frame++);

        // Special animations task
        vTaskDelay(general_animations[index].delay / portTICK_PERIOD_MS);
    }
}

// void love_animation_task(void *arg);
/* Configures LED strip and creates animation task */
void configure_leds()
{
    configure_led_strip();

    // General animations task
    xTaskCreate(animations_task, "animations_task", 5 * 1024, NULL, 2, &animations_task_handle);
}
