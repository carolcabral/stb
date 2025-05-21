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
} ANIMATIONS_T;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

void anim_rotating_plus_cw(led_strip_handle_t led_strip, int delay, color_t color);
void anim_rotating_plus_ccw(led_strip_handle_t led_strip, int delay, color_t color);
void anim_rainbow_scroll(led_strip_handle_t led_strip, int delay);
void anim_color_pulse(led_strip_handle_t led_strip, int delay);

void anim_independent_column_waves(led_strip_handle_t led_strip, int t);
void anim_sky_of_stars(led_strip_handle_t led_strip, int delay);
void anim_heart(led_strip_handle_t led_strip);
void anim_audio_spectrum(led_strip_handle_t led_strip, int delay);

void anim_rotating_plus2(led_strip_handle_t led_strip, uint8_t *initial_phase, color_t color);
void anim_single_column_wave(led_strip_handle_t led_strip, int t, int col);
#endif