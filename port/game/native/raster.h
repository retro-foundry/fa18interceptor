#ifndef FA18_NATIVE_RASTER_H
#define FA18_NATIVE_RASTER_H
#include "../memory.h"
#include "../render_line.h"
#include "../render_polygon.h"
/* Direct operations on host-owned planes, using the source's edge setup and
 * bounding boxes and panel row strides. No registers or event scheduling. */
void native_raster_line(gaddr plane,const LineSetup *line,PlaneOp op,int one_dot);
void native_raster_fill(gaddr end,uint16_t size);
void native_raster_composite(gaddr mask,gaddr dest,uint16_t size,PlaneOp op);
void native_raster_clear(gaddr end,uint16_t size);
void native_raster_copy_masked(gaddr mask,gaddr from,gaddr to,uint16_t size);
void native_raster_lane(gaddr mask,gaddr pattern,gaddr dest,uint16_t size,int16_t modulo,int set);
void native_raster_panel_copy(gaddr image,gaddr dest,uint16_t size,int16_t modulo);
void native_raster_panel_image(gaddr mask,gaddr image,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo);
void native_raster_bar(gaddr dest,uint16_t size,int16_t modulo,uint16_t first,uint16_t last,int set);
void native_raster_panel_inverted(gaddr mask,gaddr pattern,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo);
void native_raster_compass(gaddr image,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo,uint16_t first,uint16_t last,unsigned shift);
void native_raster_mark_clear(gaddr image,gaddr dest,uint16_t size,int16_t dest_modulo);
#endif
