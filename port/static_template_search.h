#ifndef FA18_STATIC_TEMPLATE_SEARCH_H
#define FA18_STATIC_TEMPLATE_SEARCH_H
#include <stddef.h>
#include <stdint.h>
/* `$C1D51E-$C1D55A`: search a big-endian sorted-word row. */
int fa18_search_static_template_row(const uint8_t *row,size_t size,int16_t needle,uint16_t *index);
#endif
