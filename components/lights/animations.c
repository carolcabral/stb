#include <stdio.h>
#include "animations.h"

#include "esp_random.h"
#include "esp_log.h"

// #define M_PI 3.14159265

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

static void color_wheel(uint8_t pos, color_t *color)
{
    if (pos < 85)
    {
        color->red = pos * 3;
        color->green = 255 - pos * 3;
        color->blue = 0;
    }
    else if (pos < 170)
    {
        pos -= 85;
        color->red = 255 - pos * 3;
        color->green = 0;
        color->blue = pos * 3;
    }
    else
    {
        pos -= 170;
        color->red = 0;
        color->green = pos * 3;
        color->blue = 255 - pos * 3;
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
float pixel_brightness[CONFIG_LED_STRIP_QTD] = {0};

// === Sky of Stars ===
void sky_of_stars(led_strip_handle_t led_strip)
{
    for (int i = 0; i < CONFIG_LED_STRIP_QTD; i++)
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

// === Rotating crosses ===

// Geração de cor suave (como color_pulse)
static void get_color(int frame, color_t *color)
{
    float phase = (frame % 120) / 120.0f * 2 * M_PI;

    color->red = (sinf(phase) + 1.0f) * 127.5f;
    color->green = (sinf(phase + 2.1f) + 1.0f) * 127.5f;
    color->blue = (sinf(phase + 4.2f) + 1.0f) * 127.5f;
}

void rotating_cross_cw(led_strip_handle_t led_strip, int frame)
{
    color_t color = {0};
    get_color(frame, &color);

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
void rotating_cross_ccw(led_strip_handle_t led_strip, int frame)
{
    color_t color = {0};
    get_color(frame, &color);

    led_strip_clear(led_strip);

    // Gira a cada 10 frames
    int step = (frame / 10) % 4;
    for (int i = 0; i < CONFIG_LED_STRIP_ROWS; i++)
    {
        int row = line_steps_ccw[step][i][0];
        int col = line_steps_ccw[step][i][1];
        int pixel = matrix[row][col];
        led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue);
    }
    led_strip_refresh(led_strip);
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
        if (pixel >= 0 && pixel < CONFIG_LED_STRIP_QTD)
            pixel_brightness[pixel] = 1.0f;
    }

    // Aplica fade aos pixels e atualiza a cor
    for (int i = 0; i < CONFIG_LED_STRIP_QTD; i++)
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
    if (col < 0 || col >= CONFIG_LED_STRIP_COLS)
        return;

    // Altura simulada entre 1 e CONFIG_LED_STRIP_ROWS
    float wave = sinf(t * M_PI / 90.0f + col);
    int height = (int)((wave + 1.0f) * (CONFIG_LED_STRIP_ROWS - 1) / 2.0f + 0.5f);

    for (int row = 0; row < CONFIG_LED_STRIP_ROWS; row++)
    {
        for (int c = 0; c < CONFIG_LED_STRIP_COLS; c++)
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
    static float levels[CONFIG_LED_STRIP_COLS] = {0}; // níveis suavizados
    int offset = 1;
    if (frame % 2 == 0)
    {
        offset = -offset; // Make it negative on even frames
    }
    for (int col = 0; col < CONFIG_LED_STRIP_COLS; col++)
    {
        // Simulação: onda + ruído
        float wave = sinf(frame * 0.12f - offset * col * 1.2f);
        float noise = (esp_random() % 100) / 100.0f;
        float target = (wave + 1.0f) / 2.0f * (CONFIG_LED_STRIP_ROWS - 1) + noise;

        // Suavização: subida rápida, descida lenta
        if (target > levels[col])
            levels[col] = target;
        else
            levels[col] *= 0.82f;

        // Garante que a base (linha 0) sempre acenda
        int height = (int)(levels[col] + 0.5f); // arredonda

        for (int row = 0; row < CONFIG_LED_STRIP_ROWS; row++)
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
    color_t rgb = {
        .red = (sinf(phase) + 1.0f) * 127.5f,
        .green = (sinf(phase + 2.1f) + 1.0f) * 127.5f,
        .blue = (sinf(phase + 4.2f) + 1.0f) * 127.5f,
    };

    for (int i = 0; i < 5; i++)
    {
        int row = cross_steps[step][i][0];
        int col = cross_steps[step][i][1];
        int pixel = matrix[row][col];
        led_strip_set_pixel(led_strip, pixel, rgb.red, rgb.green, rgb.blue);
    }
    led_strip_refresh(led_strip);
}
void tinkle_star_rainbow(led_strip_handle_t led_strip, int frame) // colorfull tinkle little star
{
    int step = (frame / 10) % 4;
    int row, col, pixel;
    color_t rgb = {0};

    led_strip_clear(led_strip);
    // Atualiza os LEDs da cruz para brilho máximo (1.0)
    for (int i = 0; i < 5; i++)
    {
        row = cross_steps[step][i][0];
        col = cross_steps[step][i][1];
        pixel = matrix[row][col];
        if (pixel >= 0 && pixel < CONFIG_LED_STRIP_QTD)
            pixel_brightness[pixel] = 1.0f;
    }

    // Aplica fade aos pixels e atualiza a cor
    for (int i = 0; i < CONFIG_LED_STRIP_QTD; i++)
    {
        // Aplica decaimento
        pixel_brightness[i] *= 0.8f;
        if (pixel_brightness[i] < 0.01f)
            pixel_brightness[i] = 0;

        // Calcula cor tipo arco-íris
        float wave = (frame + i * 3) * 0.1f;

        rgb.red = (sinf(wave) + 1.0f) * 127.5f * pixel_brightness[i];
        rgb.green = (sinf(wave + 2.1f) + 1.0f) * 127.5f * pixel_brightness[i];
        rgb.blue = (sinf(wave + 4.2f) + 1.0f) * 127.5f * pixel_brightness[i];

        led_strip_set_pixel(led_strip, i, rgb.red, rgb.green, rgb.blue);
    }

    led_strip_refresh(led_strip);
}

// ===  Rainbow Scroll ===
void rainbow_scroll(led_strip_handle_t led_strip, int frame)
{
    color_t color;

    for (int col = 0; col < CONFIG_LED_STRIP_COLS; col++)
    {
        for (int row = 0; row < CONFIG_LED_STRIP_ROWS; row++)
        {
            int pixel = matrix[row][col];
            color_wheel((col * 20 + row * 10 + frame) % 256, &color);
            led_strip_set_pixel(led_strip, pixel, color.red, color.green, color.blue);
        }
    }
    led_strip_refresh(led_strip);
}

// === Color Pulse ===
void color_pulse(led_strip_handle_t led_strip, int frame)
{
    color_t color;
    float wave, brightness;

    color_wheel((frame * 2) % 256, &color);
    wave = sinf(frame * M_PI / 180.0f);
    brightness = (wave + 1.0f) / 2.0f;
    for (int i = 0; i < CONFIG_LED_STRIP_QTD; i++)
    {
        led_strip_set_pixel(led_strip, i,
                            (uint8_t)(brightness * color.red),
                            (uint8_t)(brightness * color.green),
                            (uint8_t)(brightness * color.blue));
    }
    led_strip_refresh(led_strip);
}

// ====== Fire
void anim_fire_by_frame(led_strip_handle_t led_strip, int frame)
{
    static uint8_t heat[6][5] = {0};
    const uint8_t decay_rate = 20;
    const uint8_t spark_chance = 10;
    const uint8_t max_heat = 255;

    // Dissipação rápida do calor
    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            heat[y][x] = (heat[y][x] > decay_rate) ? (heat[y][x] - decay_rate) : 0;

            // Chance de faísca explosiva
            if ((esp_random() % 100) < spark_chance)
            {
                heat[y][x] = (esp_random() % 100) + 155; // 155–255
            }
        }
    }

    led_strip_clear(led_strip);

    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            uint8_t value = heat[y][x];
            uint8_t r, g, b;

            if (value > 230)
            {
                r = 255;
                g = 255;
                b = 80; // amarelo quente
            }
            else if (value > 180)
            {
                r = 255;
                g = 180;
                b = 40; // amarelo alaranjado
            }
            else if (value > 120)
            {
                r = 255;
                g = 100;
                b = 10; // laranja intenso
            }
            else if (value > 60)
            {
                r = 200;
                g = 60;
                b = 0; // vermelho-laranja
            }
            else if (value > 0)
            {
                r = 120;
                g = 20;
                b = 0; // vermelho escuro
            }
            else
            {
                r = g = b = 0;
            }

            uint8_t pixel = matrix[y][x];
            led_strip_set_pixel(led_strip, pixel, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}
void anim_spiral_wave(led_strip_handle_t led_strip, int frame)
{
    // Ordem dos pixels da espiral, do centro para fora
    static const uint8_t spiral_pixels[] = {
        14, // Centro
        15,
        8, // Primeira camada
        9,
        10, // Segunda camada
        13,
        22,
        21,
        20, // Terceira
        19,
        16,
        7,
        4, // Quarta
        3,
        2,
        1,
        0, // Quinta
        11,
        12,
        23,
        24, // Sexta
        25,
        26,
        27,
        28,
        29,
        18,
        17,
        6,
        5, // FInal
    };
    const uint8_t spiral_len = sizeof(spiral_pixels);
    const uint8_t trail = 10;
    const uint8_t brightness = 255;
    const uint16_t total_frames = spiral_len * 2;

    // Faz a cabeça ir até o fim e voltar: forma de onda triangular
    uint16_t f = frame % total_frames;
    uint16_t head = f < spiral_len ? f : (total_frames - 1 - f);

    led_strip_clear(led_strip);

    for (uint8_t i = 0; i < spiral_len; i++)
    {
        uint8_t pixel = spiral_pixels[i];

        int dist = (int)i - (int)head;
        if (dist < 0)
            dist = -dist;

        if (dist < trail)
        {
            float factor = 1.0f - ((float)dist / trail);

            // Gradiente em tons de roxo → lilás: vermelho proporcional + azul fixo
            uint8_t r = factor * brightness;
            uint8_t g = 0;
            uint8_t b = brightness;

            led_strip_set_pixel(led_strip, pixel, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}

void anim_pineapple(led_strip_handle_t led_strip, int frame)
{
    // --- Corpo do abacaxi (amarelo/laranja) ---
    static const uint8_t body_pixels[] = {
        8, 9, 10,
        13, 14, 15,
        16, 17, 18,
        19, 20, 21};

    // --- Folhas (verde) ---
    static const uint8_t leaves_pixels[] = {
        2, 3, 4,
        7};

    // --- Base / sombra ---
    static const uint8_t base_pixels[] = {
        22, 23, 24};

    led_strip_clear(led_strip);

    // Pulso suave no brilho
    float pulse = (sinf(frame * 0.2f) + 1.0f) * 0.5f; // varia entre 0 e 1
    uint8_t body_r = (uint8_t)(200 + 55 * pulse);     // entre amarelo e laranja
    uint8_t body_g = (uint8_t)(150 + 80 * pulse);
    uint8_t body_b = 0;

    uint8_t leaf_r = 0;
    uint8_t leaf_g = (uint8_t)(120 + 100 * pulse); // verde pulsante
    uint8_t leaf_b = 0;

    uint8_t base_r = (uint8_t)(100 * pulse);
    uint8_t base_g = (uint8_t)(40 * pulse);
    uint8_t base_b = 0;

    // Corpo
    for (uint8_t i = 0; i < sizeof(body_pixels); i++)
        led_strip_set_pixel(led_strip, body_pixels[i], body_r, body_g, body_b);

    // Folhas
    for (uint8_t i = 0; i < sizeof(leaves_pixels); i++)
        led_strip_set_pixel(led_strip, leaves_pixels[i], leaf_r, leaf_g, leaf_b);

    // Base
    for (uint8_t i = 0; i < sizeof(base_pixels); i++)
        led_strip_set_pixel(led_strip, base_pixels[i], base_r, base_g, base_b);

    led_strip_refresh(led_strip);
}
void anim_pineapple_move(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    // Movimento horizontal (vai da direita para esquerda)
    int offset = CONFIG_LED_STRIP_COLS - (frame % (CONFIG_LED_STRIP_COLS + 4)); // 4 = largura + espaço

    // Pulso de brilho para dar vida
    float pulse = (sinf(frame * 0.25f) + 1.0f) * 0.5f;

    // Cores do corpo (amarelo/laranja)
    uint8_t body_r = (uint8_t)(200 + 55 * pulse);
    uint8_t body_g = (uint8_t)(150 + 80 * pulse);
    uint8_t body_b = 0;

    // Folhas (verde pulsante)
    uint8_t leaf_r = 0;
    uint8_t leaf_g = (uint8_t)(120 + 100 * pulse);
    uint8_t leaf_b = 0;

    // Base (marrom avermelhado)
    uint8_t base_r = (uint8_t)(80 + 50 * pulse);
    uint8_t base_g = (uint8_t)(40 * pulse);
    uint8_t base_b = 0;

    // === Desenha o abacaxi relativo ao offset ===
    for (int r = 0; r < CONFIG_LED_STRIP_ROWS; r++)
    {
        for (int c = 0; c < 3; c++) // largura de 3 colunas
        {
            int x = c + offset; // deslocamento horizontal
            if (x < 0 || x >= CONFIG_LED_STRIP_COLS)
                continue;

            int pixel = matrix[r][x];

            if (r == 0 || r == 1)
            {
                // Folhas no topo
                if (c == 1 || (r == 0 && c != 0 && c != 2))
                    led_strip_set_pixel(led_strip, pixel, leaf_r, leaf_g, leaf_b);
            }
            else if (r >= 2 && r <= 4)
            {
                // Corpo principal
                led_strip_set_pixel(led_strip, pixel, body_r, body_g, body_b);
            }
            else if (r == 5)
            {
                // Base
                led_strip_set_pixel(led_strip, pixel, base_r, base_g, base_b);
            }
        }
    }

    led_strip_refresh(led_strip);
}