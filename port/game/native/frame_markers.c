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
static MarkerState marker_child(void *context,enum MarkerChild child,MarkerState values) {
    const MarkerHooks hooks={.frame=grid_frame,.context=context,.consume_values=marker_child};
    switch(child) {
    case MM_GRID_X_FIRST: case MM_GRID_X_SECOND: case MM_GRID_Z_FIRST:
    case MM_GRID_Z_SECOND: case MM_RECORD_POINT: case MM_MARKER_POINT:
        return transform_marker_point(values,NULL);
    case MM_GRID_X_LINE: case MM_GRID_Z_LINE:
        values.offset=(uint32_t)draw_clipped_segment();
        break;
    case MM_GRID_X_LABEL: case MM_GRID_Z_LABEL:
        native_draw_position_number((int16_t)values.offset,(int16_t)values.screen_y,
                                    (int16_t)values.x,(uint16_t)values.y);
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
        draw_line((int16_t)values.offset,(int16_t)values.screen_y,
                  (int16_t)values.x,(int16_t)values.y);
        break;
    default: abort();
    }
    return values;
}

void native_frame_grid_and_markers(void) {
    const MarkerHooks hooks={.frame=grid_frame,.consume_values=marker_child};
    draw_view_grid_and_markers((MarkerState){0},&hooks);
}
