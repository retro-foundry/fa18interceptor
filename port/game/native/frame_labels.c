/* Source scene-position labels, projected into ordinary host drawing planes. */
#include "frame_labels.h"
#include "../flight_markers.h"
#include "../globals.h"
#include "../numbers.h"
#include "../projection.h"
#include "../text.h"
#include <stdlib.h>

/* C32A44: choose one of four glyph shifts for a screen X; align the glyph
 * window to an even byte and use the source's wrapped 40-byte row offset. */
TextDrawResult native_draw_position_number(int16_t x,int16_t y,int16_t number,uint16_t width) {
    wr_u32(DISPLAY_VALUE,(uint32_t)(int32_t)number);
    pack_display_value();
    gaddr layout=0xc3198cu+4*(2+((uint16_t)x&15)/4);
    int16_t column=(int16_t)((x>>3)&~1);
    int16_t row=(int16_t)((uint16_t)y*40u);
    gaddr rows=(gaddr)(int32_t)row+(gaddr)(int32_t)column;
    format_digits(TEXT_LINE+1+width,width+1,0,1);
    Text line=text_line(width+1,TEXT_LINE,layout,rows,column);
    return draw_text(&line);
}

static MarkerState label_child(void *context,enum MarkerChild child,MarkerState values) {
    switch(child) {
    case MM_SCENE_PROJECT:
        project_view_point_mode((int16_t)values.offset,(int16_t)values.screen_y,
                                (int16_t)values.x,-1,0,0); /* C2ECA8 */
        break;
    case MM_SCENE_LABEL:
        *(TextDrawResult *)context=native_draw_position_number((int16_t)values.offset,(int16_t)values.screen_y,
                             (int16_t)values.x,(uint16_t)values.y);
        break;
    default: abort();
    }
    return values;
}

static TextDrawResult number_result(void *context) { return *(TextDrawResult *)context; }
NativeInputReturn native_frame_scene_labels(NativeInputReturn prior) {
    TextDrawResult number={0};
    const MarkerHooks hooks={.consume_values=label_child,.context=&number,.scene_number_result=number_result};
    const SceneLabelResult result=draw_scene_position_labels((MarkerState){0},&hooks);
    if(result.kind==SCENE_LABEL_NO_OUTPUT) return prior;
    if(result.kind==SCENE_LABEL_ROW_Z || result.kind==SCENE_LABEL_MATRIX_Z)
        return (NativeInputReturn){(uint8_t)result.component_z,NATIVE_INPUT_RETURN_SCENE_LABEL};
    if(result.number.kind==TEXT_DRAW_CHARACTER)
        return (NativeInputReturn){result.number.character,NATIVE_INPUT_RETURN_SCENE_LABEL};
    if(result.number.kind==TEXT_DRAW_GLYPH)
        return (NativeInputReturn){(uint8_t)result.number.glyph,NATIVE_INPUT_RETURN_SCENE_LABEL};
    return (NativeInputReturn){0};
}
