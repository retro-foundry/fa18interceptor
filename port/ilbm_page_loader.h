#ifndef FA18_ILBM_PAGE_LOADER_H
#define FA18_ILBM_PAGE_LOADER_H

#include <stddef.h>
#include <stdint.h>

#include "five_plane_page.h"

/* `$C0E078`, as called for `df0:pix/splsh` at `$C0E2EE`: decode the original
 * 320x200 five-plane ILBM into its separate native startup display page. This
 * boundary owns only source asset decoding; View/Copper publication remains
 * with its recovered caller. */
int fa18_load_splsh_ilbm_page(FA18FivePlanePage *page,
                              const uint8_t *data, size_t size);

#endif
