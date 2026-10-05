#ifndef FA18_NATIVE_GRAPHICS_STORAGE_H
#define FA18_NATIVE_GRAPHICS_STORAGE_H
#include "graphics_setup.h"
#include "renderer_page_setup.h"

/* Actual bounded storage for one original five-plane family and TWO list
 * owners. The second bitmap shares the first family's lower four planes.
 * No guest addresses, CPU or chipset timeline. Keep this owner stable. */
typedef struct {
    FA18RendererPageSetup renderer;
    FA18NativeGraphicsPlane planes[5];
    size_t allocated_planes;
    uint8_t colors[64];
    AmigaRgb4Palette color_map;
    uint16_t dynamic_palette[32];
    AmigaNativeViewportLists lists[2];
    FA18NativeDisplayService service;
    int map_allocated,dynamic_allocated,construction_failed;
    int32_t termination_reason;
} FA18NativeGraphicsStorage;
/* Attach the real native backend to a fresh, caller-owned zeroed storage.
 * service.active_view belongs to the native presentation owner. There is no
 * default previous View. No storage or game state is reset by this binding. */
int fa18_bind_native_graphics_storage(FA18NativeGraphicsStorage *storage,
                                         FA18NativeGraphicsSetupOps *ops);
/* Bind the already allocated family's actual buffers/offsets to the existing
 * native renderer. No page is cleared and no independent page is allocated. */
int fa18_bind_graphics_renderer(FA18NativeGraphicsStorage *storage,
                                   const FA18NativeGraphicsSetup *setup);
#endif
