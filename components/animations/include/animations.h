#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <math.h>
#include <stdint.h>
#include "led_strip.h"

#define NUM_ROWS 6
#define NUM_COLS 5
#define NUM_PIXELS NUM_ROWS *NUM_COLS
#define PI 3.14159265

typedef enum
{
    COLORFUL_START = 0,
    TINKLE_STAR_SINGLE_COLOR = 1,
    TINKLE_START_MULTI_COLOR = 2,
    ROTATING_LINE_SINGLE_COLOR_CW = 3,
    ROTATING_LINE_SINGLE_COLOR_CCW = 4,
    ROTATING_LINE_SINGLE_COLOR_ALTERNATING = 5,
    ROTATING_LINE_MULTI_COLOR_ALTERNATING = 6,
    ROTATING_LINE_MULTI_COLOR_CW = 7,
    ROTATING_LINE_MULTI_COLOR_CCW = 8,
    RAINBOW_SCROLL = 9,
    COLOR_PULSE = 10,
    COLORFUL_END = 11,

    WHITE_START = 12,
    SKY_OF_STARS = 13,
    WHITE_END = 14,

    AUDIO_SPECTRUM = 15,

    SPIRAL_WAVE = 16,

    FIRE = 17,

    I_LOVE_START = 200,
    I_LOVE_LUCAS = 201,
    HEART = 202,
    I_LOVE_YOU = 203,
    I_LOVE_END = 204,

    ANIMATIONS_MAX
} ANIMATIONS_T;

static const char *animations_name[] = {
    "COLORFUL_START",
    "TINKLE_STAR_SINGLE_COLOR",
    "TINKLE_START_MULTI_COLOR",
    "ROTATING_LINE_SINGLE_COLOR_CW",
    "ROTATING_LINE_SINGLE_COLOR_CCW",
    "ROTATING_LINE_SINGLE_COLOR_ALTERNATING",
    "ROTATING_LINE_MULTI_COLOR_ALTERNATING",
    "ROTATING_LINE_MULTI_COLOR_CW",
    "ROTATING_LINE_MULTI_COLOR_CCW",
    "RAINBOW_SCROLL",
    "COLOR_PULSE",
    "COLORFUL_END",
    "WHITE_START",
    "SKY_OF_STARS",
    "WHITE_END",
    "I_LOVE_START",
    "I_LOVE_LUCAS",
    "HEART",
    "I_LOVE_YOU",
    "I_LOVE_END",
    "AUDIO_SPECTRUM",
    "SPIRAL_WAVE",
    "FIRE",
    "ANIMATIONS_MAX",
};

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

extern ANIMATIONS_T global_current_animation;

// ======================
void sky_of_stars(led_strip_handle_t led_strip);
void audio_spectrum(led_strip_handle_t led_strip, int delay);
void single_column_wave(led_strip_handle_t led_strip, int t, int col);

// ======================
void rainbow_scroll(led_strip_handle_t led_strip, int delay);
void color_pulse(led_strip_handle_t led_strip, int delay);

// ======================
void rotating_plus_cw(led_strip_handle_t led_strip, int delay, color_t color);
void rotating_plus_ccw(led_strip_handle_t led_strip, int delay, color_t color);
void rotating_cross_single_color(led_strip_handle_t led_strip, int delay, uint8_t is_cw);
void rotating_cross_rainbow_with_trail(led_strip_handle_t led_strip, int delay, uint8_t is_cw);

// ======================
void tinkle_star_single_color(led_strip_handle_t led_strip, int delay); // colorfull single color tinkle little star;
void tinkle_star_rainbow(led_strip_handle_t led_strip, int delay);
// ======================
void heart(led_strip_handle_t led_strip, uint8_t brightness);
void growing_heart(led_strip_handle_t led_strip);
void i_love_you(led_strip_handle_t led_strip);
void i_love_lucas(led_strip_handle_t led_strip, int frame);
// ======================

void anim_fire_by_frame(led_strip_handle_t led_strip, int frame);
void anim_spiral_wave(led_strip_handle_t led_strip, uint16_t frame);
#endif