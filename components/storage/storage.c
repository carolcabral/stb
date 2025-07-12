#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_vfs.h"
#include "nvs_flash.h"

#include "storage.h"

#define LOG_FILE_PATH "/spiffs/log.txt"
#define PARTITION_LABEL NULL

#define MAX_LOG_LINES 100
#define MAX_LINE_LENGTH 128
#define MAX_LOG_SIZE_BYTES MAX_LINE_LENGTH *MAX_LOG_LINES

#define WRITE_CACHE_CYCLE 10 // Write cycles to sync file

#define NVS_NAMESPACE "log"
#define NVS_KEY_OFFSET "offset" // Stores the current write offset position

static const char *TAG = "Log";
static FILE *log_file = NULL;
static size_t log_offset = 0;

void reset_all_storage()
{
    ESP_ERROR_CHECK(esp_spiffs_format(PARTITION_LABEL));
    ESP_ERROR_CHECK(nvs_flash_erase());
}

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void init_spiffs()
{
    // SPIFFS is able to reliably utilize only around 75% of assigned partition space.
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Initialized SPIFFS");

    check_spiffs();
}

void check_spiffs()
{
    ESP_LOGI(TAG, "Performing SPIFFS check.");
    esp_err_t ret = esp_spiffs_check(PARTITION_LABEL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS check failed (%s)", esp_err_to_name(ret));
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(PARTITION_LABEL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(PARTITION_LABEL);
        return;
    }
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
}

static void save_log_offset_to_nvs(size_t offset)
{
    esp_err_t ret;
    nvs_handle_t nvs;

    if ((ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs)) == ESP_OK)
    {
        ret |= nvs_set_u32(nvs, NVS_KEY_OFFSET, offset);
        ret |= nvs_commit(nvs);
        nvs_close(nvs);
    }

    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Error saving log offset to NVS");
}

static void load_log_offset_from_nvs()
{
    esp_err_t ret;
    nvs_handle_t nvs;
    uint32_t offset = 0;
    if ((ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs)) == ESP_OK)
    {
        ret |= nvs_get_u32(nvs, NVS_KEY_OFFSET, &offset);
        nvs_close(nvs);
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error retrieving log offset from NVS");
        offset = 0;
    }

    log_offset = offset;
}

/* Creates a custom log function that stores
the ESP_LOGX calls into a file in SPIFFS. It also
rotates the file to preserve storage usage. When it
reaches MAX_LINE_LENGTH * MAX_LOG_LINES, it goes back
to the begining of the file.
 */
int custom_log_vprintf(const char *fmt, va_list args)
{
    static bool static_fatal_error = false;
    static uint32_t counter_write = 0;
    size_t write_size;

    if (log_file == NULL)
    {
        printf("%s: File handle to log file is NULL\n", __FUNCTION__);
        return -1;
    }

    if (static_fatal_error)
    {
        printf("%s: Previous error detected. Aborting..\n", __FUNCTION__);
        return -1;
    }

    char line[MAX_LINE_LENGTH] = "";
    int len = vsnprintf(line, MAX_LINE_LENGTH - 1, fmt, args);
    if (len < 0)
    {
        printf("%s: vsnprintf failed\n", __FUNCTION__);
        static_fatal_error = true;
        return len;
    }

    // Ensure it's padded to MAX_LINE_LENGTH
    if (len >= MAX_LINE_LENGTH)
    {
        len = MAX_LINE_LENGTH - 1;
        line[len] = '\n';
        len++;
    }

    if ((log_offset + len) >= MAX_LOG_SIZE_BYTES)
    {
        // Wrap back to beginning
        log_offset = 0;
    }

    fseek(log_file, log_offset, SEEK_SET);
    write_size = fwrite(line, 1, len, log_file);
    fflush(log_file);

    if (write_size != len)
    {
        printf("%s Failed vfprintf() -> disable future vfprintf(log_file) \n", __FUNCTION__);
        static_fatal_error = true;
        return write_size;
    }
    log_offset += write_size;

    // Smart commit after WRITE_CACHE_CYCLE writes
    counter_write++;
    if (counter_write % WRITE_CACHE_CYCLE == 0)
    {
        fsync(fileno(log_file));
        save_log_offset_to_nvs(log_offset);
    }

    // Optional - output to console
    return vprintf(fmt, args);
}

void start_logging_to_file()
{
    ESP_LOGI(TAG, "Starting log to file...\n");

    log_file = fopen(LOG_FILE_PATH, "r+");
    if (!log_file)
    {
        log_file = fopen(LOG_FILE_PATH, "w+");
        if (!log_file)
        {
            ESP_LOGE("LOG", "Failed to open or create log file");
            return;
        }
    }

    esp_log_set_vprintf(&custom_log_vprintf);
    load_log_offset_from_nvs(); // restore last write position
    ESP_LOGI("LOG", "Started circular log with %d lines at offset %d", MAX_LOG_LINES, log_offset);
}

void stop_logging_to_file()
{
    esp_log_set_vprintf(&vprintf);
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
        // esp_vfs_spiffs_unregister(PARTITION_LABEL);
    }
}

int read_log_file()
{
    FILE *f = fopen(LOG_FILE_PATH, "r");

    if (f == NULL)
    {
        ESP_LOGE("LOG", "Failed to open or create log file");
        return 1;
    }
    printf("==== LOG file ====\n");

    // Read characters until the end of the file
    char line[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
    {
        line[strcspn(line, "\n")] = 0;
        printf("%d: %s\n", i++, line);
    }

    fclose(f);
    printf("=================\n");

    return 0; // Indicate successful execution
}