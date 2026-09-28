#include "display_record_projection.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    FA18DisplayRecordPair pair;

    /* Return-bounded run075 `$C2E758` trace: the two accepted `$C45AC6`
     * triplets reached `$C2EA34` as these pairs. */
    assert(fa18_project_adjusted_display_pair(-4582, -1, 4582, &pair) == 0);
    assert(pair.x == 0 && pair.y == 90);
    assert(fa18_project_adjusted_display_pair(5652, -1, 5652, &pair) == 0);
    assert(pair.x == 319 && pair.y == 90);

    /* Positive odd quotient keeps the source ASR carry; the negative form
     * truncates toward zero. */
    assert(fa18_project_adjusted_display_pair(1, 0, 160, &pair) == 0);
    assert(pair.x == 161 && pair.y == 90);
    assert(fa18_project_adjusted_display_pair(-1, 0, 160, &pair) == 0);
    assert(pair.x == 159 && pair.y == 90);
    assert(fa18_project_adjusted_display_pair(30000, -30000, 1, &pair) == -1);
    assert(fa18_project_adjusted_display_pair(1, 2, 0, &pair) == -1);
    assert(fa18_project_adjusted_display_pair(1, 2, 1, NULL) == -1);
    puts("display-record projection contract passed");
    return 0;
}
