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
    AUDIO_SPECTRUM,
    HEART,
    SKY_OF_STARS,
    ANIMATIONS_MAX
} ANIMATIONS_T;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

extern ANIMATIONS_T global_current_animation;
static const char *animations_name[] = {
    "AUDIO_SPECTRUM",
    "HEART",
    "SKY_OF_STARS",
};

void rotating_plus_cw(led_strip_handle_t led_strip, int delay, color_t color);
void rotating_plus_ccw(led_strip_handle_t led_strip, int delay, color_t color);
void rainbow_scroll(led_strip_handle_t led_strip, int delay);
void color_pulse(led_strip_handle_t led_strip, int delay);
void tinkle_star_single_color(led_strip_handle_t led_strip, int delay); // colorfull single color tinkle little star;
void tinkle_star_rainbow(led_strip_handle_t led_strip, int delay);

void independent_column_waves(led_strip_handle_t led_strip, int t);
void sky_of_stars(led_strip_handle_t led_strip, int delay);
void heart(led_strip_handle_t led_strip);
void audio_spectrum(led_strip_handle_t led_strip, int delay);

void rotating_plus2(led_strip_handle_t led_strip, uint8_t *initial_phase, color_t color);
void single_column_wave(led_strip_handle_t led_strip, int t, int col);

void rotating_cross_single_color(led_strip_handle_t led_strip, int delay);
void rotating_cross_rainbow(led_strip_handle_t led_strip, int delay);
void rotating_cross_rainbow_with_trail(led_strip_handle_t led_strip, int delay);
void sky_of_stars_with_trail(led_strip_handle_t led_strip, int delay);
void sky_of_stars_colorful_with_trail(led_strip_handle_t led_strip, int delay);

void rotating_cross_single_color(led_strip_handle_t led_strip, int delay);
#endif