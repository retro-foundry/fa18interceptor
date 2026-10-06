#ifndef FA18_NATIVE_RASTER_H
#define FA18_NATIVE_RASTER_H
#include "../memory.h"
#include "../render_line.h"
#include "../render_polygon.h"
/* Direct operations on host-owned planes, using the source's edge setup and
 * descending bounding boxes. No hardware registers or event scheduling. */
void native_raster_line(gaddr plane,const LineSetup *line,PlaneOp op,int one_dot);
void native_raster_fill(gaddr end,uint16_t size);
void native_raster_composite(gaddr mask,gaddr dest,uint16_t size,PlaneOp op);
void native_raster_clear(gaddr end,uint16_t size);
void native_raster_copy_masked(gaddr mask,gaddr from,gaddr to,uint16_t size);
void native_raster_lane(gaddr mask,gaddr pattern,gaddr dest,uint16_t size,int16_t modulo,int set);
#endif
