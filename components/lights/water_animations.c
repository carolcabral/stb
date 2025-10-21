#include "esp_log.h"
#include "animations.h"
#include "timer.h"
#include <string.h>

// === WATER ANIMATIONS ===/
// Font for 5x6 letters (H, I, D, R, A, T, E, S, -, SPACE)
static const uint8_t font[][6] = {
    // H
    {0b10001,
     0b10001,
     0b11111,
     0b10001,
     0b10001,
     0b10001},

    // I
    {0b11111,
     0b00100,
     0b00100,
     0b00100,
     0b00100,
     0b11111},

    // D
    {0b11110,
     0b10001,
     0b10001,
     0b10001,
     0b10001,
     0b11110},

    // R
    {0b11110,
     0b10001,
     0b11110,
     0b10100,
     0b10010,
     0b10001},

    // A
    {0b01110,
     0b10001,
     0b10001,
     0b11111,
     0b10001,
     0b10001},

    // T
    {0b11111,
     0b00100,
     0b00100,
     0b00100,
     0b00100,
     0b00100},

    // E
    {0b11111,
     0b10000,
     0b11110,
     0b10000,
     0b10000,
     0b11111},

    // S
    {0b01111,
     0b10000,
     0b01110,
     0b00001,
     0b10001,
     0b01110},

    // Dash (-)
    {0b00000,
     0b00000,
     0b01110,
     0b00000,
     0b00000,
     0b00000},

    // Space
    {0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000},
};

void anim_water(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const float speed = 0.15f;    // wave speed
    const float frequency = 0.5f; // horizontal frequency
    const float phase = frame * speed;
    const float wave_height = 1.5f; // vertical displacement amplitude

    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            // Create a 2D wave pattern
            float v = sinf(frequency * x + phase) + cosf(frequency * y + phase * 0.7f);
            v = (v + 2.0f) / 4.0f; // normalize 0..1

            // Add small “sparkle” ripples
            float ripple = sinf((x + y + frame * 0.2f) * 0.7f);
            v += 0.1f * ripple;
            if (v > 1.0f)
                v = 1.0f;
            if (v < 0.0f)
                v = 0.0f;

            // Color gradient: deep blue → cyan → white highlights
            uint8_t r = (uint8_t)(10 + 30 * v);
            uint8_t g = (uint8_t)(80 + 160 * v);
            uint8_t b = (uint8_t)(150 + 105 * v);

            // Light shimmer based on time and position
            float shimmer = 0.5f + 0.5f * sinf(frame * 0.2f + x * 0.8f + y * 0.5f);
            r = (uint8_t)(r * shimmer);
            g = (uint8_t)(g * shimmer);
            b = (uint8_t)(b * shimmer);

            int led = matrix[y][x];
            led_strip_set_pixel(led_strip, led, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}
void anim_water_spiral(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const float wave_speed = 0.15f;
    const float spiral_speed = 0.25f;
    const float frequency = 0.6f;
    const float phase = frame * wave_speed;

    static const uint8_t spiral_pixels[] = {
        14, // Centro
        15,
        8,
        9,
        10,
        13,
        22,
        21,
        20,
        19,
        16,
        7,
        4,
        3,
        2,
        1,
        0,
        11,
        12,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        18,
        17,
        6,
        5,
    };
    const uint8_t spiral_len = sizeof(spiral_pixels);

    // Compute spiral brightness modulation
    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            float v = sinf(frequency * x + phase) + cosf(frequency * y + phase * 0.7f);
            v = (v + 2.0f) / 4.0f; // normalize 0..1

            // Base water colors (deep blue → cyan → white)
            uint8_t r = (uint8_t)(20 + 30 * v);
            uint8_t g = (uint8_t)(80 + 160 * v);
            uint8_t b = (uint8_t)(120 + 135 * v);

            // Spiral effect – brightness pulse along spiral order
            int idx = (x + y * CONFIG_LED_STRIP_COLS) % spiral_len;
            int spiral_pixel = spiral_pixels[idx];
            float spiral_phase = (float)(frame * spiral_speed + idx * 0.5f);
            float spiral_bright = 0.5f + 0.5f * sinf(spiral_phase);

            // Combine wave + spiral brightness
            float brightness = v * 0.6f + spiral_bright * 0.8f;
            if (brightness > 1.0f)
                brightness = 1.0f;

            r = (uint8_t)(r * brightness);
            g = (uint8_t)(g * brightness);
            b = (uint8_t)(b * brightness);

            int led = matrix[y][x];
            led_strip_set_pixel(led_strip, led, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}
void anim_water_spiral_blues(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const float wave_speed = 0.12f;
    const float spiral_speed = 0.25f;
    const float frequency = 0.6f;
    const float phase = frame * wave_speed;

    static const uint8_t spiral_pixels[] = {
        14,
        15,
        8,
        9,
        10,
        13,
        22,
        21,
        20,
        19,
        16,
        7,
        4,
        3,
        2,
        1,
        0,
        11,
        12,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        18,
        17,
        6,
        5,
    };
    const uint8_t spiral_len = sizeof(spiral_pixels);

    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            float v = sinf(frequency * x + phase) + cosf(frequency * y + phase * 0.7f);
            v = (v + 2.0f) / 4.0f; // normalize 0..1

            // --- Dynamic blue shades ---
            // hue shift over time
            float hue_shift = 0.6f + 0.2f * sinf(phase + (x + y) * 0.3f);

            // Blend several shades of blue
            uint8_t r = (uint8_t)(10 + 30 * hue_shift * v);
            uint8_t g = (uint8_t)(40 + 150 * v * hue_shift);
            uint8_t b = (uint8_t)(100 + 155 * v);

            // --- Spiral brightness modulation ---
            int idx = (x + y * CONFIG_LED_STRIP_COLS) % spiral_len;
            float spiral_phase = (float)(frame * spiral_speed + idx * 0.4f);
            float spiral_bright = 0.5f + 0.5f * sinf(spiral_phase);

            // Combine wave + spiral light intensity
            float brightness = v * 0.5f + spiral_bright * 0.9f;
            if (brightness > 1.0f)
                brightness = 1.0f;

            r = (uint8_t)(r * brightness);
            g = (uint8_t)(g * brightness);
            b = (uint8_t)(b * brightness);

            int led = matrix[y][x];
            led_strip_set_pixel(led_strip, led, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}
void anim_water_spiral_breathing(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const float wave_speed = 0.12f;
    const float spiral_speed = 0.25f;
    const float frequency = 0.6f;
    const float phase = frame * wave_speed;

    // Spiral order (center → outward)
    static const uint8_t spiral_pixels[] = {
        14,
        15,
        8,
        9,
        10,
        13,
        22,
        21,
        20,
        19,
        16,
        7,
        4,
        3,
        2,
        1,
        0,
        11,
        12,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        18,
        17,
        6,
        5,
    };
    const uint8_t spiral_len = sizeof(spiral_pixels);

    // Alternate spiral direction every 5 seconds (assuming ~30 FPS)
    int direction_period = 150; // ~5 seconds
    bool reverse = ((frame / direction_period) % 2) == 1;

    for (int y = 0; y < CONFIG_LED_STRIP_ROWS; y++)
    {
        for (int x = 0; x < CONFIG_LED_STRIP_COLS; x++)
        {
            // Wave pattern for water motion
            float v = sinf(frequency * x + phase) + cosf(frequency * y + phase * 0.7f);
            v = (v + 2.0f) / 4.0f; // Normalize 0..1

            // Subtle hue shift across blues
            float hue_shift = 0.6f + 0.2f * sinf(phase + (x + y) * 0.3f);

            // Blend of multiple blue shades
            uint8_t r = (uint8_t)(15 + 25 * hue_shift * v);
            uint8_t g = (uint8_t)(40 + 160 * v * hue_shift);
            uint8_t b = (uint8_t)(120 + 135 * v);

            // Spiral index
            int idx = (x + y * CONFIG_LED_STRIP_COLS) % spiral_len;
            if (reverse)
                idx = spiral_len - 1 - idx;

            // Spiral “breathing” pulse
            float spiral_phase = (float)(frame * spiral_speed + idx * 0.4f);
            float spiral_bright = 0.5f + 0.5f * sinf(spiral_phase);

            // Combine wave + spiral motion
            float brightness = v * 0.5f + spiral_bright * 0.9f;
            if (brightness > 1.0f)
                brightness = 1.0f;

            // Apply brightness to color
            r = (uint8_t)(r * brightness);
            g = (uint8_t)(g * brightness);
            b = (uint8_t)(b * brightness);

            int led = matrix[y][x];
            led_strip_set_pixel(led_strip, led, r, g, b);
        }
    }

    led_strip_refresh(led_strip);
}

void anim_hidrate_se(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    // Mensagem a ser exibida
    static const char *message = " HIDRATE-SE";

    // Cor e brilho pulsante
    float pulse = (sinf(frame * 0.1f) + 1.0f) * 0.5f;
    uint8_t blue_level = (uint8_t)(150 + 100 * pulse);
    uint8_t r = 0;
    uint8_t g = (uint8_t)(30 + 40 * pulse);
    uint8_t b = blue_level;

    const int num_chars = strlen(message);
    const int char_width = CONFIG_LED_STRIP_COLS + 1;
    const int total_width = num_chars * char_width;

    // Scroll da direita para a esquerda
    int scroll_offset = (frame / 2) % (total_width + CONFIG_LED_STRIP_COLS);

    for (int char_index = 0; char_index < num_chars; char_index++)
    {
        char c = message[char_index];
        int glyph = 7; // espaço padrão

        // Mapeamento dos caracteres na fonte
        switch (c)
        {
        case 'H':
            glyph = 0;
            break;
        case 'I':
            glyph = 1;
            break;
        case 'D':
            glyph = 2;
            break;
        case 'R':
            glyph = 3;
            break;
        case 'A':
            glyph = 4;
            break;
        case 'T':
            glyph = 5;
            break;
        case 'E':
            glyph = 6;
            break;
        case 'S':
            glyph = 7;
            break;
        case '-':
            glyph = 8;
            break;
        case ' ':
            glyph = 9;
            break;
        }

        int char_x = char_index * char_width - scroll_offset;

        // Só desenha caracteres visíveis
        if (char_x + CONFIG_LED_STRIP_COLS < 0 || char_x >= CONFIG_LED_STRIP_COLS)
            continue;

        for (int row = 0; row < CONFIG_LED_STRIP_ROWS; row++)
        {
            int mapped_row = CONFIG_LED_STRIP_ROWS - 1 - row;

            for (int col = 0; col < CONFIG_LED_STRIP_COLS; col++)
            {
                if (char_x + col < 0 || char_x + col >= CONFIG_LED_STRIP_COLS)
                    continue;

                if (font[glyph][row] & (1 << (CONFIG_LED_STRIP_COLS - 1 - col)))
                {
                    int led = matrix[mapped_row][char_x + col];
                    led_strip_set_pixel(led_strip, led, r, g, b);
                }
            }
        }
    }

    led_strip_refresh(led_strip);
}

const anim_ctx_t special_water_animation[] = {
    {"WATER_SPIRAL_BLUE", &anim_water_spiral_blues, 20},
    {"WATER_HIDRATE_SE", &anim_hidrate_se, 50},
    {"WATER_SPIRAL_BREATHING", &anim_water_spiral_breathing, 20},
};
uint8_t num_water = sizeof(special_water_animation) / sizeof(anim_ctx_t);
