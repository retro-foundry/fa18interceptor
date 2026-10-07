/* Complete source grid/aircraft-marker children, with native drawing. */
#include "frame_markers.h"
#include "frame_labels.h"
#include "../flight_markers.h"
#include "../globals.h"
#include "../projection.h"
#include "../render_line.h"
#include <stdlib.h>

/* Ordinary host scratch for the source's five grid-loop words. Source stack
 * saves are C locals in flight_markers.c; no CPU register/stack is retained. */
enum { GRID_FRAME=0x4b10 };
static gaddr grid_frame(void *context) { (void)context;return GRID_FRAME; }
static void publish_output(void *context,MarkerDrawOutput output) { *(MarkerDrawOutput *)context=output; }
static MarkerState marker_child(void *context,enum MarkerChild child,MarkerState values) {
    MarkerDrawOutput *output=context;
    const MarkerHooks hooks={.frame=grid_frame,.context=context,.consume_values=marker_child,.publish_marker_output=publish_output};
    switch(child) {
    case MM_GRID_X_FIRST: case MM_GRID_X_SECOND: case MM_GRID_Z_FIRST:
    case MM_GRID_Z_SECOND: case MM_RECORD_POINT: case MM_MARKER_POINT:
        values=transform_marker_point(values,NULL);
        *output=(MarkerDrawOutput){.kind=MARKER_DRAW_DEPTH,.depth=(int32_t)values.z};
        return values;
    case MM_GRID_X_LINE: case MM_GRID_Z_LINE:
        *output=(MarkerDrawOutput){.kind=MARKER_DRAW_SEGMENT,.segment=draw_clipped_segment_result()};
        values.offset=(uint32_t)output->segment.drawn;
        break;
    case MM_GRID_X_LABEL: case MM_GRID_Z_LABEL:
        *output=(MarkerDrawOutput){.kind=MARKER_DRAW_NUMBER,
            .number=native_draw_position_number((int16_t)values.offset,(int16_t)values.screen_y,
                                    (int16_t)values.x,(uint16_t)values.y)};
        break;
    case MM_CLASS20_MARKER: return draw_class_twenty_marker(values,&hooks);
    case MM_RECORD_MARKER: return draw_record_position_marker(values,GRID_FRAME,&hooks);
    case MM_CLASS20_PROJECT: case MM_MARKER_PROJECT: {
        int visible=project_view_point_mode((int16_t)values.offset,(int16_t)values.screen_y,
                                           (int16_t)values.x,-5,0,0); /* C2EC90 */
        values.child_negative=!visible;
        if(visible) {
            values.offset=(values.offset&0xffff0000u)|rd_u16(PROJECTED_PAIR);
            values.screen_y=(values.screen_y&0xffff0000u)|rd_u16(PROJECTED_PAIR+2);
        }
        break;
    }
    case MM_MARKER_LINE:
    {
        const LineDrawResult line=draw_line_to_row_result((int16_t)values.offset,(int16_t)values.screen_y,
                  (int16_t)values.x,(int16_t)values.y,rd_s16(LINE_LAST_ROW));
        if(line.kind!=LINE_DRAW_NONE) *output=(MarkerDrawOutput){.kind=MARKER_DRAW_LINE,.line=line};
        break;
    }
    default: abort();
    }
    return values;
}

static NativeInputReturn line_return(LineDrawResult line) {
    if(line.kind==LINE_DRAW_SIZE) return (NativeInputReturn){(uint8_t)line.size,NATIVE_INPUT_RETURN_GRID_MARKER};
    if(line.kind==LINE_DRAW_X_DELTA) return (NativeInputReturn){(uint8_t)line.x_delta,NATIVE_INPUT_RETURN_GRID_MARKER};
    return (NativeInputReturn){0};
}
NativeInputReturn native_frame_grid_and_markers(NativeInputReturn prior) {
    MarkerDrawOutput output={0};
    const MarkerHooks hooks={.frame=grid_frame,.context=&output,.consume_values=marker_child,.publish_marker_output=publish_output};
    if(!draw_view_grid_and_markers((MarkerState){0},&hooks)) return prior;
    uint8_t value;
    switch(output.kind) {
    case MARKER_DRAW_DEPTH: value=(uint8_t)output.depth;break;
    case MARKER_DRAW_HEADING: value=(uint8_t)output.heading;break;
    case MARKER_DRAW_SHAPE_OFFSET: value=(uint8_t)output.shape_offset;break;
    case MARKER_DRAW_SEGMENT:
        if(output.segment.kind==SEGMENT_DRAW_LINE) return line_return(output.segment.line);
        value=(uint8_t)output.segment.y;break;
    case MARKER_DRAW_NUMBER:
        if(output.number.kind==TEXT_DRAW_CHARACTER) value=output.number.character;
        else if(output.number.kind==TEXT_DRAW_GLYPH) value=(uint8_t)output.number.glyph;
        else return (NativeInputReturn){0};
        break;
    case MARKER_DRAW_LINE: return line_return(output.line);
    default: return (NativeInputReturn){0};
    }
    return (NativeInputReturn){value,NATIVE_INPUT_RETURN_GRID_MARKER};
}
