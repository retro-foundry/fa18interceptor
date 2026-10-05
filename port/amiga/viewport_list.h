#ifndef AMIGA_VIEWPORT_LIST_H
#define AMIGA_VIEWPORT_LIST_H
#include "rgb4.h"

typedef struct {
    uint16_t width,height,modes,row_bytes,raster_x,raster_y;
    int16_t view_x,view_y,viewport_x,viewport_y;
    uint8_t depth;
    /* Opaque 32-bit plane payloads in the list format. The compatibility
     * adapter supplies guest values; native owners supply actual buffer
     * identities/offsets and retain the corresponding buffers separately.
     * This builder neither dereferences nor manufactures pointers. */
    uint32_t planes[6];
} AmigaViewportListInput;

int amiga_build_viewport_base(const AmigaViewportListInput *input,AmigaRgb4CopperList *list);
int amiga_append_viewport_colors(AmigaRgb4CopperList *list,const uint8_t *colors,
                                   size_t color_bytes,size_t count);
int amiga_merge_viewport_records(const AmigaRgb4CopperList *list,
                                   AmigaRgb4HardwareList *hardware,size_t *at);
int amiga_finish_viewport_records(AmigaRgb4HardwareList *hardware,size_t at);

enum { AMIGA_NATIVE_VIEWPORT_INSTRUCTIONS=128 };
/* Stable caller-owned native list objects, with actual bounded backing.
 * Input/page/palette ownership and presentation remain with the caller. */
typedef struct {
    uint8_t records[AMIGA_NATIVE_VIEWPORT_INSTRUCTIONS*6];
    uint8_t merged[(AMIGA_NATIVE_VIEWPORT_INSTRUCTIONS+1)*4];
    AmigaRgb4CopperList display_list;
    AmigaRgb4HardwareList view_list;
} AmigaNativeViewportLists;

/* Build and merge the actual single-viewport list into native buffers and
 * attach its real RGB4 writer. Source parameter validation occurs before
 * replacing the old contents; subsequent failures retain preceding writes.
 * Keep the owner at a stable address while its descriptors are in use. */
int amiga_build_native_viewport(AmigaNativeViewportLists *owner,
                                  const AmigaViewportListInput *input,
                                  const uint8_t *colors,size_t color_bytes,size_t count);
#endif
