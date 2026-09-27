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

    const uint32_t table_1[] = {0xc074d8u, 0xc074e8u};
    const uint32_t table_2[] = {0xc07f00u, 0xc07f10u};
    FA18OuterPagePublication publication;
    assert(fa18_prepare_outer_page_publication(0, table_1, table_2, 2,
                                               &publication) == 0);
    assert(publication.selected_index == 0 &&
           publication.selected_pointer_1 == 0xc074d8u &&
           publication.selected_pointer_2 == 0xc07f00u);
    assert(fa18_prepare_outer_page_publication(1, table_1, table_2, 2,
                                               &publication) == 0);
    assert(publication.selected_pointer_1 == 0xc074e8u &&
           publication.selected_pointer_2 == 0xc07f10u);
    assert(fa18_prepare_outer_page_publication(2, table_1, table_2, 2,
                                               &publication) == -1);
    return 0;
}
