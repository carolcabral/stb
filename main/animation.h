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
    TINKLE_STAR_SINGLE_COLOR,
    TINKLE_START_MULTI_COLOR,
    ROTATING_LINE_SINGLE_COLOR,
    ROTATING_LINE_MULTI_COLOR,
    RAINBOW_SCROLL,
    COLOR_PULSE,

    AUDIO_SPECTRUM,
    SKY_OF_STARS,

    I_LOVE_LUCAS,
    HEART,
    I_LOVE_YOU,

    ANIMATIONS_MAX
} ANIMATIONS_T;

static const char *animations_name[] = {
    "TINKLE_STAR_SINGLE_COLOR",
    "TINKLE_START_MULTI_COLOR",
    "ROTATING_LINE_SINGLE_COLOR",
    "ROTATING_LINE_MULTI_COLOR",
    "RAINBOW_SCROLL",
    "COLOR_PULSE",
    "AUDIO_SPECTRUM",
    "SKY_OF_STARS",
    "I_LOVE_LUCAS",
    "HEART",
    "I_LOVE_YOU",

};
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

extern ANIMATIONS_T global_current_animation;

void rotating_plus_cw(led_strip_handle_t led_strip, int delay, color_t color);
void rotating_plus_ccw(led_strip_handle_t led_strip, int delay, color_t color);
void rainbow_scroll(led_strip_handle_t led_strip, int delay);
void color_pulse(led_strip_handle_t led_strip, int delay);
void tinkle_star_single_color(led_strip_handle_t led_strip, int delay); // colorfull single color tinkle little star;
void tinkle_star_rainbow(led_strip_handle_t led_strip, int delay);

void independent_column_waves(led_strip_handle_t led_strip, int t);
void sky_of_stars(led_strip_handle_t led_strip);
void heart(led_strip_handle_t led_strip, uint8_t brightness);
void i_love_lucas(led_strip_handle_t led_strip, int frame);
void growing_heart(led_strip_handle_t led_strip);
void i_love_you(led_strip_handle_t led_strip);
void anim_heart_pulse_grow(led_strip_handle_t led_strip, uint16_t frame);
void audio_spectrum(led_strip_handle_t led_strip, int delay);

void rotating_plus2(led_strip_handle_t led_strip, uint8_t *initial_phase, color_t color);
void single_column_wave(led_strip_handle_t led_strip, int t, int col);
void anim_red_finale_by_frame(led_strip_handle_t led_strip, uint16_t frame);
void rotating_cross_single_color(led_strip_handle_t led_strip, int delay, uint8_t is_cw);
void rotating_cross_rainbow_with_trail(led_strip_handle_t led_strip, int delay, uint8_t is_cw);
void sky_of_stars_with_trail(led_strip_handle_t led_strip, int delay);
void sky_of_stars_colorful_with_trail(led_strip_handle_t led_strip, int delay);

// ======================

#endif