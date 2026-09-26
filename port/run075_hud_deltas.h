#ifndef FA18_RUN075_HUD_DELTAS_H
#define FA18_RUN075_HUD_DELTAS_H

#include <stdint.h>

typedef struct { uint16_t y, x, length; const uint16_t *pixels; } FA18HudDeltaSpan;
typedef struct { uint32_t frame; uint16_t span_count; const FA18HudDeltaSpan *spans; } FA18HudDelta;

/* Frame 464: 13 changed runs. */
static const uint16_t fa18_hud_464_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_464_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_464_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_464_pixels_11[] = {0xd92};
static const uint16_t fa18_hud_464_pixels_12[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_464_spans[] = {
    {26, 85, 1, fa18_hud_464_pixels_0},
    {26, 89, 1, fa18_hud_464_pixels_1},
    {27, 85, 1, fa18_hud_464_pixels_2},
    {27, 89, 1, fa18_hud_464_pixels_3},
    {28, 85, 1, fa18_hud_464_pixels_4},
    {28, 89, 1, fa18_hud_464_pixels_5},
    {29, 85, 2, fa18_hud_464_pixels_6},
    {29, 89, 1, fa18_hud_464_pixels_7},
    {30, 85, 2, fa18_hud_464_pixels_8},
    {30, 89, 1, fa18_hud_464_pixels_9},
    {31, 85, 2, fa18_hud_464_pixels_10},
    {31, 89, 1, fa18_hud_464_pixels_11},
    {32, 85, 5, fa18_hud_464_pixels_12},
};
/* Frame 466: 12 changed runs. */
static const uint16_t fa18_hud_466_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_466_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_466_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_466_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_466_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_466_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_466_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_466_spans[] = {
    {26, 92, 4, fa18_hud_466_pixels_0},
    {27, 92, 1, fa18_hud_466_pixels_1},
    {27, 95, 1, fa18_hud_466_pixels_2},
    {28, 92, 1, fa18_hud_466_pixels_3},
    {28, 95, 1, fa18_hud_466_pixels_4},
    {29, 92, 5, fa18_hud_466_pixels_5},
    {30, 92, 2, fa18_hud_466_pixels_6},
    {30, 96, 1, fa18_hud_466_pixels_7},
    {31, 92, 2, fa18_hud_466_pixels_8},
    {31, 96, 1, fa18_hud_466_pixels_9},
    {32, 92, 2, fa18_hud_466_pixels_10},
    {32, 96, 1, fa18_hud_466_pixels_11},
};
/* Frame 468: 13 changed runs. */
static const uint16_t fa18_hud_468_pixels_0[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_9[] = {0xd92,0xd92};
static const uint16_t fa18_hud_468_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_11[] = {0xd92};
static const uint16_t fa18_hud_468_pixels_12[] = {0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_468_spans[] = {
    {26, 106, 2, fa18_hud_468_pixels_0},
    {26, 110, 1, fa18_hud_468_pixels_1},
    {27, 106, 2, fa18_hud_468_pixels_2},
    {27, 110, 1, fa18_hud_468_pixels_3},
    {28, 106, 2, fa18_hud_468_pixels_4},
    {28, 110, 1, fa18_hud_468_pixels_5},
    {29, 106, 2, fa18_hud_468_pixels_6},
    {29, 110, 1, fa18_hud_468_pixels_7},
    {30, 106, 2, fa18_hud_468_pixels_8},
    {30, 109, 2, fa18_hud_468_pixels_9},
    {31, 107, 1, fa18_hud_468_pixels_10},
    {31, 109, 1, fa18_hud_468_pixels_11},
    {32, 107, 3, fa18_hud_468_pixels_12},
};
/* Frame 470: 7 changed runs. */
static const uint16_t fa18_hud_470_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_470_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_470_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_470_pixels_3[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_470_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_470_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_470_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_470_spans[] = {
    {26, 113, 5, fa18_hud_470_pixels_0},
    {27, 113, 1, fa18_hud_470_pixels_1},
    {28, 113, 1, fa18_hud_470_pixels_2},
    {29, 113, 4, fa18_hud_470_pixels_3},
    {30, 113, 2, fa18_hud_470_pixels_4},
    {31, 113, 2, fa18_hud_470_pixels_5},
    {32, 113, 5, fa18_hud_470_pixels_6},
};
/* Frame 472: 9 changed runs. */
static const uint16_t fa18_hud_472_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_472_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_472_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_472_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_472_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_472_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_472_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_472_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_472_pixels_8[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_472_spans[] = {
    {26, 120, 5, fa18_hud_472_pixels_0},
    {27, 120, 1, fa18_hud_472_pixels_1},
    {27, 124, 1, fa18_hud_472_pixels_2},
    {28, 120, 1, fa18_hud_472_pixels_3},
    {29, 120, 2, fa18_hud_472_pixels_4},
    {30, 120, 2, fa18_hud_472_pixels_5},
    {31, 120, 2, fa18_hud_472_pixels_6},
    {31, 124, 1, fa18_hud_472_pixels_7},
    {32, 120, 5, fa18_hud_472_pixels_8},
};
/* Frame 474: 7 changed runs. */
static const uint16_t fa18_hud_474_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_474_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_474_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_474_pixels_3[] = {0xd92,0xd92};
static const uint16_t fa18_hud_474_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_474_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_474_pixels_6[] = {0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_474_spans[] = {
    {26, 127, 5, fa18_hud_474_pixels_0},
    {27, 129, 1, fa18_hud_474_pixels_1},
    {28, 129, 1, fa18_hud_474_pixels_2},
    {29, 129, 2, fa18_hud_474_pixels_3},
    {30, 129, 2, fa18_hud_474_pixels_4},
    {31, 129, 2, fa18_hud_474_pixels_5},
    {32, 129, 2, fa18_hud_474_pixels_6},
};
/* Frame 476: 12 changed runs. */
static const uint16_t fa18_hud_476_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_476_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_476_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_476_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_476_spans[] = {
    {26, 134, 5, fa18_hud_476_pixels_0},
    {27, 134, 1, fa18_hud_476_pixels_1},
    {27, 137, 2, fa18_hud_476_pixels_2},
    {28, 134, 1, fa18_hud_476_pixels_3},
    {28, 138, 1, fa18_hud_476_pixels_4},
    {29, 134, 1, fa18_hud_476_pixels_5},
    {29, 138, 1, fa18_hud_476_pixels_6},
    {30, 134, 1, fa18_hud_476_pixels_7},
    {30, 138, 1, fa18_hud_476_pixels_8},
    {31, 134, 1, fa18_hud_476_pixels_9},
    {31, 138, 1, fa18_hud_476_pixels_10},
    {32, 134, 5, fa18_hud_476_pixels_11},
};
/* Frame 478: 12 changed runs. */
static const uint16_t fa18_hud_478_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_478_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_478_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_478_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_478_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_478_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_478_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_478_spans[] = {
    {26, 141, 4, fa18_hud_478_pixels_0},
    {27, 141, 1, fa18_hud_478_pixels_1},
    {27, 144, 1, fa18_hud_478_pixels_2},
    {28, 141, 1, fa18_hud_478_pixels_3},
    {28, 144, 1, fa18_hud_478_pixels_4},
    {29, 141, 5, fa18_hud_478_pixels_5},
    {30, 141, 2, fa18_hud_478_pixels_6},
    {30, 145, 1, fa18_hud_478_pixels_7},
    {31, 141, 2, fa18_hud_478_pixels_8},
    {31, 145, 1, fa18_hud_478_pixels_9},
    {32, 141, 2, fa18_hud_478_pixels_10},
    {32, 145, 1, fa18_hud_478_pixels_11},
};
/* Frame 480: 9 changed runs. */
static const uint16_t fa18_hud_480_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_480_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_480_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_480_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_480_pixels_4[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_480_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_480_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_480_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_hud_480_pixels_8[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_480_spans[] = {
    {26, 155, 4, fa18_hud_480_pixels_0},
    {27, 155, 1, fa18_hud_480_pixels_1},
    {27, 158, 1, fa18_hud_480_pixels_2},
    {28, 158, 1, fa18_hud_480_pixels_3},
    {29, 156, 4, fa18_hud_480_pixels_4},
    {30, 158, 2, fa18_hud_480_pixels_5},
    {31, 155, 1, fa18_hud_480_pixels_6},
    {31, 158, 2, fa18_hud_480_pixels_7},
    {32, 155, 5, fa18_hud_480_pixels_8},
};
/* Frame 482: 8 changed runs. */
static const uint16_t fa18_hud_482_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_482_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_482_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_482_pixels_3[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_482_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_482_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_482_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_482_pixels_7[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_482_spans[] = {
    {26, 162, 5, fa18_hud_482_pixels_0},
    {27, 162, 1, fa18_hud_482_pixels_1},
    {28, 162, 1, fa18_hud_482_pixels_2},
    {29, 162, 5, fa18_hud_482_pixels_3},
    {30, 165, 2, fa18_hud_482_pixels_4},
    {31, 162, 1, fa18_hud_482_pixels_5},
    {31, 165, 2, fa18_hud_482_pixels_6},
    {32, 162, 5, fa18_hud_482_pixels_7},
};
/* Frame 484: 12 changed runs. */
static const uint16_t fa18_hud_484_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_484_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_484_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_484_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_484_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_484_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_484_spans[] = {
    {26, 169, 5, fa18_hud_484_pixels_0},
    {27, 169, 1, fa18_hud_484_pixels_1},
    {27, 173, 1, fa18_hud_484_pixels_2},
    {28, 169, 1, fa18_hud_484_pixels_3},
    {28, 173, 1, fa18_hud_484_pixels_4},
    {29, 169, 1, fa18_hud_484_pixels_5},
    {29, 172, 2, fa18_hud_484_pixels_6},
    {30, 169, 1, fa18_hud_484_pixels_7},
    {30, 172, 2, fa18_hud_484_pixels_8},
    {31, 169, 1, fa18_hud_484_pixels_9},
    {31, 172, 2, fa18_hud_484_pixels_10},
    {32, 169, 5, fa18_hud_484_pixels_11},
};
/* Frame 486: 7 changed runs. */
static const uint16_t fa18_hud_486_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_486_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_486_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_486_pixels_3[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_486_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_486_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_486_pixels_6[] = {0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_486_spans[] = {
    {26, 183, 5, fa18_hud_486_pixels_0},
    {27, 183, 1, fa18_hud_486_pixels_1},
    {28, 183, 1, fa18_hud_486_pixels_2},
    {29, 183, 4, fa18_hud_486_pixels_3},
    {30, 183, 2, fa18_hud_486_pixels_4},
    {31, 183, 2, fa18_hud_486_pixels_5},
    {32, 183, 2, fa18_hud_486_pixels_6},
};
/* Frame 488: 12 changed runs. */
static const uint16_t fa18_hud_488_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_488_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_488_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_488_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_488_spans[] = {
    {26, 190, 5, fa18_hud_488_pixels_0},
    {27, 190, 1, fa18_hud_488_pixels_1},
    {27, 193, 2, fa18_hud_488_pixels_2},
    {28, 190, 1, fa18_hud_488_pixels_3},
    {28, 194, 1, fa18_hud_488_pixels_4},
    {29, 190, 1, fa18_hud_488_pixels_5},
    {29, 194, 1, fa18_hud_488_pixels_6},
    {30, 190, 1, fa18_hud_488_pixels_7},
    {30, 194, 1, fa18_hud_488_pixels_8},
    {31, 190, 1, fa18_hud_488_pixels_9},
    {31, 194, 1, fa18_hud_488_pixels_10},
    {32, 190, 5, fa18_hud_488_pixels_11},
};
/* Frame 490: 12 changed runs. */
static const uint16_t fa18_hud_490_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_490_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_490_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_490_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_490_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_490_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_490_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_490_spans[] = {
    {26, 197, 4, fa18_hud_490_pixels_0},
    {27, 197, 1, fa18_hud_490_pixels_1},
    {27, 200, 1, fa18_hud_490_pixels_2},
    {28, 197, 1, fa18_hud_490_pixels_3},
    {28, 200, 1, fa18_hud_490_pixels_4},
    {29, 197, 5, fa18_hud_490_pixels_5},
    {30, 197, 2, fa18_hud_490_pixels_6},
    {30, 201, 1, fa18_hud_490_pixels_7},
    {31, 197, 2, fa18_hud_490_pixels_8},
    {31, 201, 1, fa18_hud_490_pixels_9},
    {32, 197, 2, fa18_hud_490_pixels_10},
    {32, 201, 1, fa18_hud_490_pixels_11},
};
/* Frame 492: 11 changed runs. */
static const uint16_t fa18_hud_492_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_492_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_492_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_492_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_492_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_492_pixels_10[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_492_spans[] = {
    {26, 211, 4, fa18_hud_492_pixels_0},
    {27, 211, 1, fa18_hud_492_pixels_1},
    {27, 214, 1, fa18_hud_492_pixels_2},
    {28, 211, 1, fa18_hud_492_pixels_3},
    {28, 214, 1, fa18_hud_492_pixels_4},
    {29, 211, 5, fa18_hud_492_pixels_5},
    {30, 211, 2, fa18_hud_492_pixels_6},
    {30, 215, 1, fa18_hud_492_pixels_7},
    {31, 211, 2, fa18_hud_492_pixels_8},
    {31, 215, 1, fa18_hud_492_pixels_9},
    {32, 211, 5, fa18_hud_492_pixels_10},
};
/* Frame 494: 12 changed runs. */
static const uint16_t fa18_hud_494_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_494_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_494_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_494_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_494_spans[] = {
    {26, 218, 5, fa18_hud_494_pixels_0},
    {27, 218, 1, fa18_hud_494_pixels_1},
    {27, 221, 2, fa18_hud_494_pixels_2},
    {28, 218, 1, fa18_hud_494_pixels_3},
    {28, 222, 1, fa18_hud_494_pixels_4},
    {29, 218, 1, fa18_hud_494_pixels_5},
    {29, 222, 1, fa18_hud_494_pixels_6},
    {30, 218, 1, fa18_hud_494_pixels_7},
    {30, 222, 1, fa18_hud_494_pixels_8},
    {31, 218, 1, fa18_hud_494_pixels_9},
    {31, 222, 1, fa18_hud_494_pixels_10},
    {32, 218, 5, fa18_hud_494_pixels_11},
};
/* Frame 496: 11 changed runs. */
static const uint16_t fa18_hud_496_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_496_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_496_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_496_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_496_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_496_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_496_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_496_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_496_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_496_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_496_pixels_10[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_496_spans[] = {
    {26, 225, 5, fa18_hud_496_pixels_0},
    {27, 225, 1, fa18_hud_496_pixels_1},
    {27, 229, 1, fa18_hud_496_pixels_2},
    {28, 225, 1, fa18_hud_496_pixels_3},
    {29, 225, 2, fa18_hud_496_pixels_4},
    {29, 228, 2, fa18_hud_496_pixels_5},
    {30, 225, 2, fa18_hud_496_pixels_6},
    {30, 229, 1, fa18_hud_496_pixels_7},
    {31, 225, 2, fa18_hud_496_pixels_8},
    {31, 229, 1, fa18_hud_496_pixels_9},
    {32, 225, 5, fa18_hud_496_pixels_10},
};
/* Frame 498: 7 changed runs. */
static const uint16_t fa18_hud_498_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_498_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_498_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_498_pixels_3[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_498_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_498_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_498_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_498_spans[] = {
    {26, 232, 5, fa18_hud_498_pixels_0},
    {27, 232, 1, fa18_hud_498_pixels_1},
    {28, 232, 1, fa18_hud_498_pixels_2},
    {29, 232, 4, fa18_hud_498_pixels_3},
    {30, 232, 2, fa18_hud_498_pixels_4},
    {31, 232, 2, fa18_hud_498_pixels_5},
    {32, 232, 5, fa18_hud_498_pixels_6},
};
/* Frame 500: 10 changed runs. */
static const uint16_t fa18_hud_500_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_500_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_500_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_hud_500_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_500_pixels_9[] = {0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_500_spans[] = {
    {26, 239, 1, fa18_hud_500_pixels_0},
    {26, 243, 1, fa18_hud_500_pixels_1},
    {27, 239, 1, fa18_hud_500_pixels_2},
    {27, 243, 1, fa18_hud_500_pixels_3},
    {28, 239, 1, fa18_hud_500_pixels_4},
    {28, 243, 1, fa18_hud_500_pixels_5},
    {29, 239, 5, fa18_hud_500_pixels_6},
    {30, 241, 2, fa18_hud_500_pixels_7},
    {31, 241, 2, fa18_hud_500_pixels_8},
    {32, 241, 2, fa18_hud_500_pixels_9},
};
/* Frame 513: 12 changed runs. */
static const uint16_t fa18_hud_513_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_513_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_513_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_513_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_513_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_513_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_513_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_513_spans[] = {
    {48, 103, 4, fa18_hud_513_pixels_0},
    {49, 103, 1, fa18_hud_513_pixels_1},
    {49, 106, 1, fa18_hud_513_pixels_2},
    {50, 103, 1, fa18_hud_513_pixels_3},
    {50, 106, 1, fa18_hud_513_pixels_4},
    {51, 103, 5, fa18_hud_513_pixels_5},
    {52, 103, 2, fa18_hud_513_pixels_6},
    {52, 107, 1, fa18_hud_513_pixels_7},
    {53, 103, 2, fa18_hud_513_pixels_8},
    {53, 107, 1, fa18_hud_513_pixels_9},
    {54, 103, 2, fa18_hud_513_pixels_10},
    {54, 107, 1, fa18_hud_513_pixels_11},
};
/* Frame 515: 7 changed runs. */
static const uint16_t fa18_hud_515_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_515_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_515_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_515_pixels_3[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_515_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_515_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_515_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_515_spans[] = {
    {48, 110, 5, fa18_hud_515_pixels_0},
    {49, 110, 1, fa18_hud_515_pixels_1},
    {50, 110, 1, fa18_hud_515_pixels_2},
    {51, 110, 4, fa18_hud_515_pixels_3},
    {52, 110, 2, fa18_hud_515_pixels_4},
    {53, 110, 2, fa18_hud_515_pixels_5},
    {54, 110, 5, fa18_hud_515_pixels_6},
};
/* Frame 517: 12 changed runs. */
static const uint16_t fa18_hud_517_pixels_0[] = {0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_517_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_517_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_517_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_517_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_517_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_517_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_517_spans[] = {
    {48, 118, 3, fa18_hud_517_pixels_0},
    {49, 118, 1, fa18_hud_517_pixels_1},
    {49, 120, 1, fa18_hud_517_pixels_2},
    {50, 118, 1, fa18_hud_517_pixels_3},
    {50, 120, 1, fa18_hud_517_pixels_4},
    {51, 117, 5, fa18_hud_517_pixels_5},
    {52, 117, 2, fa18_hud_517_pixels_6},
    {52, 121, 1, fa18_hud_517_pixels_7},
    {53, 117, 2, fa18_hud_517_pixels_8},
    {53, 121, 1, fa18_hud_517_pixels_9},
    {54, 117, 2, fa18_hud_517_pixels_10},
    {54, 121, 1, fa18_hud_517_pixels_11},
};
/* Frame 519: 12 changed runs. */
static const uint16_t fa18_hud_519_pixels_0[] = {0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_519_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_519_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_hud_519_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_9[] = {0xd92,0xd92};
static const uint16_t fa18_hud_519_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_519_pixels_11[] = {0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_519_spans[] = {
    {48, 124, 4, fa18_hud_519_pixels_0},
    {49, 124, 1, fa18_hud_519_pixels_1},
    {49, 128, 1, fa18_hud_519_pixels_2},
    {50, 124, 1, fa18_hud_519_pixels_3},
    {50, 128, 1, fa18_hud_519_pixels_4},
    {51, 124, 2, fa18_hud_519_pixels_5},
    {51, 128, 1, fa18_hud_519_pixels_6},
    {52, 124, 2, fa18_hud_519_pixels_7},
    {52, 128, 1, fa18_hud_519_pixels_8},
    {53, 124, 2, fa18_hud_519_pixels_9},
    {53, 128, 1, fa18_hud_519_pixels_10},
    {54, 124, 4, fa18_hud_519_pixels_11},
};
/* Frame 521: 10 changed runs. */
static const uint16_t fa18_hud_521_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_521_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_521_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_hud_521_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_521_pixels_9[] = {0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_521_spans[] = {
    {48, 131, 1, fa18_hud_521_pixels_0},
    {48, 135, 1, fa18_hud_521_pixels_1},
    {49, 131, 1, fa18_hud_521_pixels_2},
    {49, 135, 1, fa18_hud_521_pixels_3},
    {50, 131, 1, fa18_hud_521_pixels_4},
    {50, 135, 1, fa18_hud_521_pixels_5},
    {51, 131, 5, fa18_hud_521_pixels_6},
    {52, 133, 2, fa18_hud_521_pixels_7},
    {53, 133, 2, fa18_hud_521_pixels_8},
    {54, 133, 2, fa18_hud_521_pixels_9},
};
/* Frame 523: 7 changed runs. */
static const uint16_t fa18_hud_523_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_523_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_523_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_523_pixels_3[] = {0xd92,0xd92};
static const uint16_t fa18_hud_523_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_523_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_523_pixels_6[] = {0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_523_spans[] = {
    {48, 145, 5, fa18_hud_523_pixels_0},
    {49, 147, 1, fa18_hud_523_pixels_1},
    {50, 147, 1, fa18_hud_523_pixels_2},
    {51, 147, 2, fa18_hud_523_pixels_3},
    {52, 147, 2, fa18_hud_523_pixels_4},
    {53, 147, 2, fa18_hud_523_pixels_5},
    {54, 147, 2, fa18_hud_523_pixels_6},
};
/* Frame 525: 12 changed runs. */
static const uint16_t fa18_hud_525_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_525_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_525_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_525_pixels_11[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_525_spans[] = {
    {48, 152, 5, fa18_hud_525_pixels_0},
    {49, 152, 1, fa18_hud_525_pixels_1},
    {49, 155, 2, fa18_hud_525_pixels_2},
    {50, 152, 1, fa18_hud_525_pixels_3},
    {50, 156, 1, fa18_hud_525_pixels_4},
    {51, 152, 1, fa18_hud_525_pixels_5},
    {51, 156, 1, fa18_hud_525_pixels_6},
    {52, 152, 1, fa18_hud_525_pixels_7},
    {52, 156, 1, fa18_hud_525_pixels_8},
    {53, 152, 1, fa18_hud_525_pixels_9},
    {53, 156, 1, fa18_hud_525_pixels_10},
    {54, 152, 5, fa18_hud_525_pixels_11},
};
/* Frame 527: 7 changed runs. */
static const uint16_t fa18_hud_527_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_527_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_527_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_527_pixels_3[] = {0xd92,0xd92};
static const uint16_t fa18_hud_527_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_527_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_527_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_527_spans[] = {
    {48, 166, 1, fa18_hud_527_pixels_0},
    {49, 166, 1, fa18_hud_527_pixels_1},
    {50, 166, 1, fa18_hud_527_pixels_2},
    {51, 166, 2, fa18_hud_527_pixels_3},
    {52, 166, 2, fa18_hud_527_pixels_4},
    {53, 166, 2, fa18_hud_527_pixels_5},
    {54, 166, 5, fa18_hud_527_pixels_6},
};
/* Frame 529: 12 changed runs. */
static const uint16_t fa18_hud_529_pixels_0[] = {0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_529_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_5[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_529_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_529_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_529_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_529_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_529_pixels_11[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_529_spans[] = {
    {48, 174, 3, fa18_hud_529_pixels_0},
    {49, 174, 1, fa18_hud_529_pixels_1},
    {49, 176, 1, fa18_hud_529_pixels_2},
    {50, 174, 1, fa18_hud_529_pixels_3},
    {50, 176, 1, fa18_hud_529_pixels_4},
    {51, 173, 5, fa18_hud_529_pixels_5},
    {52, 173, 2, fa18_hud_529_pixels_6},
    {52, 177, 1, fa18_hud_529_pixels_7},
    {53, 173, 2, fa18_hud_529_pixels_8},
    {53, 177, 1, fa18_hud_529_pixels_9},
    {54, 173, 2, fa18_hud_529_pixels_10},
    {54, 177, 1, fa18_hud_529_pixels_11},
};
/* Frame 531: 13 changed runs. */
static const uint16_t fa18_hud_531_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_531_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_531_pixels_9[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_10[] = {0xd92,0xd92};
static const uint16_t fa18_hud_531_pixels_11[] = {0xd92};
static const uint16_t fa18_hud_531_pixels_12[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_531_spans[] = {
    {48, 180, 1, fa18_hud_531_pixels_0},
    {48, 184, 1, fa18_hud_531_pixels_1},
    {49, 180, 1, fa18_hud_531_pixels_2},
    {49, 184, 1, fa18_hud_531_pixels_3},
    {50, 180, 1, fa18_hud_531_pixels_4},
    {50, 184, 1, fa18_hud_531_pixels_5},
    {51, 180, 2, fa18_hud_531_pixels_6},
    {51, 184, 1, fa18_hud_531_pixels_7},
    {52, 180, 2, fa18_hud_531_pixels_8},
    {52, 184, 1, fa18_hud_531_pixels_9},
    {53, 180, 2, fa18_hud_531_pixels_10},
    {53, 184, 1, fa18_hud_531_pixels_11},
    {54, 180, 5, fa18_hud_531_pixels_12},
};
/* Frame 533: 15 changed runs. */
static const uint16_t fa18_hud_533_pixels_0[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_2[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_6[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_8[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_9[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_11[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_12[] = {0xd92};
static const uint16_t fa18_hud_533_pixels_13[] = {0xd92,0xd92};
static const uint16_t fa18_hud_533_pixels_14[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_533_spans[] = {
    {48, 187, 2, fa18_hud_533_pixels_0},
    {48, 191, 1, fa18_hud_533_pixels_1},
    {49, 187, 2, fa18_hud_533_pixels_2},
    {49, 191, 1, fa18_hud_533_pixels_3},
    {50, 187, 1, fa18_hud_533_pixels_4},
    {50, 189, 1, fa18_hud_533_pixels_5},
    {50, 191, 1, fa18_hud_533_pixels_6},
    {51, 187, 1, fa18_hud_533_pixels_7},
    {51, 190, 2, fa18_hud_533_pixels_8},
    {52, 187, 2, fa18_hud_533_pixels_9},
    {52, 191, 1, fa18_hud_533_pixels_10},
    {53, 187, 2, fa18_hud_533_pixels_11},
    {53, 191, 1, fa18_hud_533_pixels_12},
    {54, 187, 2, fa18_hud_533_pixels_13},
    {54, 191, 1, fa18_hud_533_pixels_14},
};
/* Frame 535: 9 changed runs. */
static const uint16_t fa18_hud_535_pixels_0[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_535_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_535_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_535_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_535_pixels_4[] = {0xd92,0xd92};
static const uint16_t fa18_hud_535_pixels_5[] = {0xd92,0xd92};
static const uint16_t fa18_hud_535_pixels_6[] = {0xd92,0xd92};
static const uint16_t fa18_hud_535_pixels_7[] = {0xd92};
static const uint16_t fa18_hud_535_pixels_8[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const FA18HudDeltaSpan fa18_hud_535_spans[] = {
    {48, 194, 5, fa18_hud_535_pixels_0},
    {49, 194, 1, fa18_hud_535_pixels_1},
    {49, 198, 1, fa18_hud_535_pixels_2},
    {50, 194, 1, fa18_hud_535_pixels_3},
    {51, 194, 2, fa18_hud_535_pixels_4},
    {52, 194, 2, fa18_hud_535_pixels_5},
    {53, 194, 2, fa18_hud_535_pixels_6},
    {53, 198, 1, fa18_hud_535_pixels_7},
    {54, 194, 5, fa18_hud_535_pixels_8},
};
/* Frame 537: 13 changed runs. */
static const uint16_t fa18_hud_537_pixels_0[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_1[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_2[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_3[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_4[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_5[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_6[] = {0xd92,0xd92,0xd92,0xd92,0xd92};
static const uint16_t fa18_hud_537_pixels_7[] = {0xd92,0xd92};
static const uint16_t fa18_hud_537_pixels_8[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_9[] = {0xd92,0xd92};
static const uint16_t fa18_hud_537_pixels_10[] = {0xd92};
static const uint16_t fa18_hud_537_pixels_11[] = {0xd92,0xd92};
static const uint16_t fa18_hud_537_pixels_12[] = {0xd92};
static const FA18HudDeltaSpan fa18_hud_537_spans[] = {
    {48, 201, 1, fa18_hud_537_pixels_0},
    {48, 205, 1, fa18_hud_537_pixels_1},
    {49, 201, 1, fa18_hud_537_pixels_2},
    {49, 205, 1, fa18_hud_537_pixels_3},
    {50, 201, 1, fa18_hud_537_pixels_4},
    {50, 205, 1, fa18_hud_537_pixels_5},
    {51, 201, 5, fa18_hud_537_pixels_6},
    {52, 201, 2, fa18_hud_537_pixels_7},
    {52, 205, 1, fa18_hud_537_pixels_8},
    {53, 201, 2, fa18_hud_537_pixels_9},
    {53, 205, 1, fa18_hud_537_pixels_10},
    {54, 201, 2, fa18_hud_537_pixels_11},
    {54, 205, 1, fa18_hud_537_pixels_12},
};
/* Frame 559: 359 changed runs. */
static const uint16_t fa18_hud_559_pixels_0[] = {0x447};
static const uint16_t fa18_hud_559_pixels_1[] = {0x447};
static const uint16_t fa18_hud_559_pixels_2[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_3[] = {0x447};
static const uint16_t fa18_hud_559_pixels_4[] = {0x447};
static const uint16_t fa18_hud_559_pixels_5[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_6[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_7[] = {0x447};
static const uint16_t fa18_hud_559_pixels_8[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_9[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_10[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_11[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_12[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_13[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_14[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_15[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_16[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_17[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_18[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_19[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_20[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_21[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_22[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_23[] = {0x447};
static const uint16_t fa18_hud_559_pixels_24[] = {0x447};
static const uint16_t fa18_hud_559_pixels_25[] = {0x447};
static const uint16_t fa18_hud_559_pixels_26[] = {0x447};
static const uint16_t fa18_hud_559_pixels_27[] = {0x447};
static const uint16_t fa18_hud_559_pixels_28[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_29[] = {0x447};
static const uint16_t fa18_hud_559_pixels_30[] = {0x447};
static const uint16_t fa18_hud_559_pixels_31[] = {0x447};
static const uint16_t fa18_hud_559_pixels_32[] = {0x447};
static const uint16_t fa18_hud_559_pixels_33[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_34[] = {0x447};
static const uint16_t fa18_hud_559_pixels_35[] = {0x447};
static const uint16_t fa18_hud_559_pixels_36[] = {0x447};
static const uint16_t fa18_hud_559_pixels_37[] = {0x447};
static const uint16_t fa18_hud_559_pixels_38[] = {0x447};
static const uint16_t fa18_hud_559_pixels_39[] = {0x447};
static const uint16_t fa18_hud_559_pixels_40[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_41[] = {0x447};
static const uint16_t fa18_hud_559_pixels_42[] = {0x447};
static const uint16_t fa18_hud_559_pixels_43[] = {0x447};
static const uint16_t fa18_hud_559_pixels_44[] = {0x447};
static const uint16_t fa18_hud_559_pixels_45[] = {0x447};
static const uint16_t fa18_hud_559_pixels_46[] = {0x447};
static const uint16_t fa18_hud_559_pixels_47[] = {0x447};
static const uint16_t fa18_hud_559_pixels_48[] = {0x447};
static const uint16_t fa18_hud_559_pixels_49[] = {0x447};
static const uint16_t fa18_hud_559_pixels_50[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_51[] = {0x447};
static const uint16_t fa18_hud_559_pixels_52[] = {0x447};
static const uint16_t fa18_hud_559_pixels_53[] = {0x447};
static const uint16_t fa18_hud_559_pixels_54[] = {0x447};
static const uint16_t fa18_hud_559_pixels_55[] = {0x447};
static const uint16_t fa18_hud_559_pixels_56[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_57[] = {0x447};
static const uint16_t fa18_hud_559_pixels_58[] = {0x447};
static const uint16_t fa18_hud_559_pixels_59[] = {0x447};
static const uint16_t fa18_hud_559_pixels_60[] = {0x447};
static const uint16_t fa18_hud_559_pixels_61[] = {0x447};
static const uint16_t fa18_hud_559_pixels_62[] = {0x447};
static const uint16_t fa18_hud_559_pixels_63[] = {0x447};
static const uint16_t fa18_hud_559_pixels_64[] = {0x447};
static const uint16_t fa18_hud_559_pixels_65[] = {0x447};
static const uint16_t fa18_hud_559_pixels_66[] = {0x447};
static const uint16_t fa18_hud_559_pixels_67[] = {0x447};
static const uint16_t fa18_hud_559_pixels_68[] = {0x447};
static const uint16_t fa18_hud_559_pixels_69[] = {0x447};
static const uint16_t fa18_hud_559_pixels_70[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_71[] = {0x447};
static const uint16_t fa18_hud_559_pixels_72[] = {0x447};
static const uint16_t fa18_hud_559_pixels_73[] = {0x447};
static const uint16_t fa18_hud_559_pixels_74[] = {0x447};
static const uint16_t fa18_hud_559_pixels_75[] = {0x447};
static const uint16_t fa18_hud_559_pixels_76[] = {0x447};
static const uint16_t fa18_hud_559_pixels_77[] = {0x447};
static const uint16_t fa18_hud_559_pixels_78[] = {0x447};
static const uint16_t fa18_hud_559_pixels_79[] = {0x447};
static const uint16_t fa18_hud_559_pixels_80[] = {0x447};
static const uint16_t fa18_hud_559_pixels_81[] = {0x447};
static const uint16_t fa18_hud_559_pixels_82[] = {0x447};
static const uint16_t fa18_hud_559_pixels_83[] = {0x447};
static const uint16_t fa18_hud_559_pixels_84[] = {0x447};
static const uint16_t fa18_hud_559_pixels_85[] = {0x447};
static const uint16_t fa18_hud_559_pixels_86[] = {0x447};
static const uint16_t fa18_hud_559_pixels_87[] = {0x447};
static const uint16_t fa18_hud_559_pixels_88[] = {0x447};
static const uint16_t fa18_hud_559_pixels_89[] = {0x447};
static const uint16_t fa18_hud_559_pixels_90[] = {0x447};
static const uint16_t fa18_hud_559_pixels_91[] = {0x447};
static const uint16_t fa18_hud_559_pixels_92[] = {0x447};
static const uint16_t fa18_hud_559_pixels_93[] = {0x447};
static const uint16_t fa18_hud_559_pixels_94[] = {0x447};
static const uint16_t fa18_hud_559_pixels_95[] = {0x447};
static const uint16_t fa18_hud_559_pixels_96[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_97[] = {0x447};
static const uint16_t fa18_hud_559_pixels_98[] = {0x447};
static const uint16_t fa18_hud_559_pixels_99[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_100[] = {0x447};
static const uint16_t fa18_hud_559_pixels_101[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_102[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_103[] = {0x447};
static const uint16_t fa18_hud_559_pixels_104[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_105[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_106[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_107[] = {0x447};
static const uint16_t fa18_hud_559_pixels_108[] = {0x447};
static const uint16_t fa18_hud_559_pixels_109[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_110[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_111[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_112[] = {0x447};
static const uint16_t fa18_hud_559_pixels_113[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_114[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_115[] = {0x447};
static const uint16_t fa18_hud_559_pixels_116[] = {0x447};
static const uint16_t fa18_hud_559_pixels_117[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_118[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_119[] = {0x447};
static const uint16_t fa18_hud_559_pixels_120[] = {0x447};
static const uint16_t fa18_hud_559_pixels_121[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_122[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_123[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_124[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_125[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_126[] = {0x447};
static const uint16_t fa18_hud_559_pixels_127[] = {0x447};
static const uint16_t fa18_hud_559_pixels_128[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_129[] = {0x447};
static const uint16_t fa18_hud_559_pixels_130[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_131[] = {0x447};
static const uint16_t fa18_hud_559_pixels_132[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_133[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_134[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_135[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_136[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_137[] = {0x447};
static const uint16_t fa18_hud_559_pixels_138[] = {0x447};
static const uint16_t fa18_hud_559_pixels_139[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_140[] = {0x447};
static const uint16_t fa18_hud_559_pixels_141[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_142[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_143[] = {0x447};
static const uint16_t fa18_hud_559_pixels_144[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_145[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_146[] = {0x447};
static const uint16_t fa18_hud_559_pixels_147[] = {0x447};
static const uint16_t fa18_hud_559_pixels_148[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_149[] = {0x447};
static const uint16_t fa18_hud_559_pixels_150[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_151[] = {0x447};
static const uint16_t fa18_hud_559_pixels_152[] = {0x447};
static const uint16_t fa18_hud_559_pixels_153[] = {0x447};
static const uint16_t fa18_hud_559_pixels_154[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_155[] = {0x447};
static const uint16_t fa18_hud_559_pixels_156[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_157[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_158[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_159[] = {0x447};
static const uint16_t fa18_hud_559_pixels_160[] = {0x447};
static const uint16_t fa18_hud_559_pixels_161[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_162[] = {0x447};
static const uint16_t fa18_hud_559_pixels_163[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_164[] = {0x447};
static const uint16_t fa18_hud_559_pixels_165[] = {0x447};
static const uint16_t fa18_hud_559_pixels_166[] = {0x447};
static const uint16_t fa18_hud_559_pixels_167[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_168[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_169[] = {0x447};
static const uint16_t fa18_hud_559_pixels_170[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_171[] = {0x447};
static const uint16_t fa18_hud_559_pixels_172[] = {0x447};
static const uint16_t fa18_hud_559_pixels_173[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_174[] = {0x447};
static const uint16_t fa18_hud_559_pixels_175[] = {0x447};
static const uint16_t fa18_hud_559_pixels_176[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_177[] = {0x447};
static const uint16_t fa18_hud_559_pixels_178[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_179[] = {0x447};
static const uint16_t fa18_hud_559_pixels_180[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_181[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_182[] = {0x447};
static const uint16_t fa18_hud_559_pixels_183[] = {0x447};
static const uint16_t fa18_hud_559_pixels_184[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_185[] = {0x447};
static const uint16_t fa18_hud_559_pixels_186[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_187[] = {0x447};
static const uint16_t fa18_hud_559_pixels_188[] = {0x447};
static const uint16_t fa18_hud_559_pixels_189[] = {0x447};
static const uint16_t fa18_hud_559_pixels_190[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_191[] = {0x447};
static const uint16_t fa18_hud_559_pixels_192[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_193[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_194[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_195[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_196[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_197[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_198[] = {0x447};
static const uint16_t fa18_hud_559_pixels_199[] = {0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_200[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_201[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_202[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_203[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_204[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_205[] = {0x447};
static const uint16_t fa18_hud_559_pixels_206[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_207[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_208[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_209[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_210[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_211[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_212[] = {0x447};
static const uint16_t fa18_hud_559_pixels_213[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_214[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_215[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_216[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_217[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_218[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_219[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_220[] = {0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_221[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_222[] = {0x447};
static const uint16_t fa18_hud_559_pixels_223[] = {0x447};
static const uint16_t fa18_hud_559_pixels_224[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_225[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_226[] = {0x447};
static const uint16_t fa18_hud_559_pixels_227[] = {0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_228[] = {0x447};
static const uint16_t fa18_hud_559_pixels_229[] = {0x447};
static const uint16_t fa18_hud_559_pixels_230[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_231[] = {0x447};
static const uint16_t fa18_hud_559_pixels_232[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_233[] = {0x447};
static const uint16_t fa18_hud_559_pixels_234[] = {0x447};
static const uint16_t fa18_hud_559_pixels_235[] = {0x447};
static const uint16_t fa18_hud_559_pixels_236[] = {0x447};
static const uint16_t fa18_hud_559_pixels_237[] = {0x447};
static const uint16_t fa18_hud_559_pixels_238[] = {0x447};
static const uint16_t fa18_hud_559_pixels_239[] = {0x447};
static const uint16_t fa18_hud_559_pixels_240[] = {0x447};
static const uint16_t fa18_hud_559_pixels_241[] = {0x447};
static const uint16_t fa18_hud_559_pixels_242[] = {0x447};
static const uint16_t fa18_hud_559_pixels_243[] = {0x447};
static const uint16_t fa18_hud_559_pixels_244[] = {0x447};
static const uint16_t fa18_hud_559_pixels_245[] = {0x447};
static const uint16_t fa18_hud_559_pixels_246[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_247[] = {0x447};
static const uint16_t fa18_hud_559_pixels_248[] = {0x447};
static const uint16_t fa18_hud_559_pixels_249[] = {0x447};
static const uint16_t fa18_hud_559_pixels_250[] = {0x447};
static const uint16_t fa18_hud_559_pixels_251[] = {0x447};
static const uint16_t fa18_hud_559_pixels_252[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_253[] = {0x447};
static const uint16_t fa18_hud_559_pixels_254[] = {0x447};
static const uint16_t fa18_hud_559_pixels_255[] = {0x447};
static const uint16_t fa18_hud_559_pixels_256[] = {0x447};
static const uint16_t fa18_hud_559_pixels_257[] = {0x447};
static const uint16_t fa18_hud_559_pixels_258[] = {0x447};
static const uint16_t fa18_hud_559_pixels_259[] = {0x447};
static const uint16_t fa18_hud_559_pixels_260[] = {0x447};
static const uint16_t fa18_hud_559_pixels_261[] = {0x447};
static const uint16_t fa18_hud_559_pixels_262[] = {0x447};
static const uint16_t fa18_hud_559_pixels_263[] = {0x447};
static const uint16_t fa18_hud_559_pixels_264[] = {0x447};
static const uint16_t fa18_hud_559_pixels_265[] = {0x447};
static const uint16_t fa18_hud_559_pixels_266[] = {0x447};
static const uint16_t fa18_hud_559_pixels_267[] = {0x447};
static const uint16_t fa18_hud_559_pixels_268[] = {0x447};
static const uint16_t fa18_hud_559_pixels_269[] = {0x447};
static const uint16_t fa18_hud_559_pixels_270[] = {0x447};
static const uint16_t fa18_hud_559_pixels_271[] = {0x447};
static const uint16_t fa18_hud_559_pixels_272[] = {0x447};
static const uint16_t fa18_hud_559_pixels_273[] = {0x447};
static const uint16_t fa18_hud_559_pixels_274[] = {0x447};
static const uint16_t fa18_hud_559_pixels_275[] = {0x447};
static const uint16_t fa18_hud_559_pixels_276[] = {0x447};
static const uint16_t fa18_hud_559_pixels_277[] = {0x447};
static const uint16_t fa18_hud_559_pixels_278[] = {0x447};
static const uint16_t fa18_hud_559_pixels_279[] = {0x447};
static const uint16_t fa18_hud_559_pixels_280[] = {0x447};
static const uint16_t fa18_hud_559_pixels_281[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_282[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_283[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_284[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_285[] = {0x447};
static const uint16_t fa18_hud_559_pixels_286[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_287[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_288[] = {0x447};
static const uint16_t fa18_hud_559_pixels_289[] = {0x447};
static const uint16_t fa18_hud_559_pixels_290[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_291[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_292[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_293[] = {0x447};
static const uint16_t fa18_hud_559_pixels_294[] = {0x447};
static const uint16_t fa18_hud_559_pixels_295[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_296[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_297[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_298[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_299[] = {0x447};
static const uint16_t fa18_hud_559_pixels_300[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_301[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_302[] = {0x447};
static const uint16_t fa18_hud_559_pixels_303[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_304[] = {0x447};
static const uint16_t fa18_hud_559_pixels_305[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_306[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_307[] = {0x447};
static const uint16_t fa18_hud_559_pixels_308[] = {0x447};
static const uint16_t fa18_hud_559_pixels_309[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_310[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_311[] = {0x447};
static const uint16_t fa18_hud_559_pixels_312[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_313[] = {0x447};
static const uint16_t fa18_hud_559_pixels_314[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_315[] = {0x447};
static const uint16_t fa18_hud_559_pixels_316[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_317[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_318[] = {0x447};
static const uint16_t fa18_hud_559_pixels_319[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_320[] = {0x447};
static const uint16_t fa18_hud_559_pixels_321[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_322[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_323[] = {0x447};
static const uint16_t fa18_hud_559_pixels_324[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_325[] = {0x447};
static const uint16_t fa18_hud_559_pixels_326[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_327[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_328[] = {0x447};
static const uint16_t fa18_hud_559_pixels_329[] = {0x447};
static const uint16_t fa18_hud_559_pixels_330[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_331[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_332[] = {0x447};
static const uint16_t fa18_hud_559_pixels_333[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_334[] = {0x447};
static const uint16_t fa18_hud_559_pixels_335[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_336[] = {0x447};
static const uint16_t fa18_hud_559_pixels_337[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_338[] = {0x447};
static const uint16_t fa18_hud_559_pixels_339[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_340[] = {0x447};
static const uint16_t fa18_hud_559_pixels_341[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_342[] = {0x447};
static const uint16_t fa18_hud_559_pixels_343[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_344[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_345[] = {0x447};
static const uint16_t fa18_hud_559_pixels_346[] = {0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_347[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_348[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_349[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_350[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_351[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_352[] = {0x447};
static const uint16_t fa18_hud_559_pixels_353[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_354[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_355[] = {0x447};
static const uint16_t fa18_hud_559_pixels_356[] = {0x447,0x447,0x447,0x447,0x447};
static const uint16_t fa18_hud_559_pixels_357[] = {0x447,0x447};
static const uint16_t fa18_hud_559_pixels_358[] = {0x447};
static const FA18HudDeltaSpan fa18_hud_559_spans[] = {
    {26, 71, 1, fa18_hud_559_pixels_0},
    {26, 75, 1, fa18_hud_559_pixels_1},
    {26, 78, 5, fa18_hud_559_pixels_2},
    {26, 85, 1, fa18_hud_559_pixels_3},
    {26, 89, 1, fa18_hud_559_pixels_4},
    {26, 92, 4, fa18_hud_559_pixels_5},
    {26, 106, 2, fa18_hud_559_pixels_6},
    {26, 110, 1, fa18_hud_559_pixels_7},
    {26, 113, 5, fa18_hud_559_pixels_8},
    {26, 120, 5, fa18_hud_559_pixels_9},
    {26, 127, 5, fa18_hud_559_pixels_10},
    {26, 134, 5, fa18_hud_559_pixels_11},
    {26, 141, 4, fa18_hud_559_pixels_12},
    {26, 155, 4, fa18_hud_559_pixels_13},
    {26, 162, 5, fa18_hud_559_pixels_14},
    {26, 169, 5, fa18_hud_559_pixels_15},
    {26, 183, 5, fa18_hud_559_pixels_16},
    {26, 190, 5, fa18_hud_559_pixels_17},
    {26, 197, 4, fa18_hud_559_pixels_18},
    {26, 211, 4, fa18_hud_559_pixels_19},
    {26, 218, 5, fa18_hud_559_pixels_20},
    {26, 225, 5, fa18_hud_559_pixels_21},
    {26, 232, 5, fa18_hud_559_pixels_22},
    {26, 239, 1, fa18_hud_559_pixels_23},
    {26, 243, 1, fa18_hud_559_pixels_24},
    {27, 71, 1, fa18_hud_559_pixels_25},
    {27, 75, 1, fa18_hud_559_pixels_26},
    {27, 78, 1, fa18_hud_559_pixels_27},
    {27, 81, 2, fa18_hud_559_pixels_28},
    {27, 85, 1, fa18_hud_559_pixels_29},
    {27, 89, 1, fa18_hud_559_pixels_30},
    {27, 92, 1, fa18_hud_559_pixels_31},
    {27, 95, 1, fa18_hud_559_pixels_32},
    {27, 106, 2, fa18_hud_559_pixels_33},
    {27, 110, 1, fa18_hud_559_pixels_34},
    {27, 113, 1, fa18_hud_559_pixels_35},
    {27, 120, 1, fa18_hud_559_pixels_36},
    {27, 124, 1, fa18_hud_559_pixels_37},
    {27, 129, 1, fa18_hud_559_pixels_38},
    {27, 134, 1, fa18_hud_559_pixels_39},
    {27, 137, 2, fa18_hud_559_pixels_40},
    {27, 141, 1, fa18_hud_559_pixels_41},
    {27, 144, 1, fa18_hud_559_pixels_42},
    {27, 155, 1, fa18_hud_559_pixels_43},
    {27, 158, 1, fa18_hud_559_pixels_44},
    {27, 162, 1, fa18_hud_559_pixels_45},
    {27, 169, 1, fa18_hud_559_pixels_46},
    {27, 173, 1, fa18_hud_559_pixels_47},
    {27, 183, 1, fa18_hud_559_pixels_48},
    {27, 190, 1, fa18_hud_559_pixels_49},
    {27, 193, 2, fa18_hud_559_pixels_50},
    {27, 197, 1, fa18_hud_559_pixels_51},
    {27, 200, 1, fa18_hud_559_pixels_52},
    {27, 211, 1, fa18_hud_559_pixels_53},
    {27, 214, 1, fa18_hud_559_pixels_54},
    {27, 218, 1, fa18_hud_559_pixels_55},
    {27, 221, 2, fa18_hud_559_pixels_56},
    {27, 225, 1, fa18_hud_559_pixels_57},
    {27, 229, 1, fa18_hud_559_pixels_58},
    {27, 232, 1, fa18_hud_559_pixels_59},
    {27, 239, 1, fa18_hud_559_pixels_60},
    {27, 243, 1, fa18_hud_559_pixels_61},
    {28, 71, 1, fa18_hud_559_pixels_62},
    {28, 75, 1, fa18_hud_559_pixels_63},
    {28, 78, 1, fa18_hud_559_pixels_64},
    {28, 82, 1, fa18_hud_559_pixels_65},
    {28, 85, 1, fa18_hud_559_pixels_66},
    {28, 89, 1, fa18_hud_559_pixels_67},
    {28, 92, 1, fa18_hud_559_pixels_68},
    {28, 95, 1, fa18_hud_559_pixels_69},
    {28, 106, 2, fa18_hud_559_pixels_70},
    {28, 110, 1, fa18_hud_559_pixels_71},
    {28, 113, 1, fa18_hud_559_pixels_72},
    {28, 120, 1, fa18_hud_559_pixels_73},
    {28, 129, 1, fa18_hud_559_pixels_74},
    {28, 134, 1, fa18_hud_559_pixels_75},
    {28, 138, 1, fa18_hud_559_pixels_76},
    {28, 141, 1, fa18_hud_559_pixels_77},
    {28, 144, 1, fa18_hud_559_pixels_78},
    {28, 158, 1, fa18_hud_559_pixels_79},
    {28, 162, 1, fa18_hud_559_pixels_80},
    {28, 169, 1, fa18_hud_559_pixels_81},
    {28, 173, 1, fa18_hud_559_pixels_82},
    {28, 183, 1, fa18_hud_559_pixels_83},
    {28, 190, 1, fa18_hud_559_pixels_84},
    {28, 194, 1, fa18_hud_559_pixels_85},
    {28, 197, 1, fa18_hud_559_pixels_86},
    {28, 200, 1, fa18_hud_559_pixels_87},
    {28, 211, 1, fa18_hud_559_pixels_88},
    {28, 214, 1, fa18_hud_559_pixels_89},
    {28, 218, 1, fa18_hud_559_pixels_90},
    {28, 222, 1, fa18_hud_559_pixels_91},
    {28, 225, 1, fa18_hud_559_pixels_92},
    {28, 232, 1, fa18_hud_559_pixels_93},
    {28, 239, 1, fa18_hud_559_pixels_94},
    {28, 243, 1, fa18_hud_559_pixels_95},
    {29, 71, 5, fa18_hud_559_pixels_96},
    {29, 78, 1, fa18_hud_559_pixels_97},
    {29, 82, 1, fa18_hud_559_pixels_98},
    {29, 85, 2, fa18_hud_559_pixels_99},
    {29, 89, 1, fa18_hud_559_pixels_100},
    {29, 92, 5, fa18_hud_559_pixels_101},
    {29, 106, 2, fa18_hud_559_pixels_102},
    {29, 110, 1, fa18_hud_559_pixels_103},
    {29, 113, 4, fa18_hud_559_pixels_104},
    {29, 120, 2, fa18_hud_559_pixels_105},
    {29, 129, 2, fa18_hud_559_pixels_106},
    {29, 134, 1, fa18_hud_559_pixels_107},
    {29, 138, 1, fa18_hud_559_pixels_108},
    {29, 141, 5, fa18_hud_559_pixels_109},
    {29, 156, 4, fa18_hud_559_pixels_110},
    {29, 162, 5, fa18_hud_559_pixels_111},
    {29, 169, 1, fa18_hud_559_pixels_112},
    {29, 172, 2, fa18_hud_559_pixels_113},
    {29, 183, 4, fa18_hud_559_pixels_114},
    {29, 190, 1, fa18_hud_559_pixels_115},
    {29, 194, 1, fa18_hud_559_pixels_116},
    {29, 197, 5, fa18_hud_559_pixels_117},
    {29, 211, 5, fa18_hud_559_pixels_118},
    {29, 218, 1, fa18_hud_559_pixels_119},
    {29, 222, 1, fa18_hud_559_pixels_120},
    {29, 225, 2, fa18_hud_559_pixels_121},
    {29, 228, 2, fa18_hud_559_pixels_122},
    {29, 232, 4, fa18_hud_559_pixels_123},
    {29, 239, 5, fa18_hud_559_pixels_124},
    {30, 73, 2, fa18_hud_559_pixels_125},
    {30, 78, 1, fa18_hud_559_pixels_126},
    {30, 82, 1, fa18_hud_559_pixels_127},
    {30, 85, 2, fa18_hud_559_pixels_128},
    {30, 89, 1, fa18_hud_559_pixels_129},
    {30, 92, 2, fa18_hud_559_pixels_130},
    {30, 96, 1, fa18_hud_559_pixels_131},
    {30, 106, 2, fa18_hud_559_pixels_132},
    {30, 109, 2, fa18_hud_559_pixels_133},
    {30, 113, 2, fa18_hud_559_pixels_134},
    {30, 120, 2, fa18_hud_559_pixels_135},
    {30, 129, 2, fa18_hud_559_pixels_136},
    {30, 134, 1, fa18_hud_559_pixels_137},
    {30, 138, 1, fa18_hud_559_pixels_138},
    {30, 141, 2, fa18_hud_559_pixels_139},
    {30, 145, 1, fa18_hud_559_pixels_140},
    {30, 158, 2, fa18_hud_559_pixels_141},
    {30, 165, 2, fa18_hud_559_pixels_142},
    {30, 169, 1, fa18_hud_559_pixels_143},
    {30, 172, 2, fa18_hud_559_pixels_144},
    {30, 183, 2, fa18_hud_559_pixels_145},
    {30, 190, 1, fa18_hud_559_pixels_146},
    {30, 194, 1, fa18_hud_559_pixels_147},
    {30, 197, 2, fa18_hud_559_pixels_148},
    {30, 201, 1, fa18_hud_559_pixels_149},
    {30, 211, 2, fa18_hud_559_pixels_150},
    {30, 215, 1, fa18_hud_559_pixels_151},
    {30, 218, 1, fa18_hud_559_pixels_152},
    {30, 222, 1, fa18_hud_559_pixels_153},
    {30, 225, 2, fa18_hud_559_pixels_154},
    {30, 229, 1, fa18_hud_559_pixels_155},
    {30, 232, 2, fa18_hud_559_pixels_156},
    {30, 241, 2, fa18_hud_559_pixels_157},
    {31, 73, 2, fa18_hud_559_pixels_158},
    {31, 78, 1, fa18_hud_559_pixels_159},
    {31, 82, 1, fa18_hud_559_pixels_160},
    {31, 85, 2, fa18_hud_559_pixels_161},
    {31, 89, 1, fa18_hud_559_pixels_162},
    {31, 92, 2, fa18_hud_559_pixels_163},
    {31, 96, 1, fa18_hud_559_pixels_164},
    {31, 107, 1, fa18_hud_559_pixels_165},
    {31, 109, 1, fa18_hud_559_pixels_166},
    {31, 113, 2, fa18_hud_559_pixels_167},
    {31, 120, 2, fa18_hud_559_pixels_168},
    {31, 124, 1, fa18_hud_559_pixels_169},
    {31, 129, 2, fa18_hud_559_pixels_170},
    {31, 134, 1, fa18_hud_559_pixels_171},
    {31, 138, 1, fa18_hud_559_pixels_172},
    {31, 141, 2, fa18_hud_559_pixels_173},
    {31, 145, 1, fa18_hud_559_pixels_174},
    {31, 155, 1, fa18_hud_559_pixels_175},
    {31, 158, 2, fa18_hud_559_pixels_176},
    {31, 162, 1, fa18_hud_559_pixels_177},
    {31, 165, 2, fa18_hud_559_pixels_178},
    {31, 169, 1, fa18_hud_559_pixels_179},
    {31, 172, 2, fa18_hud_559_pixels_180},
    {31, 183, 2, fa18_hud_559_pixels_181},
    {31, 190, 1, fa18_hud_559_pixels_182},
    {31, 194, 1, fa18_hud_559_pixels_183},
    {31, 197, 2, fa18_hud_559_pixels_184},
    {31, 201, 1, fa18_hud_559_pixels_185},
    {31, 211, 2, fa18_hud_559_pixels_186},
    {31, 215, 1, fa18_hud_559_pixels_187},
    {31, 218, 1, fa18_hud_559_pixels_188},
    {31, 222, 1, fa18_hud_559_pixels_189},
    {31, 225, 2, fa18_hud_559_pixels_190},
    {31, 229, 1, fa18_hud_559_pixels_191},
    {31, 232, 2, fa18_hud_559_pixels_192},
    {31, 241, 2, fa18_hud_559_pixels_193},
    {32, 73, 2, fa18_hud_559_pixels_194},
    {32, 78, 5, fa18_hud_559_pixels_195},
    {32, 85, 5, fa18_hud_559_pixels_196},
    {32, 92, 2, fa18_hud_559_pixels_197},
    {32, 96, 1, fa18_hud_559_pixels_198},
    {32, 107, 3, fa18_hud_559_pixels_199},
    {32, 113, 5, fa18_hud_559_pixels_200},
    {32, 120, 5, fa18_hud_559_pixels_201},
    {32, 129, 2, fa18_hud_559_pixels_202},
    {32, 134, 5, fa18_hud_559_pixels_203},
    {32, 141, 2, fa18_hud_559_pixels_204},
    {32, 145, 1, fa18_hud_559_pixels_205},
    {32, 155, 5, fa18_hud_559_pixels_206},
    {32, 162, 5, fa18_hud_559_pixels_207},
    {32, 169, 5, fa18_hud_559_pixels_208},
    {32, 183, 2, fa18_hud_559_pixels_209},
    {32, 190, 5, fa18_hud_559_pixels_210},
    {32, 197, 2, fa18_hud_559_pixels_211},
    {32, 201, 1, fa18_hud_559_pixels_212},
    {32, 211, 5, fa18_hud_559_pixels_213},
    {32, 218, 5, fa18_hud_559_pixels_214},
    {32, 225, 5, fa18_hud_559_pixels_215},
    {32, 232, 5, fa18_hud_559_pixels_216},
    {32, 241, 2, fa18_hud_559_pixels_217},
    {48, 103, 4, fa18_hud_559_pixels_218},
    {48, 110, 5, fa18_hud_559_pixels_219},
    {48, 118, 3, fa18_hud_559_pixels_220},
    {48, 124, 4, fa18_hud_559_pixels_221},
    {48, 131, 1, fa18_hud_559_pixels_222},
    {48, 135, 1, fa18_hud_559_pixels_223},
    {48, 145, 5, fa18_hud_559_pixels_224},
    {48, 152, 5, fa18_hud_559_pixels_225},
    {48, 166, 1, fa18_hud_559_pixels_226},
    {48, 174, 3, fa18_hud_559_pixels_227},
    {48, 180, 1, fa18_hud_559_pixels_228},
    {48, 184, 1, fa18_hud_559_pixels_229},
    {48, 187, 2, fa18_hud_559_pixels_230},
    {48, 191, 1, fa18_hud_559_pixels_231},
    {48, 194, 5, fa18_hud_559_pixels_232},
    {48, 201, 1, fa18_hud_559_pixels_233},
    {48, 205, 1, fa18_hud_559_pixels_234},
    {49, 103, 1, fa18_hud_559_pixels_235},
    {49, 106, 1, fa18_hud_559_pixels_236},
    {49, 110, 1, fa18_hud_559_pixels_237},
    {49, 118, 1, fa18_hud_559_pixels_238},
    {49, 120, 1, fa18_hud_559_pixels_239},
    {49, 124, 1, fa18_hud_559_pixels_240},
    {49, 128, 1, fa18_hud_559_pixels_241},
    {49, 131, 1, fa18_hud_559_pixels_242},
    {49, 135, 1, fa18_hud_559_pixels_243},
    {49, 147, 1, fa18_hud_559_pixels_244},
    {49, 152, 1, fa18_hud_559_pixels_245},
    {49, 155, 2, fa18_hud_559_pixels_246},
    {49, 166, 1, fa18_hud_559_pixels_247},
    {49, 174, 1, fa18_hud_559_pixels_248},
    {49, 176, 1, fa18_hud_559_pixels_249},
    {49, 180, 1, fa18_hud_559_pixels_250},
    {49, 184, 1, fa18_hud_559_pixels_251},
    {49, 187, 2, fa18_hud_559_pixels_252},
    {49, 191, 1, fa18_hud_559_pixels_253},
    {49, 194, 1, fa18_hud_559_pixels_254},
    {49, 198, 1, fa18_hud_559_pixels_255},
    {49, 201, 1, fa18_hud_559_pixels_256},
    {49, 205, 1, fa18_hud_559_pixels_257},
    {50, 103, 1, fa18_hud_559_pixels_258},
    {50, 106, 1, fa18_hud_559_pixels_259},
    {50, 110, 1, fa18_hud_559_pixels_260},
    {50, 118, 1, fa18_hud_559_pixels_261},
    {50, 120, 1, fa18_hud_559_pixels_262},
    {50, 124, 1, fa18_hud_559_pixels_263},
    {50, 128, 1, fa18_hud_559_pixels_264},
    {50, 131, 1, fa18_hud_559_pixels_265},
    {50, 135, 1, fa18_hud_559_pixels_266},
    {50, 147, 1, fa18_hud_559_pixels_267},
    {50, 152, 1, fa18_hud_559_pixels_268},
    {50, 156, 1, fa18_hud_559_pixels_269},
    {50, 166, 1, fa18_hud_559_pixels_270},
    {50, 174, 1, fa18_hud_559_pixels_271},
    {50, 176, 1, fa18_hud_559_pixels_272},
    {50, 180, 1, fa18_hud_559_pixels_273},
    {50, 184, 1, fa18_hud_559_pixels_274},
    {50, 187, 1, fa18_hud_559_pixels_275},
    {50, 189, 1, fa18_hud_559_pixels_276},
    {50, 191, 1, fa18_hud_559_pixels_277},
    {50, 194, 1, fa18_hud_559_pixels_278},
    {50, 201, 1, fa18_hud_559_pixels_279},
    {50, 205, 1, fa18_hud_559_pixels_280},
    {51, 103, 5, fa18_hud_559_pixels_281},
    {51, 110, 4, fa18_hud_559_pixels_282},
    {51, 117, 5, fa18_hud_559_pixels_283},
    {51, 124, 2, fa18_hud_559_pixels_284},
    {51, 128, 1, fa18_hud_559_pixels_285},
    {51, 131, 5, fa18_hud_559_pixels_286},
    {51, 147, 2, fa18_hud_559_pixels_287},
    {51, 152, 1, fa18_hud_559_pixels_288},
    {51, 156, 1, fa18_hud_559_pixels_289},
    {51, 166, 2, fa18_hud_559_pixels_290},
    {51, 173, 5, fa18_hud_559_pixels_291},
    {51, 180, 2, fa18_hud_559_pixels_292},
    {51, 184, 1, fa18_hud_559_pixels_293},
    {51, 187, 1, fa18_hud_559_pixels_294},
    {51, 190, 2, fa18_hud_559_pixels_295},
    {51, 194, 2, fa18_hud_559_pixels_296},
    {51, 201, 5, fa18_hud_559_pixels_297},
    {52, 103, 2, fa18_hud_559_pixels_298},
    {52, 107, 1, fa18_hud_559_pixels_299},
    {52, 110, 2, fa18_hud_559_pixels_300},
    {52, 117, 2, fa18_hud_559_pixels_301},
    {52, 121, 1, fa18_hud_559_pixels_302},
    {52, 124, 2, fa18_hud_559_pixels_303},
    {52, 128, 1, fa18_hud_559_pixels_304},
    {52, 133, 2, fa18_hud_559_pixels_305},
    {52, 147, 2, fa18_hud_559_pixels_306},
    {52, 152, 1, fa18_hud_559_pixels_307},
    {52, 156, 1, fa18_hud_559_pixels_308},
    {52, 166, 2, fa18_hud_559_pixels_309},
    {52, 173, 2, fa18_hud_559_pixels_310},
    {52, 177, 1, fa18_hud_559_pixels_311},
    {52, 180, 2, fa18_hud_559_pixels_312},
    {52, 184, 1, fa18_hud_559_pixels_313},
    {52, 187, 2, fa18_hud_559_pixels_314},
    {52, 191, 1, fa18_hud_559_pixels_315},
    {52, 194, 2, fa18_hud_559_pixels_316},
    {52, 201, 2, fa18_hud_559_pixels_317},
    {52, 205, 1, fa18_hud_559_pixels_318},
    {53, 103, 2, fa18_hud_559_pixels_319},
    {53, 107, 1, fa18_hud_559_pixels_320},
    {53, 110, 2, fa18_hud_559_pixels_321},
    {53, 117, 2, fa18_hud_559_pixels_322},
    {53, 121, 1, fa18_hud_559_pixels_323},
    {53, 124, 2, fa18_hud_559_pixels_324},
    {53, 128, 1, fa18_hud_559_pixels_325},
    {53, 133, 2, fa18_hud_559_pixels_326},
    {53, 147, 2, fa18_hud_559_pixels_327},
    {53, 152, 1, fa18_hud_559_pixels_328},
    {53, 156, 1, fa18_hud_559_pixels_329},
    {53, 166, 2, fa18_hud_559_pixels_330},
    {53, 173, 2, fa18_hud_559_pixels_331},
    {53, 177, 1, fa18_hud_559_pixels_332},
    {53, 180, 2, fa18_hud_559_pixels_333},
    {53, 184, 1, fa18_hud_559_pixels_334},
    {53, 187, 2, fa18_hud_559_pixels_335},
    {53, 191, 1, fa18_hud_559_pixels_336},
    {53, 194, 2, fa18_hud_559_pixels_337},
    {53, 198, 1, fa18_hud_559_pixels_338},
    {53, 201, 2, fa18_hud_559_pixels_339},
    {53, 205, 1, fa18_hud_559_pixels_340},
    {54, 103, 2, fa18_hud_559_pixels_341},
    {54, 107, 1, fa18_hud_559_pixels_342},
    {54, 110, 5, fa18_hud_559_pixels_343},
    {54, 117, 2, fa18_hud_559_pixels_344},
    {54, 121, 1, fa18_hud_559_pixels_345},
    {54, 124, 4, fa18_hud_559_pixels_346},
    {54, 133, 2, fa18_hud_559_pixels_347},
    {54, 147, 2, fa18_hud_559_pixels_348},
    {54, 152, 5, fa18_hud_559_pixels_349},
    {54, 166, 5, fa18_hud_559_pixels_350},
    {54, 173, 2, fa18_hud_559_pixels_351},
    {54, 177, 1, fa18_hud_559_pixels_352},
    {54, 180, 5, fa18_hud_559_pixels_353},
    {54, 187, 2, fa18_hud_559_pixels_354},
    {54, 191, 1, fa18_hud_559_pixels_355},
    {54, 194, 5, fa18_hud_559_pixels_356},
    {54, 201, 2, fa18_hud_559_pixels_357},
    {54, 205, 1, fa18_hud_559_pixels_358},
};
/* Frame 586: 117 changed runs. */
static const uint16_t fa18_hud_586_pixels_0[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_1[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_2[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_3[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_4[] = {0x777};
static const uint16_t fa18_hud_586_pixels_5[] = {0x777};
static const uint16_t fa18_hud_586_pixels_6[] = {0x777};
static const uint16_t fa18_hud_586_pixels_7[] = {0x777};
static const uint16_t fa18_hud_586_pixels_8[] = {0x777};
static const uint16_t fa18_hud_586_pixels_9[] = {0x777};
static const uint16_t fa18_hud_586_pixels_10[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_11[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_12[] = {0x777};
static const uint16_t fa18_hud_586_pixels_13[] = {0x777};
static const uint16_t fa18_hud_586_pixels_14[] = {0x777};
static const uint16_t fa18_hud_586_pixels_15[] = {0x777};
static const uint16_t fa18_hud_586_pixels_16[] = {0x777};
static const uint16_t fa18_hud_586_pixels_17[] = {0x777};
static const uint16_t fa18_hud_586_pixels_18[] = {0x777};
static const uint16_t fa18_hud_586_pixels_19[] = {0x777};
static const uint16_t fa18_hud_586_pixels_20[] = {0x777};
static const uint16_t fa18_hud_586_pixels_21[] = {0x777};
static const uint16_t fa18_hud_586_pixels_22[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_23[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_24[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_25[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_26[] = {0x777};
static const uint16_t fa18_hud_586_pixels_27[] = {0x777};
static const uint16_t fa18_hud_586_pixels_28[] = {0x777};
static const uint16_t fa18_hud_586_pixels_29[] = {0x777};
static const uint16_t fa18_hud_586_pixels_30[] = {0x777};
static const uint16_t fa18_hud_586_pixels_31[] = {0x777};
static const uint16_t fa18_hud_586_pixels_32[] = {0x777};
static const uint16_t fa18_hud_586_pixels_33[] = {0x777};
static const uint16_t fa18_hud_586_pixels_34[] = {0x777};
static const uint16_t fa18_hud_586_pixels_35[] = {0x777};
static const uint16_t fa18_hud_586_pixels_36[] = {0x777};
static const uint16_t fa18_hud_586_pixels_37[] = {0x777};
static const uint16_t fa18_hud_586_pixels_38[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_39[] = {0x777};
static const uint16_t fa18_hud_586_pixels_40[] = {0x777};
static const uint16_t fa18_hud_586_pixels_41[] = {0x777};
static const uint16_t fa18_hud_586_pixels_42[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_43[] = {0x777};
static const uint16_t fa18_hud_586_pixels_44[] = {0x777};
static const uint16_t fa18_hud_586_pixels_45[] = {0x777};
static const uint16_t fa18_hud_586_pixels_46[] = {0x777};
static const uint16_t fa18_hud_586_pixels_47[] = {0x777};
static const uint16_t fa18_hud_586_pixels_48[] = {0x777};
static const uint16_t fa18_hud_586_pixels_49[] = {0x777};
static const uint16_t fa18_hud_586_pixels_50[] = {0x777};
static const uint16_t fa18_hud_586_pixels_51[] = {0x777};
static const uint16_t fa18_hud_586_pixels_52[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_53[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_54[] = {0x777};
static const uint16_t fa18_hud_586_pixels_55[] = {0x777};
static const uint16_t fa18_hud_586_pixels_56[] = {0x777};
static const uint16_t fa18_hud_586_pixels_57[] = {0x777};
static const uint16_t fa18_hud_586_pixels_58[] = {0x777};
static const uint16_t fa18_hud_586_pixels_59[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_60[] = {0x777};
static const uint16_t fa18_hud_586_pixels_61[] = {0x777};
static const uint16_t fa18_hud_586_pixels_62[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_63[] = {0x777};
static const uint16_t fa18_hud_586_pixels_64[] = {0x777};
static const uint16_t fa18_hud_586_pixels_65[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_66[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_67[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_68[] = {0x777};
static const uint16_t fa18_hud_586_pixels_69[] = {0x777};
static const uint16_t fa18_hud_586_pixels_70[] = {0x777};
static const uint16_t fa18_hud_586_pixels_71[] = {0x777};
static const uint16_t fa18_hud_586_pixels_72[] = {0x777};
static const uint16_t fa18_hud_586_pixels_73[] = {0x777,0x777};
static const uint16_t fa18_hud_586_pixels_74[] = {0x777};
static const uint16_t fa18_hud_586_pixels_75[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_76[] = {0x777};
static const uint16_t fa18_hud_586_pixels_77[] = {0x777};
static const uint16_t fa18_hud_586_pixels_78[] = {0x777};
static const uint16_t fa18_hud_586_pixels_79[] = {0x777};
static const uint16_t fa18_hud_586_pixels_80[] = {0x777};
static const uint16_t fa18_hud_586_pixels_81[] = {0x777};
static const uint16_t fa18_hud_586_pixels_82[] = {0x777};
static const uint16_t fa18_hud_586_pixels_83[] = {0x777};
static const uint16_t fa18_hud_586_pixels_84[] = {0x777};
static const uint16_t fa18_hud_586_pixels_85[] = {0x777};
static const uint16_t fa18_hud_586_pixels_86[] = {0x777};
static const uint16_t fa18_hud_586_pixels_87[] = {0x777};
static const uint16_t fa18_hud_586_pixels_88[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_89[] = {0x777,0x777};
static const uint16_t fa18_hud_586_pixels_90[] = {0x777};
static const uint16_t fa18_hud_586_pixels_91[] = {0x777};
static const uint16_t fa18_hud_586_pixels_92[] = {0x777};
static const uint16_t fa18_hud_586_pixels_93[] = {0x777};
static const uint16_t fa18_hud_586_pixels_94[] = {0x777};
static const uint16_t fa18_hud_586_pixels_95[] = {0x777};
static const uint16_t fa18_hud_586_pixels_96[] = {0x777};
static const uint16_t fa18_hud_586_pixels_97[] = {0x777};
static const uint16_t fa18_hud_586_pixels_98[] = {0x777};
static const uint16_t fa18_hud_586_pixels_99[] = {0x777};
static const uint16_t fa18_hud_586_pixels_100[] = {0x777};
static const uint16_t fa18_hud_586_pixels_101[] = {0x777};
static const uint16_t fa18_hud_586_pixels_102[] = {0x777};
static const uint16_t fa18_hud_586_pixels_103[] = {0x777};
static const uint16_t fa18_hud_586_pixels_104[] = {0x777};
static const uint16_t fa18_hud_586_pixels_105[] = {0x777};
static const uint16_t fa18_hud_586_pixels_106[] = {0x777,0x777,0x777};
static const uint16_t fa18_hud_586_pixels_107[] = {0x777,0x777};
static const uint16_t fa18_hud_586_pixels_108[] = {0x151};
static const uint16_t fa18_hud_586_pixels_109[] = {0x151};
static const uint16_t fa18_hud_586_pixels_110[] = {0x151};
static const uint16_t fa18_hud_586_pixels_111[] = {0x151,0x151};
static const uint16_t fa18_hud_586_pixels_112[] = {0x151};
static const uint16_t fa18_hud_586_pixels_113[] = {0x151};
static const uint16_t fa18_hud_586_pixels_114[] = {0x000};
static const uint16_t fa18_hud_586_pixels_115[] = {0x151};
static const uint16_t fa18_hud_586_pixels_116[] = {0x151,0x151};
static const FA18HudDeltaSpan fa18_hud_586_spans[] = {
    {53, 141, 3, fa18_hud_586_pixels_0},
    {53, 145, 3, fa18_hud_586_pixels_1},
    {53, 161, 3, fa18_hud_586_pixels_2},
    {53, 165, 3, fa18_hud_586_pixels_3},
    {54, 143, 1, fa18_hud_586_pixels_4},
    {54, 145, 1, fa18_hud_586_pixels_5},
    {54, 161, 1, fa18_hud_586_pixels_6},
    {54, 163, 1, fa18_hud_586_pixels_7},
    {54, 165, 1, fa18_hud_586_pixels_8},
    {54, 167, 1, fa18_hud_586_pixels_9},
    {55, 141, 3, fa18_hud_586_pixels_10},
    {55, 145, 3, fa18_hud_586_pixels_11},
    {55, 161, 1, fa18_hud_586_pixels_12},
    {55, 163, 1, fa18_hud_586_pixels_13},
    {55, 165, 1, fa18_hud_586_pixels_14},
    {55, 167, 1, fa18_hud_586_pixels_15},
    {56, 143, 1, fa18_hud_586_pixels_16},
    {56, 147, 1, fa18_hud_586_pixels_17},
    {56, 161, 1, fa18_hud_586_pixels_18},
    {56, 163, 1, fa18_hud_586_pixels_19},
    {56, 165, 1, fa18_hud_586_pixels_20},
    {56, 167, 1, fa18_hud_586_pixels_21},
    {57, 141, 3, fa18_hud_586_pixels_22},
    {57, 145, 3, fa18_hud_586_pixels_23},
    {57, 161, 3, fa18_hud_586_pixels_24},
    {57, 165, 3, fa18_hud_586_pixels_25},
    {60, 144, 1, fa18_hud_586_pixels_26},
    {60, 164, 1, fa18_hud_586_pixels_27},
    {60, 184, 1, fa18_hud_586_pixels_28},
    {61, 134, 1, fa18_hud_586_pixels_29},
    {61, 144, 1, fa18_hud_586_pixels_30},
    {61, 154, 1, fa18_hud_586_pixels_31},
    {61, 164, 1, fa18_hud_586_pixels_32},
    {61, 174, 1, fa18_hud_586_pixels_33},
    {61, 184, 1, fa18_hud_586_pixels_34},
    {63, 159, 1, fa18_hud_586_pixels_35},
    {64, 159, 1, fa18_hud_586_pixels_36},
    {65, 159, 1, fa18_hud_586_pixels_37},
    {84, 106, 3, fa18_hud_586_pixels_38},
    {84, 219, 1, fa18_hud_586_pixels_39},
    {84, 222, 1, fa18_hud_586_pixels_40},
    {84, 224, 1, fa18_hud_586_pixels_41},
    {84, 226, 3, fa18_hud_586_pixels_42},
    {85, 106, 1, fa18_hud_586_pixels_43},
    {85, 108, 1, fa18_hud_586_pixels_44},
    {85, 219, 1, fa18_hud_586_pixels_45},
    {85, 222, 1, fa18_hud_586_pixels_46},
    {85, 224, 1, fa18_hud_586_pixels_47},
    {85, 226, 1, fa18_hud_586_pixels_48},
    {86, 106, 1, fa18_hud_586_pixels_49},
    {86, 108, 1, fa18_hud_586_pixels_50},
    {86, 219, 1, fa18_hud_586_pixels_51},
    {86, 222, 3, fa18_hud_586_pixels_52},
    {86, 226, 3, fa18_hud_586_pixels_53},
    {87, 106, 1, fa18_hud_586_pixels_54},
    {87, 108, 1, fa18_hud_586_pixels_55},
    {87, 219, 1, fa18_hud_586_pixels_56},
    {87, 224, 1, fa18_hud_586_pixels_57},
    {87, 228, 1, fa18_hud_586_pixels_58},
    {88, 106, 3, fa18_hud_586_pixels_59},
    {88, 219, 1, fa18_hud_586_pixels_60},
    {88, 224, 1, fa18_hud_586_pixels_61},
    {88, 226, 3, fa18_hud_586_pixels_62},
    {91, 102, 1, fa18_hud_586_pixels_63},
    {91, 104, 1, fa18_hud_586_pixels_64},
    {91, 106, 3, fa18_hud_586_pixels_65},
    {91, 222, 3, fa18_hud_586_pixels_66},
    {91, 226, 3, fa18_hud_586_pixels_67},
    {92, 102, 1, fa18_hud_586_pixels_68},
    {92, 104, 1, fa18_hud_586_pixels_69},
    {92, 107, 1, fa18_hud_586_pixels_70},
    {92, 222, 1, fa18_hud_586_pixels_71},
    {92, 227, 1, fa18_hud_586_pixels_72},
    {93, 102, 2, fa18_hud_586_pixels_73},
    {93, 107, 1, fa18_hud_586_pixels_74},
    {93, 222, 3, fa18_hud_586_pixels_75},
    {93, 227, 1, fa18_hud_586_pixels_76},
    {94, 102, 1, fa18_hud_586_pixels_77},
    {94, 104, 1, fa18_hud_586_pixels_78},
    {94, 107, 1, fa18_hud_586_pixels_79},
    {94, 222, 1, fa18_hud_586_pixels_80},
    {94, 227, 1, fa18_hud_586_pixels_81},
    {95, 102, 1, fa18_hud_586_pixels_82},
    {95, 104, 1, fa18_hud_586_pixels_83},
    {95, 107, 1, fa18_hud_586_pixels_84},
    {95, 222, 1, fa18_hud_586_pixels_85},
    {95, 227, 1, fa18_hud_586_pixels_86},
    {121, 97, 1, fa18_hud_586_pixels_87},
    {121, 102, 3, fa18_hud_586_pixels_88},
    {121, 109, 2, fa18_hud_586_pixels_89},
    {122, 97, 1, fa18_hud_586_pixels_90},
    {122, 102, 1, fa18_hud_586_pixels_91},
    {122, 104, 1, fa18_hud_586_pixels_92},
    {122, 108, 1, fa18_hud_586_pixels_93},
    {123, 97, 1, fa18_hud_586_pixels_94},
    {123, 102, 1, fa18_hud_586_pixels_95},
    {123, 104, 1, fa18_hud_586_pixels_96},
    {123, 108, 1, fa18_hud_586_pixels_97},
    {123, 110, 1, fa18_hud_586_pixels_98},
    {124, 97, 1, fa18_hud_586_pixels_99},
    {124, 102, 1, fa18_hud_586_pixels_100},
    {124, 104, 1, fa18_hud_586_pixels_101},
    {124, 108, 1, fa18_hud_586_pixels_102},
    {124, 110, 1, fa18_hud_586_pixels_103},
    {125, 97, 1, fa18_hud_586_pixels_104},
    {125, 100, 1, fa18_hud_586_pixels_105},
    {125, 102, 3, fa18_hud_586_pixels_106},
    {125, 109, 2, fa18_hud_586_pixels_107},
    {175, 245, 1, fa18_hud_586_pixels_108},
    {176, 245, 1, fa18_hud_586_pixels_109},
    {177, 245, 1, fa18_hud_586_pixels_110},
    {177, 248, 2, fa18_hud_586_pixels_111},
    {178, 245, 1, fa18_hud_586_pixels_112},
    {178, 248, 1, fa18_hud_586_pixels_113},
    {178, 250, 1, fa18_hud_586_pixels_114},
    {179, 245, 1, fa18_hud_586_pixels_115},
    {179, 248, 2, fa18_hud_586_pixels_116},
};

static const FA18HudDelta fa18_run075_hud_deltas[] = {
    {464, 13, fa18_hud_464_spans},
    {466, 12, fa18_hud_466_spans},
    {468, 13, fa18_hud_468_spans},
    {470, 7, fa18_hud_470_spans},
    {472, 9, fa18_hud_472_spans},
    {474, 7, fa18_hud_474_spans},
    {476, 12, fa18_hud_476_spans},
    {478, 12, fa18_hud_478_spans},
    {480, 9, fa18_hud_480_spans},
    {482, 8, fa18_hud_482_spans},
    {484, 12, fa18_hud_484_spans},
    {486, 7, fa18_hud_486_spans},
    {488, 12, fa18_hud_488_spans},
    {490, 12, fa18_hud_490_spans},
    {492, 11, fa18_hud_492_spans},
    {494, 12, fa18_hud_494_spans},
    {496, 11, fa18_hud_496_spans},
    {498, 7, fa18_hud_498_spans},
    {500, 10, fa18_hud_500_spans},
    {513, 12, fa18_hud_513_spans},
    {515, 7, fa18_hud_515_spans},
    {517, 12, fa18_hud_517_spans},
    {519, 12, fa18_hud_519_spans},
    {521, 10, fa18_hud_521_spans},
    {523, 7, fa18_hud_523_spans},
    {525, 12, fa18_hud_525_spans},
    {527, 7, fa18_hud_527_spans},
    {529, 12, fa18_hud_529_spans},
    {531, 13, fa18_hud_531_spans},
    {533, 15, fa18_hud_533_spans},
    {535, 9, fa18_hud_535_spans},
    {537, 13, fa18_hud_537_spans},
    {559, 359, fa18_hud_559_spans},
    {586, 117, fa18_hud_586_spans},
};
#define FA18_RUN075_HUD_DELTA_COUNT 34

#endif
