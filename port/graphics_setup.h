#ifndef FA18_NATIVE_GRAPHICS_SETUP_H
#define FA18_NATIVE_GRAPHICS_SETUP_H
#include "input_callback.h"
#include "amiga/viewport_list.h"

typedef struct { uint8_t *bytes; size_t byte_count; uint32_t payload; } FA18NativeGraphicsPlane;
typedef struct {
    uint16_t row_bytes,rows;
    uint8_t flags,depth;
    FA18NativeGraphicsPlane *planes[8];
} FA18NativeGraphicsBitmap;
typedef struct { void *active_view; } FA18NativeDisplayService;
typedef struct FA18NativeGraphicsSetup {
    FA18NativeInputDisplay *display;
    FA18NativeDisplayService *service;
    void *previous_view,*short_view;
    FA18NativeGraphicsBitmap bitmap_storage[2],*bitmaps[2],*raster_bitmap;
    FA18NativeGraphicsBitmap *drawing_bitmap[2];
    uint16_t raster_x,raster_y,width,height,modes;
    int16_t view_x,view_y,viewport_x,viewport_y;
    FA18NativeGraphicsPlane *source[9],*table_a[8],*table_b[10];
    AmigaRgb4Palette *color_map,*viewport_color_map;
    FA18NativeInputDisplayPair pairs[2];
    const uint16_t *initial_palette; /* original mutable 32-word bank */
} FA18NativeGraphicsSetup;
typedef struct {
    FA18NativeDisplayService *(*open_display)(void *context,unsigned version);
    FA18NativeGraphicsPlane *(*allocate_plane)(void *context,size_t bytes,uint32_t flags);
    AmigaRgb4Palette *(*allocate_color_map)(void *context,size_t colors);
    uint16_t *(*allocate_dynamic_palette)(void *context,size_t bytes,uint32_t flags);
    /* Return 1 when the native service call was handled, even if its library
     * operation fails. The original parent ignores MakeVPort/MrgCop results;
     * services retain their old live outputs on operation failure. Return 0
     * only for a missing/unsupported service contract. */
    int (*make_viewport)(void *context,FA18NativeGraphicsSetup *state);
    int (*merge_view)(void *context,FA18NativeGraphicsSetup *state);
    void (*terminate)(void *context,int32_t reason);
    void *context;
} FA18NativeGraphicsSetupOps;

/* Complete $C15DB4 and its actual $C2F4DE table child, using ordinary objects.
 * Bitmap/View/RastPort data initialization retains the accepted host semantics.
 * Allocations occur in source order; the first four are checked together,
 * before allocating/checking the fifth. Error calls terminate this invocation.
 * initial_palette is copied sequentially, unmasked, after dynamic allocation.
 * The dynamic buffer is allocated but NOT seeded by this original routine.
 * Return 1 on completed invocation, 0 on termination/missing native owners;
 * preceding writes/allocations remain. The caller owns cleanup/lifetimes. */
int fa18_initialize_native_graphics(FA18NativeGraphicsSetup *state,
                                       const FA18NativeGraphicsSetupOps *ops);
/* Actual separate $C160D6 entry, called by $C15D68 after menu setup. Shares
 * the lower four planes; clears live lists, builds/publishes pair 1, then
 * resets the shared draw page. No second five-plane allocation. */
int fa18_build_native_second_display(FA18NativeGraphicsSetup *state,
                                         const FA18NativeGraphicsSetupOps *ops);
#endif
