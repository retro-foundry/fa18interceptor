/* Connected C0EFD4 cleanup/overlays; grid and scene labels have their own owners. */
#include "frame_tail.h"
#include "../player_input.h"
#include "../globals.h"
#include "../hud_stream.h"
#include "../numbers.h"
#include "../render_page.h"
#include "../text.h"
#include <stdlib.h>

static HudStreamState numeric_child(void *context,enum StreamChild child,HudStreamState values) {
    switch(child) {
    case HS_BCD_FIRST: case HS_BCD_SECOND: case HS_BCD_THIRD:
        pack_display_value();
        break;
    case HS_DIGITS_FIRST: case HS_DIGITS_SECOND: case HS_DIGITS_THIRD: {
        /* C32AC8 clears stride/row modifiers and uses all four page planes.
         * Its incoming A5 is overwritten by PAGE_PLANE_TABLE, not a selector. */
        int count=(uint16_t)values.primary+1;
        format_digits(values.registers,count,0,0);
        Text line=text_line(count,values.stream,values.pointers,values.screen,0);
        *(TextDrawResult *)context=draw_text(&line);
        break;
    }
    default: abort();
    }
    return values;
}

int native_frame_selection_cleanup(void) {
    wr_u16(UPDATE_STAGE_MARKER,0x1d4);
    return drop_lost_selection(); /* C12242; C31F4A immediately following is RTS. */
}

NativeInputReturn native_frame_debug_overlay(NativeInputReturn prior) {
    /* C0F386 runs on both the drawing and idle branches, after C2B3C2.
     * The source's idle UPDATE_ACTIVE gate suppresses these overlays. */
    const int active=rd_u8(UPDATE_TAIL_CONDITION) && rd_u8(UPDATE_ACTIVE);
    if(active) {
        wr_u16(UPDATE_STAGE_MARKER,0x210);
        draw_page_debug_mark();
        wr_u16(UPDATE_STAGE_MARKER,0x218);
        TextDrawResult text={0};
        const HudStreamHooks hooks={.consume_values=numeric_child,.context=&text};
        draw_stream_numeric_fields((HudStreamState){0},&hooks);
        if(text.kind==TEXT_DRAW_CHARACTER)
            prior=(NativeInputReturn){text.character,NATIVE_INPUT_RETURN_DEBUG_TEXT};
        else if(text.kind==TEXT_DRAW_GLYPH)
            prior=(NativeInputReturn){(uint8_t)text.glyph,NATIVE_INPUT_RETURN_DEBUG_TEXT};
        else prior=(NativeInputReturn){0};
    }
    wr_u16(UPDATE_STAGE_MARKER,0x220); /* C32CEE belongs to the frontend. */
    return prior;
}
