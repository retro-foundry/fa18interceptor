#ifndef FA18_GAME_RENDER_SPAN_H
#define FA18_GAME_RENDER_SPAN_H

#include <stdint.h>

/* Bound a span at `*position` (made relative to SPAN_ORIGIN) against
 * `limit`, advancing `*cursor` past the words it skips. Returns -1 when the
 * span lies beyond the limit, 0 when nothing remains, otherwise the
 * remaining count (position + limit - 20). */
int16_t bound_span(int16_t *position, int16_t limit, int32_t *cursor);

#endif
