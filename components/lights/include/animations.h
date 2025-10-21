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

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} color_t;

#endif