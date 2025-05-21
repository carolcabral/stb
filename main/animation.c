#include "animation.h"

#include "esp_random.h"
#include "esp_log.h"

// static const uint8_t matrix2[NUM_ROWS][NUM_COLS] = {
//     {5, 6, 17, 18, 29},
//     {4, 7, 16, 19, 28},
//     {3, 8, 15, 20, 27},
//     {2, 9, 14, 21, 26},
//     {1, 10, 13, 22, 25},
//     {0, 11, 12, 23, 24},
// };
static const uint8_t matrix[NUM_ROWS][NUM_COLS] = {
    {0, 11, 12, 23, 24},
    {1, 10, 13, 22, 25},
    {2, 9, 14, 21, 26},
    {3, 8, 15, 20, 27},
    {4, 7, 16, 19, 28},
    {5, 6, 17, 18, 29},
};

static const char *TAG = "Animations";

// === Sky of Stars ===
void anim_sky_of_stars(led_strip_handle_t led_strip, int delay)
{
    // ESP_LOGI(TAG, "Sky full of stars %u", delay);
    // while (1)
    // {
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (esp_random() % 20 == 0)
        {
            // Turn on random pixel as white star
            led_strip_set_pixel(led_strip, i, 255, 255, 255);
        }
        else
        {
            led_strip_set_pixel(led_strip, i, 0, 0, 0);
        }
    }
    led_strip_refresh(led_strip);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    // }
}

// === Heart ===
void heart(led_strip_handle_t led_strip, uint8_t brightness)
{
    static uint8_t heart_pixels[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    for (uint8_t i = 0; i < sizeof(heart_pixels); i++)
        led_strip_set_pixel(led_strip, heart_pixels[i], brightness * 1, 0, 0);

    led_strip_refresh(led_strip);
}
void anim_heart(led_strip_handle_t led_strip)
{
    // Growing heart
    uint8_t brightness = 10;
    int delay = 100;
    static uint8_t heart_pixels0[] = {14};
    static uint8_t heart_pixels1[] = {7, 8, 9, 13, 15, 19, 20, 21};
    static uint8_t heart_pixels2[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    static uint8_t heart_pixels3[] = {1, 5, 17, 23, 25, 29};

    while (1)
    {
        for (uint8_t i = 0; i < sizeof(heart_pixels0); i++)
            led_strip_set_pixel(led_strip, heart_pixels0[i], brightness * 1, 0, 0);

        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        led_strip_clear(led_strip);
        for (uint8_t i = 0; i < sizeof(heart_pixels1); i++)
            led_strip_set_pixel(led_strip, heart_pixels1[i], brightness * 1, 0, 0);

        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        led_strip_clear(led_strip);
        for (uint8_t i = 0; i < sizeof(heart_pixels2); i++)
            led_strip_set_pixel(led_strip, heart_pixels2[i], brightness * 1, 0, 0);

        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        // led_strip_clear(led_strip);

        // Gradient heart
        uint8_t step = 256 / 8;
        uint8_t i = 1;

        do
        {
            heart(led_strip, i);
            i += step;
            vTaskDelay(delay / portTICK_PERIOD_MS);
        } while (i + step <= 255);

        do
        {
            heart(led_strip, i);
            i -= step;
            vTaskDelay(delay / portTICK_PERIOD_MS);

        } while (i - step >= 0);

        led_strip_clear(led_strip);
        for (uint8_t i = 0; i < sizeof(heart_pixels3); i++)
            led_strip_set_pixel(led_strip, heart_pixels3[i], brightness * 1, 0, 0);

        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        led_strip_clear(led_strip);
    }
}

// === Rotating cross ===
void anim_rotating_plus_cw(led_strip_handle_t led_strip, int delay, color_t color)
{
    // Centro aproximado da matriz (2,2) ou (row,col)
    int cx = NUM_ROWS / 2 - 1; // = 2
    int cy = NUM_COLS / 2;     // = 2
    int phase = 0;
    while (1) // TODO: WHILE?
    {
        // Fase da rotação
        phase = phase % 4;

        for (int row = 0; row < NUM_ROWS; row++)
        {
            for (int col = 0; col < NUM_COLS; col++)
            {
                int pixel = matrix[row][col];

                bool is_on = false;

                // Clockwise
                switch (phase)
                {
                case 0: // vertical
                    is_on = (col == cy);
                    break;
                case 1: // diagonal /
                    is_on = (row - cx == col - cy);
                    break;
                case 2: // horizontal
                    is_on = (row == cx);
                    break;
                case 3: // diagonal /
                    is_on = (row - cx == -(col - cy));
                    break;
                }

                if (is_on)
                {
                    led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue); // orange
                }
                else
                {
                    led_strip_set_pixel(led_strip, pixel, 0, 0, 0); // off
                }
            }
        }

        led_strip_refresh(led_strip);

        phase++;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}
void anim_rotating_plus_ccw(led_strip_handle_t led_strip, int delay, color_t color)
{
    // Centro aproximado da matriz (2,2) ou (row,col)
    int cx = NUM_ROWS / 2 - 1; // = 2
    int cy = NUM_COLS / 2;     // = 2
    int phase = 0;
    while (1) // TODO: WHILE?
    {
        // Fase da rotação
        phase = phase % 4;

        for (int row = 0; row < NUM_ROWS; row++)
        {
            for (int col = 0; col < NUM_COLS; col++)
            {
                int pixel = matrix[row][col];

                bool is_on = false;

                // Clockwise
                switch (phase)
                {
                case 0: // vertical
                    is_on = (col == cy);
                    break;
                case 3: // diagonal /
                    is_on = (row - cx == col - cy);
                    break;
                case 2: // horizontal
                    is_on = (row == cx);
                    break;
                case 1: // diagonal /
                    is_on = (row - cx == -(col - cy));
                    break;
                }

                if (is_on)
                {
                    led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue); // orange
                }
                else
                {
                    led_strip_set_pixel(led_strip, pixel, 0, 0, 0); // off
                }
            }
        }

        led_strip_refresh(led_strip);

        phase++;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}

// === Audio spectrum cross ===
void anim_single_column_wave(led_strip_handle_t led_strip, int t, int col)
{
    if (col < 0 || col >= NUM_COLS)
        return;

    // Altura simulada entre 1 e NUM_ROWS
    float wave = sinf(t * PI / 90.0f + col);
    int height = (int)((wave + 1.0f) * (NUM_ROWS - 1) / 2.0f + 0.5f);

    for (int row = 0; row < NUM_ROWS; row++)
    {
        for (int c = 0; c < NUM_COLS; c++)
        {
            int pixel = matrix[row][c];

            if (c == col && row <= height)
            {
                if (row >= 4)
                    led_strip_set_pixel(led_strip, pixel, 255, 0, 0);
                else
                    led_strip_set_pixel(led_strip, pixel, 255, 255, 255);
            }
            else
            {
                // Apaga outros LEDs
                led_strip_set_pixel(led_strip, pixel, 0, 0, 0);
            }
        }
    }
    led_strip_refresh(led_strip);
}

void anim_audio_spectrum(led_strip_handle_t led_strip, int delay)
{
    static float levels[NUM_COLS] = {0}; // níveis suavizados
    int t = 0;
    while (1)
    {
        for (int col = 0; col < NUM_COLS; col++)
        {
            // Simulação: onda + ruído
            float wave = sinf(t * 0.12f + col * 1.2f);
            float noise = (esp_random() % 100) / 100.0f;
            float target = (wave + 1.0f) / 2.0f * (NUM_ROWS - 1) + noise;

            // Suavização: subida rápida, descida lenta
            if (target > levels[col])
                levels[col] = target;
            else
                levels[col] *= 0.92f;

            // Garante que a base (linha 0) sempre acenda
            int height = (int)(levels[col] + 0.5f); // arredonda

            for (int row = 0; row < NUM_ROWS; row++)
            {
                int pixel = matrix[row][col];

                if (row <= height)
                {
                    if (row >= 4)
                        led_strip_set_pixel(led_strip, pixel, 255, 0, 0); // vermelho
                    else
                        led_strip_set_pixel(led_strip, pixel, 0, 255, 255); // cyan
                }
                else
                {
                    led_strip_set_pixel(led_strip, pixel, 0, 0, 0); // apagado
                }
            }
        }
        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        t++;
    }
}
static void color_wheel(uint8_t pos, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (pos < 85)
    {
        *r = pos * 3;
        *g = 255 - pos * 3;
        *b = 0;
    }
    else if (pos < 170)
    {
        pos -= 85;
        *r = 255 - pos * 3;
        *g = 0;
        *b = pos * 3;
    }
    else
    {
        pos -= 170;
        *r = 0;
        *g = pos * 3;
        *b = 255 - pos * 3;
    }
}

// ===  Rainbow Scroll ===
void anim_rainbow_scroll(led_strip_handle_t led_strip, int delay)
{
    int frame = 0;
    while (1)
    {
        for (int col = 0; col < NUM_COLS; col++)
        {
            for (int row = 0; row < NUM_ROWS; row++)
            {
                int pixel = matrix[row][col];
                uint8_t r, g, b;
                color_wheel((col * 20 + row * 10 + frame) % 256, &r, &g, &b);
                led_strip_set_pixel(led_strip, pixel, r, g, b);
            }
        }
        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        frame++;
    }
}

// === Color Pulse ===
void anim_color_pulse(led_strip_handle_t led_strip, int delay)
{
    uint8_t r, g, b;
    int frame = 0;
    float wave, brightness;

    while (1)
    {
        color_wheel((frame * 2) % 256, &r, &g, &b);
        wave = sinf(frame * PI / 180.0f);
        brightness = (wave + 1.0f) / 2.0f;
        for (int i = 0; i < NUM_PIXELS; i++)
        {
            led_strip_set_pixel(led_strip, i,
                                (uint8_t)(brightness * r),
                                (uint8_t)(brightness * g),
                                (uint8_t)(brightness * b));
        }
        led_strip_refresh(led_strip);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        frame++;
    }
}
