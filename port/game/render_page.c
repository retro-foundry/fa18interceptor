/* Double-buffered display pages. */
#include "render_page.h"

#include "globals.h"
#include "memory.h"
#include "render_line.h"

/* Each page has a four-plane pointer table and a five-entry table. */
#define PLANE_TABLE_BYTES 0x10
#define PAGE_TABLE_BYTES 0x14

void select_draw_page(void) {
    gaddr planes = PAGE0_PLANE_TABLE, pointers = PAGE0_POINTER_TABLE;
    if (rd_u16(DRAW_PAGE)) {
        planes += PLANE_TABLE_BYTES;
        pointers += PAGE_TABLE_BYTES;
    }
    wr_u32(PAGE_PLANE_TABLE, planes);
    wr_u32(PAGE_POINTER_TABLE, pointers);
}

void draw_page_debug_mark(void) {
    wr_u32(LINE_STYLE,0xfffff);
    wr_u16(CURRENT_COLOUR,13);
    if(rd_u16(DRAW_PAGE)) draw_line(0,0,8,9);
}
