/* Source stores symbols in the native aircraft HUD. */
#include "stores.h"
#include "../globals.h"
#include "../hud_stores.h"

void native_hud_draw_stores(void) {
    gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
    int16_t centre=(int16_t)(0x44+rd_s16(SPAN_ORIGIN_Y));
    /* Source view offsets are multiples of 16, so the centre pixel's mask
     * supplies nonzero count bits ($0800). C30A00's explicit selection bit
     * then determines the last visible store's colour independently of the
     * preceding glyph pointer's upper bits.
     * Source panning uses 0, +/-7, +/-14, +/-21 and +/-50 words. When the
     * centre is offscreen every store is clipped too. Aircraft stocks start
     * at $24/$44 and only decrement: each stream's terminator leaves colour
     * zero, independently of inherited glyph/column values in that case. */
    uint32_t count=rd_u16(PIXEL_MASKS+2*((uint16_t)centre&15));
    draw_stores_icons(count,(uint8_t)(rd_u8(record+0x63)&0xf0));
}
