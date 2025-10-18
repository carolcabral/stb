#include "esp_log.h"
#include "animations.h"
#include "timer.h"

extern TaskHandle_t animations_task_handle;

const char message[] = "I h LUCAS ";
const uint8_t font[][CONFIG_LED_STRIP_ROWS] = {
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

static void heart(led_strip_handle_t led_strip, uint8_t brightness)
{
    static uint8_t heart_pixels[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    for (uint8_t i = 0; i < sizeof(heart_pixels); i++)
        led_strip_set_pixel(led_strip, heart_pixels[i], brightness * 1, 0, 0);

    led_strip_refresh(led_strip);
}

static void gradient_heart(led_strip_handle_t led_strip)
{
    int delay = 100;

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
}

void growing_heart_orig(led_strip_handle_t led_strip)
{
    // Growing heart
    uint8_t brightness = 255;
    int delay = 100;
    static uint8_t heart_pixels0[] = {14};
    static uint8_t heart_pixels1[] = {7, 8, 9, 13, 15, 19, 20, 21};
    static uint8_t heart_pixels2[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    static uint8_t heart_pixels3[] = {1, 5, 17, 23, 25, 29};

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

    gradient_heart(led_strip);

    for (uint8_t i = 0; i < sizeof(heart_pixels3); i++)
        led_strip_set_pixel(led_strip, heart_pixels3[i], brightness * 1, 0, 0);
    led_strip_refresh(led_strip);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    led_strip_clear(led_strip);
}

void i_love_you_orig(led_strip_handle_t led_strip)
{
    // Growing heart
    uint8_t brightness = 255;
    int delay = 100;

    static uint8_t i_pixels[] = {6, 11, 12, 13, 14, 15, 16, 17, 18, 23};
    static uint8_t u_pixels[] = {6, 7, 8, 9, 10, 11, 12, 18, 19, 20, 21, 22, 23};

    // I
    for (uint8_t i = 0; i < sizeof(i_pixels); i++)
        led_strip_set_pixel(led_strip, i_pixels[i], brightness * 1, 0, 0);
    led_strip_refresh(led_strip);
    vTaskDelay(delay * 10 / portTICK_PERIOD_MS);
    led_strip_clear(led_strip);

    // LOVE
    gradient_heart(led_strip);
    // U
    for (uint8_t i = 0; i < sizeof(u_pixels); i++)
        led_strip_set_pixel(led_strip, u_pixels[i], brightness * 1, 0, 0);
    led_strip_refresh(led_strip);
    vTaskDelay(delay * 10 / portTICK_PERIOD_MS);
    led_strip_clear(led_strip);
}

// ==================
void i_love_you(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    // Message to display — 'h' is the heart symbol
    static const char message[] = "IhU";

    const int num_chars = sizeof(message) - 1;
    const int char_spacing = 1; // 1 column spacing between characters
    const int total_width = num_chars * (CONFIG_LED_STRIP_COLS + char_spacing);

    // Controls scroll speed
    int scroll_speed = 2;
    int scroll_offset = (frame / scroll_speed) % (total_width + CONFIG_LED_STRIP_COLS);

    // Draw each character
    for (int char_index = 0; char_index < num_chars; char_index++)
    {
        char c = message[char_index];
        int glyph = 7; // default = blank

        switch (c)
        {
        case 'I':
            glyph = 0;
            break;
        case 'h':
            glyph = 1;
            break; // heart
        case 'U':
            glyph = 3;
            break;
        default:
            break;
        }

        // Calculate position on the matrix
        int char_x = char_index * (CONFIG_LED_STRIP_COLS + char_spacing) - scroll_offset;

        // Skip if completely out of bounds
        if (char_x + CONFIG_LED_STRIP_COLS < 0 || char_x >= CONFIG_LED_STRIP_COLS)
            continue;

        for (int row = 0; row < CONFIG_LED_STRIP_ROWS; row++)
        {
            int mapped_row = CONFIG_LED_STRIP_ROWS - 1 - row; // flip vertically if needed

            for (int col = 0; col < CONFIG_LED_STRIP_COLS; col++)
            {
                if (char_x + col < 0 || char_x + col >= CONFIG_LED_STRIP_COLS)
                    continue;

                if (font[glyph][row] & (1 << (CONFIG_LED_STRIP_COLS - 1 - col)))
                {
                    int led = matrix[mapped_row][char_x + col];
                    uint8_t r = (c == 'h') ? 255 : 255; // heart and letters both red for now
                    uint8_t g = (c == 'h') ? 0 : 0;
                    uint8_t b = (c == 'h') ? 0 : 0;
                    led_strip_set_pixel(led_strip, led, r, g, b);
                }
            }
        }
    }

    led_strip_refresh(led_strip);
}

void growing_heart(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    uint8_t brightness = 255;
    int phase = frame % 60;

    static const uint8_t heart_pixels0[] = {14};
    static const uint8_t heart_pixels1[] = {7, 8, 9, 13, 15, 19, 20, 21};
    static const uint8_t heart_pixels2[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
    static const uint8_t heart_pixels3[] = {1, 5, 17, 23, 25, 29};

    // Mostra fases do coração
    if (phase < 10)
    {
        for (uint8_t i = 0; i < sizeof(heart_pixels0); i++)
            led_strip_set_pixel(led_strip, heart_pixels0[i], brightness, 0, 0);
    }
    else if (phase < 20)
    {
        for (uint8_t i = 0; i < sizeof(heart_pixels1); i++)
            led_strip_set_pixel(led_strip, heart_pixels1[i], brightness, 0, 0);
    }
    else if (phase < 30)
    {
        for (uint8_t i = 0; i < sizeof(heart_pixels2); i++)
            led_strip_set_pixel(led_strip, heart_pixels2[i], brightness, 0, 0);
    }
    else if (phase < 40)
    {
        for (uint8_t i = 0; i < sizeof(heart_pixels3); i++)
            led_strip_set_pixel(led_strip, heart_pixels3[i], brightness, 0, 0);
    }
    else
    {
        // Fade pulsante
        uint8_t pulse = abs((phase - 40) * 12 - 120);
        float f = (float)pulse / 120.0f;
        uint8_t br = (uint8_t)(brightness * f);

        static const uint8_t heart_full[] = {2, 3, 4, 6, 10, 12, 16, 18, 22, 26, 27, 28};
        for (uint8_t i = 0; i < sizeof(heart_full); i++)
            led_strip_set_pixel(led_strip, heart_full[i], br, 0, 0);
    }

    led_strip_refresh(led_strip);
}

void i_love_lucas(led_strip_handle_t led_strip, int frame)
{
    led_strip_clear(led_strip);

    const int num_chars = sizeof(message) - 1;                       // sem o '\0'
    const int total_width = num_chars * (CONFIG_LED_STRIP_COLS + 1); // 1 coluna de espaço entre letras

    int scroll_offset = (frame / 2) % (total_width + CONFIG_LED_STRIP_COLS); // desliza da direita para esquerda

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

        int char_x = char_index * (CONFIG_LED_STRIP_COLS + 1) - scroll_offset;

        // Desenha caractere se estiver visível
        if (char_x + CONFIG_LED_STRIP_COLS < 0 || char_x >= CONFIG_LED_STRIP_COLS)
            continue;

        for (int row = 0; row < CONFIG_LED_STRIP_ROWS && row < CONFIG_LED_STRIP_ROWS; row++)
        {
            int mapped_row = CONFIG_LED_STRIP_ROWS - 1 - row;

            for (int col = 0; col < CONFIG_LED_STRIP_COLS; col++)
            {
                if (char_x + col < 0 || char_x + col >= CONFIG_LED_STRIP_COLS)
                    continue;

                if (font[glyph][row] & (1 << (CONFIG_LED_STRIP_COLS - 1 - col)))
                {
                    int led = matrix[mapped_row][char_x + col];
                    led_strip_set_pixel(led_strip, led, 255, 0, 0);
                }
            }
        }
    }

    led_strip_refresh(led_strip);
}

#if 0
void love_animation_task(timer_ctx_t *timer)
{
    anim_ctx_t love_animation[] = {
        {"i_love_you", &i_love_you, 100},
        {"growing_heart", &growing_heart, 20},
        {"i_love_lucas", &i_love_lucas, 50},
    };

    uint8_t max_animations = sizeof(love_animation) / sizeof(anim_ctx_t);
    int index = 0, frame = 0;
    double current_time = 0;

    while (index < max_animations) // Passa apenas uma vez a animação
    {
        current_time = get_current_time(timer) + 1;
        if ((int)current_time % ANIMATION_INTERVAL_IN_SEC_CONTROLLED == 0)
        {
            index++;
            clear_timer(timer);
            printf("%d: LOVE LOVE AMOR%d / %d: %s\n", (int)current_time, index, max_animations, love_animation[index].name);
        }

        love_animation[index].func(led_strip, frame++);
        vTaskDelay(love_animation[index].delay / portTICK_PERIOD_MS);
    }
}
#endif
// === WATER ANIMATIONS ===/
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

const anim_ctx_t special_love_animation[] = {
    {"i_love_you", &i_love_you, 100},
    {"growing_heart", &growing_heart, 20},
    {"i_love_lucas", &i_love_lucas, 50},
};
const anim_ctx_t special_water_animation[] = {
    {"WATER", &anim_water, 20},
    {"WATER", &anim_water_spiral_blues, 20},
    // {"WATER", &anim_water_spiral, 20},
};

uint8_t num_love = sizeof(special_love_animation) / sizeof(anim_ctx_t);
uint8_t num_water = sizeof(special_water_animation) / sizeof(anim_ctx_t);

void run_requested_animations(const anim_ctx_t *anim, uint8_t num, timer_ctx_t *timer)
{
    printf("RUn requested num: %d\n", num);
    int index = 0, frame = 0;
    double current_time = 0;

    while (index < num)
    {
        current_time = get_current_time(timer) + 1;
        if ((int)current_time % ANIMATION_INTERVAL_IN_SEC_CONTROLLED == 0)
        {
            index++;
            clear_timer(timer);
            printf("%d: LOVE LOVE AMOR%d / %d: %s\n", (int)current_time, index, num, anim[index].name);
        }

        anim[index].func(led_strip, frame++);
        vTaskDelay(anim[index].delay / portTICK_PERIOD_MS);
    }
}
