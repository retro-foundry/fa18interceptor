#ifndef FA18_RUN075_FRAME462_DATA_H
#define FA18_RUN075_FRAME462_DATA_H

#include <stdint.h>

typedef struct { uint16_t y, x, length; const uint16_t *pixels; } FA18Frame462Span;

enum { FA18_RUN075_FRAME462_SPANS = 12 };
static const uint16_t fa18_frame462_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_frame462_pixels_1[] = {0xd92};
static const uint16_t fa18_frame462_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_frame462_pixels_3[] = {0xd92};
static const uint16_t fa18_frame462_pixels_4[] = {0xd92};
static const uint16_t fa18_frame462_pixels_5[] = {0xd92};
static const uint16_t fa18_frame462_pixels_6[] = {0xd92};
static const uint16_t fa18_frame462_pixels_7[] = {0xd92};
static const uint16_t fa18_frame462_pixels_8[] = {0xd92};
static const uint16_t fa18_frame462_pixels_9[] = {0xd92};
static const uint16_t fa18_frame462_pixels_10[] = {0xd92};
static const uint16_t fa18_frame462_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};

static const FA18Frame462Span fa18_run075_frame462_spans[] = {
    {26, 78, 5, fa18_frame462_pixels_0},
    {27, 78, 1, fa18_frame462_pixels_1},
    {27, 81, 2, fa18_frame462_pixels_2},
    {28, 78, 1, fa18_frame462_pixels_3},
    {28, 82, 1, fa18_frame462_pixels_4},
    {29, 78, 1, fa18_frame462_pixels_5},
    {29, 82, 1, fa18_frame462_pixels_6},
    {30, 78, 1, fa18_frame462_pixels_7},
    {30, 82, 1, fa18_frame462_pixels_8},
    {31, 78, 1, fa18_frame462_pixels_9},
    {31, 82, 1, fa18_frame462_pixels_10},
    {32, 78, 5, fa18_frame462_pixels_11},
};

#endif
