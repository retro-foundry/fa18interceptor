#ifndef FA18_GAME_HUD_READOUTS_H
#define FA18_GAME_HUD_READOUTS_H

#include <stdint.h>

#include "memory.h"
#include "text.h"

/* The cockpit's numeric readouts for the viewed control record. Each builds
 * its characters in TEXT_LINE and draws them with the small (3x5) or the
 * 8-pixel text plotter. Most redraw only when the value changes (a cache
 * word, see display_value_to_draw) or for a few passes after it does. */

TextDrawResult draw_scale_readout(void); /* $C31A64: 2, 10 or 40 by +$63 */
void draw_zoom_readout(void);     /* $C31ACC: 10, 20 or 40 by ZOOM_SCALE */
TextDrawResult draw_speed_readout(void);    /* $C31F4C: +$6E / 12, "KTS" in a context */
TextDrawResult draw_altitude_readout(void); /* $C3201A: +$18 >> 10 * 5, "FT" in a context */
void draw_record_72_readout(void);/* $C3212A: +$72 >> 8 */
void draw_record_2b_readout(void);/* $C32178: |+$2B << 8| / 307 */
void draw_grid_z_readout(void);   /* $C321D2: the grid row from +$1C */
void draw_grid_x_readout(void);   /* $C32260: the grid column from +$14 */

/* 8-pixel readouts in colour 13. */
void draw_weapon_readout(void);           /* $C31C60: " SW n", " AM n" or "GUN nnn" */
void draw_signed_readout(int16_t value);  /* $C31D16: sign and four digits */
void draw_load_readout(void);             /* $C31D64: +$56 with a decimal point */
void draw_shoot_cue(void);                /* $C31E6C: "IN RNG" or " SHOOT" */
TextDrawResult draw_heading_readout(void); /* $C31EB6: "HDG" and three digits */

/* Small text: the weapon kind over "ARM" or " NO" by what is left ($C328A8). */
void draw_weapon_status(void);

/* The threat lights, 2x2 squares by THREAT_EVENTS ($C3112A). */
void draw_threat_lights(void);

/* Three decimal digits of DISPLAY_VALUE_BCD into TEXT_LINE, zeros kept,
 * of which the first two are drawn with `layout` at `rows` ($C33F54 gives
 * rows $858). */
void draw_three_digits(gaddr layout, gaddr rows);

#endif
