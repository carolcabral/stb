#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "nvs_flash.h"

#include "interrupts.h"
#include "bleprph.h"
#include "timer.h"
#include "animations.h"
#include "storage.h"

static const char *TAG = "example";

#define DEFAULT_DELAY 10
#define DEFAULT_TIMER 10

#define BLINK_GPIO CONFIG_BLINK_GPIO

static led_strip_handle_t led_strip;

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = NUM_PIXELS, // at least one LED on board
    };
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

static void animations_task(void *arg)
{
    int frame = 0;
    int delay = DEFAULT_DELAY;

    while (1)
    {
        // printf("Current animation: %s\n", animations_name[global_current_animation]);
        switch (global_current_animation)
        {
        case AUDIO_SPECTRUM:
            delay = DEFAULT_DELAY;

            // uint8_t col = (frame % (delay * 1000)) % NUM_COLS;
            // single_column_wave(led_strip, frame, 0);
            audio_spectrum(led_strip, frame);
            break;
        case COLORFUL_START:
            delay = DEFAULT_DELAY;
            global_current_animation++;
            break;
        case TINKLE_STAR_SINGLE_COLOR:
            tinkle_star_single_color(led_strip, frame);
            break;

        case TINKLE_START_MULTI_COLOR:
            tinkle_star_rainbow(led_strip, frame);
            break;

        case ROTATING_LINE_SINGLE_COLOR_CW:
            rotating_cross_single_color(led_strip, frame, 1);
            break;
        case ROTATING_LINE_SINGLE_COLOR_CCW:
            rotating_cross_single_color(led_strip, frame, 0);
            break;
        case ROTATING_LINE_SINGLE_COLOR_ALTERNATING:
            rotating_cross_single_color(led_strip, frame, frame % 2);
            break;
        case ROTATING_LINE_MULTI_COLOR_ALTERNATING:
            rotating_cross_rainbow_with_trail(led_strip, frame, frame % 2);
            break;
        case ROTATING_LINE_MULTI_COLOR_CW:
            rotating_cross_rainbow_with_trail(led_strip, frame, 1);
            break;
        case ROTATING_LINE_MULTI_COLOR_CCW:
            rotating_cross_rainbow_with_trail(led_strip, frame, 0);
            break;
        case RAINBOW_SCROLL:
            rainbow_scroll(led_strip, frame);
            break;
        case COLOR_PULSE:
        case COLORFUL_END:
            color_pulse(led_strip, frame);
            break;

        case WHITE_START:
            delay = 20;
            global_current_animation++;
            break;
        case SKY_OF_STARS:
        case WHITE_END:

            sky_of_stars(led_strip);
            break;

        // Love
        case I_LOVE_START:
            stop_timer();
            configure_timer_alarm(10); // Restart timer to 5s
            start_timer();
            delay = 50;
            global_current_animation++; // Move to next animation
            break;
        case I_LOVE_YOU:
            i_love_you(led_strip);
            break;
        case HEART:
            growing_heart(led_strip);
            break;
        case I_LOVE_LUCAS:
            i_love_lucas(led_strip, frame);
            break;

        case I_LOVE_END:
            delay = DEFAULT_DELAY;
            stop_timer();
            // Restore timer
            configure_timer_alarm(DEFAULT_TIMER);
            start_timer();

            break;

        case SPIRAL_WAVE:
            delay = 100;
            anim_spiral_wave(led_strip, frame);
            break;

        case FIRE:
            anim_fire_by_frame(led_strip, frame);
            break;

        case ANIMATIONS_MAX:
        default:
            led_strip_clear(led_strip);
            break;
        }
        frame++;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    init_nvs();

    init_spiffs();

    configure_interrupts();

    configure_timer(DEFAULT_TIMER);

    /* Configure the peripheral according to the LED type */
    configure_led();

    printf("Minimum free heap size: %lu bytes\n", esp_get_minimum_free_heap_size());

    configure_ble();

    read_log_file();

    start_logging_to_file();

    xTaskCreate(animations_task, "animations_task", 2048, NULL, 10, NULL);

    read_log_file();
    stop_logging_to_file();
    reset_all_storage();
}
