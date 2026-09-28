#ifndef FA18_GAME_RENDER_PAGE_H
#define FA18_GAME_RENDER_PAGE_H

/* Point the renderer at the plane tables of the page being drawn: the draw
 * page alternates with the displayed one (double buffering). */
void select_draw_page(void);

#endif
