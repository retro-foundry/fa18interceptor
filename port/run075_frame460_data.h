#ifndef FA18_RUN075_FRAME460_DATA_H
#define FA18_RUN075_FRAME460_DATA_H

#include <stdint.h>

typedef struct { uint16_t y, x, length; const uint16_t *pixels; } FA18Frame460Span;

enum { FA18_RUN075_FRAME460_SPANS = 10 };
static const uint16_t fa18_frame460_pixels_0[] = {0xd92};
static const uint16_t fa18_frame460_pixels_1[] = {0xd92};
static const uint16_t fa18_frame460_pixels_2[] = {0xd92};
static const uint16_t fa18_frame460_pixels_3[] = {0xd92};
static const uint16_t fa18_frame460_pixels_4[] = {0xd92};
static const uint16_t fa18_frame460_pixels_5[] = {0xd92};
static const uint16_t fa18_frame460_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_frame460_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_frame460_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_frame460_pixels_9[] = {0xd92,0xd92};

static const FA18Frame460Span fa18_run075_frame460_spans[] = {
    {26, 71, 1, fa18_frame460_pixels_0},
    {26, 75, 1, fa18_frame460_pixels_1},
    {27, 71, 1, fa18_frame460_pixels_2},
    {27, 75, 1, fa18_frame460_pixels_3},
    {28, 71, 1, fa18_frame460_pixels_4},
    {28, 75, 1, fa18_frame460_pixels_5},
    {29, 71, 5, fa18_frame460_pixels_6},
    {30, 73, 2, fa18_frame460_pixels_7},
    {31, 73, 2, fa18_frame460_pixels_8},
    {32, 73, 2, fa18_frame460_pixels_9},
};

#endif
