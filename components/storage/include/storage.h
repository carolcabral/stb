#include "esp_log.h"

void init_nvs();

void init_spiffs();
void check_spiffs();

void start_logging_to_file();
void stop_logging_to_file();

int read_log_file();

void reset_all_storage();