#ifndef FA18_GAME_POSTFLIGHT_VARIANTS_H
#define FA18_GAME_POSTFLIGHT_VARIANTS_H

/* Drawing heads of the two postflight dispatcher targets. Both fall through
 * to the common $C31392 tail, which is handled separately. */
void draw_postflight_tuple_pairs(void); /* $C3129A-$C3130E */
void draw_postflight_fixed_quad(void); /* $C31312-$C31392 */

#endif
