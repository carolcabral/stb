/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#include "animation.h"
#include "timer.h"

static const char *TAG = "example";

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

#if 0
void app_main2(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();
    // int brightness = 255;
    int t = 0;
    float brightness, wave;
    int pixel;
    while (1)
    {
        led_strip_clear(led_strip);

        for (int col = 0; col < NUM_COLS; col++)
        {
            uint8_t r, g, b;
            color_wheel((col * 40 + t) % 256, &r, &g, &b);

            for (int row = 0; row < NUM_ROWS; row++)
            {
                pixel = pixel_matrix[row][col];

                wave = sin((t + row * 10) * PI / 180.0f); // radians
                brightness = (wave + 1.0f) / 2.0f;        // 0 to 1
                ESP_LOGI(TAG, "Pixel: %d, %d %d %d", pixel,
                         (int)(brightness * r),
                         (int)(brightness * g),
                         (int)(brightness * b));
                led_strip_set_pixel(led_strip, pixel,
                                    (int)(brightness * r),
                                    (int)(brightness * g),
                                    (int)(brightness * b));
                led_strip_refresh(led_strip);
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }
        }

        // led_strip_refresh(led_strip);
        t = (t + 5) % 360;
        // vTaskDelay(800 / portTICK_PERIOD_MS);
    }

    return;

    growing_heart();
    growing_heart();
    growing_heart();
    growing_heart();

    blink_heart();
    blink_heart();

    return;

    blink_heart();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    led_strip_clear(led_strip);

    return;

    // ESP_LOGI(TAG, "Turning the LED %d with brightness %d!", i, brightness);
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
        led_strip_set_pixel(led_strip, i, brightness * 1, brightness * 1, brightness * 1);

        led_strip_refresh(led_strip);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    // while (1)
    // {
    brightness = 1;
    set_color(50, 50, 50, brightness);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    brightness = 255;
    set_color(50, 0, 0, brightness);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    led_strip_clear(led_strip);

    for (uint8_t x = 0; x < WIDTH; x++)
    {
        for (uint8_t y = 0; y < HEIGHT; y++)
        {
            uint8_t led = get_led_number(x, y);
            ESP_LOGI(TAG, "LED n x: %d, y: %d = %d", x, y, led);
            brightness = 10;

            led_strip_set_pixel(led_strip, led, brightness * 50, brightness * 50, brightness * 50);
            led_strip_refresh(led_strip);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        // }
    }
    led_strip_clear(led_strip);
}
#endif
static void animations_task(void *arg)
{
    int frame = 0;
    int delay = 45;
    while (1)
    {
        printf("Current animation: %s\n", animations_name[global_current_animation]);
        switch (global_current_animation)
        {
        case AUDIO_SPECTRUM:
            // uint8_t col = (frame % (delay * 1000)) % NUM_COLS;
            // single_column_wave(led_strip, frame, 0);
            audio_spectrum(led_strip, frame);
            break;

        case SKY_OF_STARS:
            sky_of_stars(led_strip);

            break;

        case TINKLE_STAR_SINGLE_COLOR:
            tinkle_star_single_color(led_strip, frame);

            break;

        case TINKLE_START_MULTI_COLOR:
            tinkle_star_rainbow(led_strip, frame);

            break;

        case ROTATING_LINE_SINGLE_COLOR:
            rotating_cross_single_color(led_strip, frame, frame % 2);

            break;

        case ROTATING_LINE_MULTI_COLOR:
            rotating_cross_rainbow_with_trail(led_strip, frame, frame % 2);

            break;
        case RAINBOW_SCROLL:
            rainbow_scroll(led_strip, frame);
            break;
        case COLOR_PULSE:
            color_pulse(led_strip, frame);
            break;

        // Love
        case I_LOVE_YOU:
            // Restart timer to 5s
            stop_timer();
            configure_timer_alarm(5);
            start_timer();
            i_love_you(led_strip);

            break;

        case HEART:
            growing_heart(led_strip);
            break;

        case I_LOVE_LUCAS:
            delay = 50;
            i_love_lucas(led_strip, frame);

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
    configure_pins();

    /* Configure the peripheral according to the LED type */
    configure_led();

    printf("Minimum free heap size: %lu bytes\n", esp_get_minimum_free_heap_size());

    // int delay = 50;
    // while (1)
    // {

    //     tinkle_star_single_color(led_strip, delay);
    //     delay++;
    //     vTaskDelay(10 / portTICK_PERIOD_MS);
    // }

    // start gpio task
    xTaskCreate(animations_task, "animations_task", 2048, NULL, 10, NULL);
// while (1)
// {
//     vTaskDelay(250 / portTICK_PERIOD_MS);
// }
#ifdef SKY
    int lowest_delay = 500;
    int highest_delay = 1;
    int current_delay = 100;
    while (1)
    {
        for (current_delay = current_delay; current_delay >= highest_delay; current_delay--)
            anim_sky_of_stars(led_strip, current_delay);
        ESP_LOGI(TAG, "Going down...");

        for (current_delay = highest_delay; current_delay <= lowest_delay; current_delay += 10)
        {
            ESP_LOGI(TAG, "%d", current_delay);

            anim_sky_of_stars(led_strip, current_delay);
        }
        ESP_LOGI(TAG, "Going up...");
    }
#endif
    // anim_rotating_cross_single_color(led_strip, 10);
    // anim_rotating_cross_rainbow(led_strip, delay);
    // anim_rotating_cross_rainbow_with_trail(led_strip, delay);
    // anim_rotating_cross_single_color(led_strip, delay);

    // color_t c1 = {250, 160, 0};
    // anim_rotating_plus_cw(led_strip, 1000, c1);
    // const uint8_t matrix[NUM_ROWS][NUM_COLS] = {
    //     {0, 11, 12, 23, 24},
    //     {1, 10, 13, 22, 25},
    //     {2, 9, 14, 21, 26},
    //     {3, 8, 15, 20, 27},
    //     {4, 7, 16, 19, 28},
    //     {5, 6, 17, 18, 29},
    // };
    // anim_audio_spectrum(led_strip);
    // Traverse each column
    // uint8_t pixel = 0;
    // for (uint8_t col = 0; col < NUM_COLS; col++)
    // {
    //     // Traverse rows
    //     for (uint8_t row = 0; row < NUM_ROWS; row++)
    //     {
    //         pixel = matrix[row][col];
    //         led_strip_set_pixel(led_strip, pixel, 1, 1, 1);
    //         led_strip_refresh(led_strip);
    //         vTaskDelay(250 / portTICK_PERIOD_MS);
    //         led_strip_clear(led_strip);
    //     }
    // }

    // anim_audio_spectrum(led_strip, 50);

    // anim_rainbow_scroll(led_strip, 10);
    // anim_color_pulse(led_strip, 10);
}
