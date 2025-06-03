#include <stdio.h>

#include "driver/gptimer.h"
#include "esp_log.h"

#include "timer.h"
#include "interrupts.h"
// #include "animations.h"

static gptimer_handle_t gptimer = NULL;
static bool timer_is_running = false;

static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    interrupt_source_t it_source = {.is_ble = false, .mask_value = 0, .is_timer = true};

    xQueueSendFromISR(queue, &it_source, NULL);
    return (high_task_awoken == pdTRUE);
}

void configure_timer_alarm(int timer_interval_sec)
{
    if (gptimer == NULL || timer_is_running)
        return;

    ESP_LOGW(__FUNCTION__, "Configurando alarme\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,                           // When the alarm event occurs, the timer will automatically reload to 0
        .alarm_count = 1000000 * timer_interval_sec, // Set the actual alarm period, since the resolution is 1us, 1000000 represents 1s
        .flags.auto_reload_on_alarm = true,          // Enable auto-reload function
    };
    // Set the timer's alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
}

void clear_timer()
{
    if (gptimer == NULL)
        return;

    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));
}
void configure_timer(int timer_interval_sec)
{
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

    configure_timer_alarm(timer_interval_sec);
    ESP_LOGW("", "Timer configurado!\n");
    start_timer();
}

void start_timer()
{
    if (gptimer == NULL || timer_is_running)
        return;

    ESP_LOGW(__FUNCTION__, "Starting timer\n");

    timer_is_running = true;
    // Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    ESP_LOGI("", "Timer enabled\n");
}
void stop_timer()
{
    if (gptimer == NULL || !timer_is_running)
        return;

    ESP_LOGW(__FUNCTION__, "Stopping timer\n");

    timer_is_running = false;
    // Stop the timer
    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    // Disable the timer
    ESP_ERROR_CHECK(gptimer_disable(gptimer));
}
