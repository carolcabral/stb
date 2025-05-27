#include "animation.h"

#include "esp_random.h"
#include "esp_log.h"

static const char *TAG = "Animations";

ANIMATIONS_T global_current_animation = 0;

static const uint8_t matrix[NUM_ROWS][NUM_COLS] = {
    {0, 11, 12, 23, 24},
    {1, 10, 13, 22, 25},
    {2, 9, 14, 21, 26},
    {3, 8, 15, 20, 27},
    {4, 7, 16, 19, 28},
    {5, 6, 17, 18, 29},
};

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

static const int cross_steps[4][5][2] = {
    {{2, 2}, {1, 2}, {3, 2}, {2, 1}, {2, 3}}, // +
    {{2, 2}, {1, 1}, {3, 3}, {1, 3}, {3, 1}}, // x
    {{2, 2}, {0, 2}, {4, 2}, {2, 0}, {2, 4}}, // esticada +
    {{2, 2}, {0, 0}, {4, 4}, {0, 4}, {4, 0}}  // esticada x
};

static const int line_steps[4][6][2] = {
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}},
    {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {4, 4}},
    {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 4}},
    {{4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 4}},
};

static const int line_steps_ccw[4][6][2] = {
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}},
    {{4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 4}},
    {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 4}},
    {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {4, 4}},
};
float pixel_brightness[NUM_PIXELS] = {0};

// === Sky of Stars ===
void sky_of_stars(led_strip_handle_t led_strip)
{
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
}

// === Heart ===
const char message[] = "I h LUCAS ";
const uint8_t font[][NUM_ROWS] = {
    // I
    {
        0b01110,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b01110},
    // Heart ♥
    {
        0b01010,
        0b11111,
        0b11111,
        0b11111,
        0b01110,
        0b00100},
    // L
    {
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b11111},
    // U
    {
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110},
    // C
    {
        0b01110,
        0b10001,
        0b10000,
        0b10000,
        0b10001,
        0b01110},
    // A
    {
        0b01110,
        0b10001,
        0b10001,
        0b11111,
        0b10001,
        0b10001},
    // S
    {
        0b01110,
        0b10000,
        0b01110,
        0b00001,
        0b10001,
        0b01110},
    // space
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000}};
void heart(led_strip_handle_t led_strip, uint8_t brightness)
{
    static uint8_t heart_pixels[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    for (uint8_t i = 0; i < sizeof(heart_pixels); i++)
        led_strip_set_pixel(led_strip, heart_pixels[i], brightness * 1, 0, 0);

    led_strip_refresh(led_strip);
}
void growing_heart(led_strip_handle_t led_strip)
{
    // Growing heart
    uint8_t brightness = 255;
    int delay = 100;
    static uint8_t heart_pixels0[] = {14};
    static uint8_t heart_pixels1[] = {7, 8, 9, 13, 15, 19, 20, 21};
    static uint8_t heart_pixels2[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    static uint8_t heart_pixels3[] = {1, 5, 17, 23, 25, 29};

    while (global_current_animation == HEART)
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
void i_love_you(led_strip_handle_t led_strip)
{
    // Growing heart
    uint8_t brightness = 255;
    int delay = 100;

    static uint8_t i_pixels[] = {6, 11, 12, 13, 14, 15, 16, 17, 18, 23};
    static uint8_t u_pixels[] = {6, 7, 8, 9, 10, 11, 12, 18, 19, 20, 21, 22, 23};

    while (global_current_animation == I_LOVE_YOU)
    {
        // I
        for (uint8_t i = 0; i < sizeof(i_pixels); i++)
            led_strip_set_pixel(led_strip, i_pixels[i], brightness * 1, 0, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(delay * 10 / portTICK_PERIOD_MS);
        led_strip_clear(led_strip);

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

        // U
        for (uint8_t i = 0; i < sizeof(u_pixels); i++)
            led_strip_set_pixel(led_strip, u_pixels[i], brightness * 1, 0, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(delay * 10 / portTICK_PERIOD_MS);
        led_strip_clear(led_strip);
    }
}
void i_love_lucas(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const int num_chars = sizeof(message) - 1;          // sem o '\0'
    const int total_width = num_chars * (NUM_COLS + 1); // 1 coluna de espaço entre letras

    int scroll_offset = (frame / 2) % (total_width + NUM_COLS); // desliza da direita para esquerda

    for (int char_index = 0; char_index < num_chars; char_index++)
    {
        char c = message[char_index];
        int glyph = 7; // espaço por padrão

        switch (c)
        {
        case 'I':
            glyph = 0;
            break;
        case 'h': // Heart
            glyph = 1;
            break;
        case 'L':
            glyph = 2;
            break;
        case 'U':
            glyph = 3;
            break;
        case 'C':
            glyph = 4;
            break;
        case 'A':
            glyph = 5;
            break;
        case 'S':
            glyph = 6;
            break;
        case ' ':
            glyph = 7;
            break;
        }

        int char_x = char_index * (NUM_COLS + 1) - scroll_offset;

        // Desenha caractere se estiver visível
        if (char_x + NUM_COLS < 0 || char_x >= NUM_COLS)
            continue;

        for (int row = 0; row < NUM_ROWS && row < NUM_ROWS; row++)
        {
            int mapped_row = NUM_ROWS - 1 - row;

            for (int col = 0; col < NUM_COLS; col++)
            {
                if (char_x + col < 0 || char_x + col >= NUM_COLS)
                    continue;

                if (font[glyph][row] & (1 << (NUM_COLS - 1 - col)))
                {
                    int led = matrix[mapped_row][char_x + col];
                    led_strip_set_pixel(led_strip, led, 255, 0, 0);
                }
            }
        }
    }

    led_strip_refresh(led_strip);
}

// === Rotating cross ===
void rotating_plus_cw(led_strip_handle_t led_strip, int frame, color_t color)
{
    led_strip_clear(led_strip);

    // Gira a cada 10 frames
    int step = (frame / 10) % 4;
    for (int i = 0; i < 6; i++)
    {
        int row = line_steps[step][i][0];
        int col = line_steps[step][i][1];
        int pixel = matrix[row][col];
        led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue);
    }
    led_strip_refresh(led_strip);
}
void rotating_plus_ccw(led_strip_handle_t led_strip, int frame, color_t color)
{
    led_strip_clear(led_strip);

    // Gira a cada 10 frames
    int step = (frame / 10) % 4;
    for (int i = 0; i < NUM_ROWS; i++)
    {
        int row = line_steps_ccw[step][i][0];
        int col = line_steps_ccw[step][i][1];
        int pixel = matrix[row][col];
        led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue);
    }
    led_strip_refresh(led_strip);
}
void rotating_cross_single_color(led_strip_handle_t led_strip, int frame, uint8_t is_cw) // colorfull single color tinkle little star
{
    // Geração de cor suave (como color_pulse)
    float phase = (frame % 120) / 120.0f * 2 * M_PI;
    color_t color = {
        .red = (sinf(phase) + 1.0f) * 127.5f,
        .green = (sinf(phase + 2.1f) + 1.0f) * 127.5f,
        .blue = (sinf(phase + 4.2f) + 1.0f) * 127.5f,
    };

    if (is_cw)
        rotating_plus_cw(led_strip, frame, color);
    else
        rotating_plus_ccw(led_strip, frame, color);
}
void rotating_cross_rainbow_with_trail(led_strip_handle_t led_strip, int frame, uint8_t is_cw) // colorfull tinkle little star
{
    led_strip_clear(led_strip);
    int step = (frame / 10) % 4;
    int row, col;

    // Atualiza os LEDs da cruz para brilho máximo (1.0)
    for (int i = 0; i < 6; i++)
    {
        if (is_cw)
        {
            row = line_steps[step][i][0];
            col = line_steps[step][i][1];
        }
        else
        {
            row = line_steps_ccw[step][i][0];
            col = line_steps_ccw[step][i][1];
        }

        int pixel = matrix[row][col];
        if (pixel >= 0 && pixel < NUM_PIXELS)
            pixel_brightness[pixel] = 1.0f;
    }

    // Aplica fade aos pixels e atualiza a cor
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        // Aplica decaimento
        pixel_brightness[i] *= 0.6f;
        if (pixel_brightness[i] < 0.01f)
            pixel_brightness[i] = 0;

        // Calcula cor tipo arco-íris
        float wave = (frame + i * 3) * 0.1f;

        uint8_t r = (sinf(wave) + 1.0f) * 127.5f * pixel_brightness[i];
        uint8_t g = (sinf(wave + 2.1f) + 1.0f) * 127.5f * pixel_brightness[i];
        uint8_t b = (sinf(wave + 4.2f) + 1.0f) * 127.5f * pixel_brightness[i];

        led_strip_set_pixel(led_strip, i, r, g, b);
    }

    led_strip_refresh(led_strip);
}

// === Audio spectrum cross ===
void single_column_wave(led_strip_handle_t led_strip, int t, int col)
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
void audio_spectrum(led_strip_handle_t led_strip, int frame)
{
    static float levels[NUM_COLS] = {0}; // níveis suavizados
    int offset = 1;
    if (frame % 2 == 0)
    {
        offset = -offset; // Make it negative on even frames
    }
    for (int col = 0; col < NUM_COLS; col++)
    {
        // Simulação: onda + ruído
        float wave = sinf(frame * 0.12f - offset * col * 1.2f);
        float noise = (esp_random() % 100) / 100.0f;
        float target = (wave + 1.0f) / 2.0f * (NUM_ROWS - 1) + noise;

        // Suavização: subida rápida, descida lenta
        if (target > levels[col])
            levels[col] = target;
        else
            levels[col] *= 0.82f;

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
}

// === Tinkle starts ===
void tinkle_star_single_color(led_strip_handle_t led_strip, int frame) // colorfull single color tinkle little star
{
    led_strip_clear(led_strip);

    // Gira a cada 10 frames
    int step = (frame / 10) % 4;

    // Geração de cor suave (como color_pulse)
    float phase = (frame % 120) / 120.0f * 2 * M_PI;
    uint8_t red = (sinf(phase) + 1.0f) * 127.5f;
    uint8_t green = (sinf(phase + 2.1f) + 1.0f) * 127.5f;
    uint8_t blue = (sinf(phase + 4.2f) + 1.0f) * 127.5f;

    for (int i = 0; i < 5; i++)
    {
        int row = cross_steps[step][i][0];
        int col = cross_steps[step][i][1];
        int pixel = matrix[row][col];
        led_strip_set_pixel(led_strip, pixel, red, green, blue);
    }
    led_strip_refresh(led_strip);
}
void tinkle_star_rainbow(led_strip_handle_t led_strip, int frame) // colorfull tinkle little star
{
    led_strip_clear(led_strip);
    int step = (frame / 10) % 4;

    // Atualiza os LEDs da cruz para brilho máximo (1.0)
    for (int i = 0; i < 5; i++)
    {
        int row = cross_steps[step][i][0];
        int col = cross_steps[step][i][1];
        int pixel = matrix[row][col];
        if (pixel >= 0 && pixel < NUM_PIXELS)
            pixel_brightness[pixel] = 1.0f;
    }

    // Aplica fade aos pixels e atualiza a cor
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        // Aplica decaimento
        pixel_brightness[i] *= 0.8f;
        if (pixel_brightness[i] < 0.01f)
            pixel_brightness[i] = 0;

        // Calcula cor tipo arco-íris
        float wave = (frame + i * 3) * 0.1f;

        uint8_t r = (sinf(wave) + 1.0f) * 127.5f * pixel_brightness[i];
        uint8_t g = (sinf(wave + 2.1f) + 1.0f) * 127.5f * pixel_brightness[i];
        uint8_t b = (sinf(wave + 4.2f) + 1.0f) * 127.5f * pixel_brightness[i];

        led_strip_set_pixel(led_strip, i, r, g, b);
    }

    led_strip_refresh(led_strip);
}

// ===  Rainbow Scroll ===
void rainbow_scroll(led_strip_handle_t led_strip, int frame)
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
}

// === Color Pulse ===
void color_pulse(led_strip_handle_t led_strip, int frame)
{
    uint8_t r, g, b;
    float wave, brightness;

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
}
