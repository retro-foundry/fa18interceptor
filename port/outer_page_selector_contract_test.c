#include "outer_page_selector.h"

#include <assert.h>

int main(void) {
    FA18OuterPagePointerPair pair = FA18_OUTER_PAGE_POINTER_PAIR_1;

    assert(fa18_select_outer_page_pointer_pair(0, &pair) == 0);
    assert(pair == FA18_OUTER_PAGE_POINTER_PAIR_0);
    assert(fa18_advance_outer_page_index(0) == 1);

    assert(fa18_select_outer_page_pointer_pair(1, &pair) == 0);
    assert(pair == FA18_OUTER_PAGE_POINTER_PAIR_1);
    assert(fa18_advance_outer_page_index(1) == 0);

    assert(fa18_select_outer_page_pointer_pair(2, &pair) == 0);
    assert(pair == FA18_OUTER_PAGE_POINTER_PAIR_1);
    assert(fa18_advance_outer_page_index(2) == UINT16_MAX);
    assert(fa18_select_outer_page_pointer_pair(0, 0) == -1);
    return 0;
}
