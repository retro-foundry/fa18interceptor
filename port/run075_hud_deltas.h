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
};
#define FA18_RUN075_HUD_DELTA_COUNT 19

#endif
