#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <math.h>
#include <stdint.h>
#include "lights.h"

#define ANIMATION_INTERVAL_IN_SEC_DEFAULT 10
#define ANIMATION_INTERVAL_IN_SEC_CONTROLLED 20

#define REQUEST_ANIM_LOVE 255
#define REQUEST_ANIM_WATER 254

typedef void (*animation_func_t)(led_strip_handle_t, int);

typedef struct
{
    const char name[30];
    animation_func_t func;
    uint8_t delay;
} anim_ctx_t;

// ======================
void audio_spectrum(led_strip_handle_t led_strip, int delay);
void sky_of_stars(led_strip_handle_t led_strip);
void single_column_wave(led_strip_handle_t led_strip, int t, int col);

// ======================
void rainbow_scroll(led_strip_handle_t led_strip, int delay);
void color_pulse(led_strip_handle_t led_strip, int delay);

// ======================
void rotating_cross_cw(led_strip_handle_t led_strip, int frame);
void rotating_cross_ccw(led_strip_handle_t led_strip, int frame);
void rotating_cross_rainbow_with_trail(led_strip_handle_t led_strip, int delay, uint8_t is_cw);

// ======================
void tinkle_star_single_color(led_strip_handle_t led_strip, int delay); // colorfull single color tinkle little star;
void tinkle_star_rainbow(led_strip_handle_t led_strip, int delay);

void anim_fire_by_frame(led_strip_handle_t led_strip, int frame);
void anim_spiral_wave(led_strip_handle_t led_strip, int frame);

void anim_pineapple(led_strip_handle_t led_strip, int frame);
void anim_pineapple_move(led_strip_handle_t led_strip, int frame);

// void love_animation_task(void *arg)
// void anim_water(led_strip_handle_t led_strip, int frame);
// void anim_water_spiral(led_strip_handle_t led_strip, int frame);
// void anim_water_spiral_blues(led_strip_handle_t led_strip, int frame);
// void create_love_animation();

#endif