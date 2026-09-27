#ifndef FA18_MENU_RENDER_H
#define FA18_MENU_RENDER_H

#include <stddef.h>

#include "hunk.h"
#include "menu_record.h"
#include "video.h"

/* Bounded `$C32F54-$C33168` static glyph route used by the initial top-level
 * menu. It reads the original palette, position table, and glyph streams. */
int fa18_render_top_level_menu(FA18Video *video, const FA18Hunks *exe,
                               const FA18MenuRecord *records, size_t count);

#endif
