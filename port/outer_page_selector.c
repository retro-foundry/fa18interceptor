#include "outer_page_selector.h"

int fa18_select_outer_page_pointer_pair(uint16_t selected_index,
                                        FA18OuterPagePointerPair *pair) {
    if (!pair) return -1;
    *pair = selected_index == 0 ? FA18_OUTER_PAGE_POINTER_PAIR_0
                                : FA18_OUTER_PAGE_POINTER_PAIR_1;
    return 0;
}

uint16_t fa18_advance_outer_page_index(uint16_t selected_index) {
    return (uint16_t)(UINT16_C(1) - selected_index);
}
