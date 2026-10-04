#ifndef AMIGA_HOST_GRAPHICS_H
#define AMIGA_HOST_GRAPHICS_H
#include "host_compat.h"
/* PAL View coordinates are absolute beam origins; viewport offsets are
 * relative. Defaults match the reference graphics.library View. */
int amiga_host_init_view(AmigaHostCompat *,uint32_t view);
/* Packed OCS View/CopList construction. The embedding machine installs the
 * resulting hardware list. Semantic compatibility; exact OS layout/timing is
 * deferred. No captured Copper program or desktop state is used. */
int amiga_host_make_viewport(AmigaHostCompat *,uint32_t view,uint32_t viewport);
int amiga_host_merge_view(AmigaHostCompat *,uint32_t view);
int amiga_host_load_rgb4(AmigaHostCompat *,uint32_t viewport,uint32_t colors,unsigned count);
int amiga_host_free_copper(AmigaHostCompat *,uint32_t allocation);
#endif
