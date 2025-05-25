#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "timer.h"
#include "animation.h"

#define GPIO_INPUT_IO_1 34
#define GPIO_INPUT_IO_2 35
#define GPIO_INPUT_IO_3 32
#define GPIO_INPUT_IO_4 33
#define GPIO_INPUT_IO_5 25
#define GPIO_INPUT_IO_6 26
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_1) | (1ULL << GPIO_INPUT_IO_2) | \
                            (1ULL << GPIO_INPUT_IO_3) | (1ULL << GPIO_INPUT_IO_4) | \
                            (1ULL << GPIO_INPUT_IO_5) | (1ULL << GPIO_INPUT_IO_6))

static QueueHandle_t evt_queue = NULL;

typedef struct
{
    bool is_gpio;
    uint32_t gpio_number;
    bool is_timer;

} interrupt_source_t;

static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    interrupt_source_t it_source = {.is_gpio = false, .gpio_number = 0, .is_timer = true};

    xQueueSendFromISR(queue, &it_source, NULL);
    return (high_task_awoken == pdTRUE);
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    interrupt_source_t it_source = {.is_gpio = true, .gpio_number = gpio_num, .is_timer = false};
    xQueueSendFromISR(evt_queue, &it_source, NULL);
}

static void get_interrupt_task(void *arg)
{
    interrupt_source_t it_source;

    uint8_t mask;
    for (;;)
    {
        if (xQueueReceive(evt_queue, &it_source, portMAX_DELAY))
        {
            printf("queue received!\n");
            if (it_source.is_gpio)
            {

                mask = gpio_get_level(GPIO_INPUT_IO_1) << 0 |
                       (gpio_get_level(GPIO_INPUT_IO_2) << 1) |
                       (gpio_get_level(GPIO_INPUT_IO_3) << 2) |
                       (gpio_get_level(GPIO_INPUT_IO_4) << 3) |
                       (gpio_get_level(GPIO_INPUT_IO_5) << 4) |
                       (gpio_get_level(GPIO_INPUT_IO_6) << 5);

                printf("GPIO IT %d \n", (mask - 1) % ANIMATIONS_MAX);
                global_current_animation = (mask - 1) % ANIMATIONS_MAX;
                // printf("GPIO[%ld] intr, val: %d | %s \n", io_num, mask, animations_name[global_current_animation]);
            }
            if (it_source.is_timer)
            {
                printf("TIMER!\n");
                global_current_animation = (global_current_animation + 1) % ANIMATIONS_MAX;
            }
        }
    }
}

static void configure_gpios(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *)GPIO_INPUT_IO_1);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void *)GPIO_INPUT_IO_2);
    gpio_isr_handler_add(GPIO_INPUT_IO_3, gpio_isr_handler, (void *)GPIO_INPUT_IO_3);
    gpio_isr_handler_add(GPIO_INPUT_IO_4, gpio_isr_handler, (void *)GPIO_INPUT_IO_4);
    gpio_isr_handler_add(GPIO_INPUT_IO_5, gpio_isr_handler, (void *)GPIO_INPUT_IO_5);
    gpio_isr_handler_add(GPIO_INPUT_IO_6, gpio_isr_handler, (void *)GPIO_INPUT_IO_6);
}

static void configure_timer(int timer_interval_sec)
{

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Select the default clock source
        .direction = GPTIMER_COUNT_UP,      // Counting direction is up
        .resolution_hz = 1 * 1000 * 1000,   // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond
    };
    // Create a timer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_on_alarm_cb, // Call the user callback function when the alarm event occurs
    };
    // Register timer event callback functions, allowing user context to be carried
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, evt_queue));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,                           // When the alarm event occurs, the timer will automatically reload to 0
        .alarm_count = 1000000 * timer_interval_sec, // Set the actual alarm period, since the resolution is 1us, 1000000 represents 1s
        .flags.auto_reload_on_alarm = true,          // Enable auto-reload function
    };
    // Set the timer's alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void configure_pins()
{
    // create a queue to handle gpio event from isr
    evt_queue = xQueueCreate(10, sizeof(interrupt_source_t));
    if (!evt_queue)
    {
        ESP_LOGE("[configure_pins]", "Creating queue failed");
        return;
    }

    // start gpio task
    xTaskCreate(get_interrupt_task, "get_interrupt_task", 2048, NULL, 10, NULL);

    configure_timer(5);
    configure_gpios();
}