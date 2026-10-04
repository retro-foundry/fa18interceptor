#ifndef FA18_GLUE_HUD_HISTORY_STREAM_MATH_H
#define FA18_GLUE_HUD_HISTORY_STREAM_MATH_H
#include "glue_hud_projection_parents_math.h"
/* Family-local SBCD flag convention, matching the source oracle's byte
 * decimal subtraction including its undefined N/V results and sticky Z. */
static uint8_t history_decimal_subtract_flags(uint8_t source,uint8_t destination) {
    uint32_t result=(destination&15u)-(source&15u)-XFLAG_AS_1();
    FLAG_V=~result;if(result>9) result-=6;
    result+=(destination&0xf0u)-(source&0xf0u);FLAG_X=FLAG_C=(result>0x99u)<<8;
    if(FLAG_C) result+=0xa0u;result&=255u;FLAG_V&=result;FLAG_N=NFLAG_8(result);FLAG_Z|=result;return (uint8_t)result;
}
#endif
