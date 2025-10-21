#include <stdio.h>

#include "esp_log.h"

#include "timer.h"
#include "interrupts.h"

#define WATER_TIMER_ALARM_IN_SEC 30 * 60

timer_ctx_t water_timer = {
    .handle = NULL,
    .is_running = 0,
    .alarm_interval_sec = WATER_TIMER_ALARM_IN_SEC,
};

static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;

    if (queue != NULL)
    {
        interrupt_ctx_t it = {.mask_value = &timer, .source = IT_SOURCE_TIMER};
        xQueueSendFromISR(queue, &it, NULL);
    }
    return (high_task_awoken == pdTRUE);
}

static void start_timer(timer_ctx_t *timer)
{
    if (timer->handle == NULL || timer->is_running)
        return;

    ESP_ERROR_CHECK(gptimer_enable(timer->handle));
    ESP_ERROR_CHECK(gptimer_start(timer->handle));
    timer->is_running = true;
}

static void stop_timer(timer_ctx_t *timer)
{
    if (timer->handle == NULL || !timer->is_running)
        return;

    timer->is_running = false;
    ESP_ERROR_CHECK(gptimer_stop(timer->handle));
    ESP_ERROR_CHECK(gptimer_disable(timer->handle));
    ESP_LOGW(__FUNCTION__, "Timer stopped\n");
}

void delete_timer(timer_ctx_t *timer)
{
    stop_timer(timer);
    ESP_ERROR_CHECK(gptimer_del_timer(timer->handle));
    ESP_LOGW(__FUNCTION__, "TImer deleted!");
}

void clear_timer(timer_ctx_t *timer)
{
    if (timer->handle == NULL)
        return;

    ESP_ERROR_CHECK(gptimer_set_raw_count(timer->handle, 0));
}

static void restart_timer(timer_ctx_t *timer)
{
    stop_timer(timer);
    clear_timer(timer);
    start_timer(timer);
}

double get_current_time(timer_ctx_t *timer)
{
    uint64_t count = 0;
    ESP_ERROR_CHECK(gptimer_get_raw_count(timer->handle, &count));
    return (double)count / TIMERS_RESOLUTION_HZ;
}

/**
 * Configures and starts a timer, and if alarm_interval_sec is different than zero,
 * creates an alarm with default callback function that sends the timer handle as
 * information to the event queue.
 */
void configure_timer(timer_ctx_t *timer)
{
    if (timer->handle == NULL)
    {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Select the default clock source
            .direction = GPTIMER_COUNT_UP,      // Counting direction is up
            .resolution_hz = TIMERS_RESOLUTION_HZ,
        };
        // Create a timer instance
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer->handle));

        gptimer_event_callbacks_t cbs = {
            .on_alarm = timer_on_alarm_cb, // Call the user callback function when the alarm event occurs
        };
        // Register timer event callback functions, allowing user context to be carried
        ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer->handle, &cbs, evt_queue));
        ESP_LOGI(__FUNCTION__, "Novo timer configurado 0x%p", timer->handle);
    }

    if (timer->is_running)
    {
        ESP_LOGE(__FUNCTION__, "Timer is running! Unable to configure new timer!");
        return;
    }

    if (timer->alarm_interval_sec != 0)
    {
        gptimer_alarm_config_t alarm_config = {
            .reload_count = 0,                                               // When the alarm event occurs, the timer will automatically reload to 0
            .alarm_count = TIMERS_RESOLUTION_HZ * timer->alarm_interval_sec, // Set the actual alarm period, since the resolution is 1us, 1000000 represents 1s
            // .flags.auto_reload_on_alarm = false,                // Enable auto-reload function
            .flags.auto_reload_on_alarm = true, // Enable auto-reload function
        };
        // Set the timer's alarm action
        ESP_ERROR_CHECK(gptimer_set_alarm_action(timer->handle, &alarm_config));

        ESP_LOGI(__FUNCTION__, "Novo alarme configurado\n");
    }
    start_timer(timer);
}
