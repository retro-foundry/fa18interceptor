#ifndef FA18_OUTER_PAGE_SELECTOR_H
#define FA18_OUTER_PAGE_SELECTOR_H

#include <stdint.h>

typedef enum {
    FA18_OUTER_PAGE_POINTER_PAIR_0 = 0,
    FA18_OUTER_PAGE_POINTER_PAIR_1 = 1
} FA18OuterPagePointerPair;

/* `$C2F558-$C2F581`: select the published pointer-table pair. */
int fa18_select_outer_page_pointer_pair(uint16_t selected_index,
                                        FA18OuterPagePointerPair *pair);

/* `$C1617E-$C16283` tail: word-width `1 - selected_index` for the next loop. */
uint16_t fa18_advance_outer_page_index(uint16_t selected_index);

#endif
