#include <assert.h>

#include "projected_segment_clip.h"

int main(void) {
    const FA18ViewVertex endpoints[2] = {{0, 0, 10}, {0, 0, 20}};
    FA18ViewVertex candidate;
    int outside;

    assert(fa18_clip_projected_segment(endpoints,
                                       FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6,
                                       &candidate, &outside) == 0);
    assert(candidate.x == 14 && candidate.y == 0 && candidate.depth == 14 && !outside);
    assert(fa18_clip_projected_segment(endpoints,
                                       FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4,
                                       &candidate, &outside) == 0);
    assert(candidate.x == -14 && candidate.y == 0 && candidate.depth == 14 && !outside);
    assert(fa18_clip_projected_segment(endpoints,
                                       FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128,
                                       &candidate, &outside) == 0);
    assert(candidate.x == 0 && candidate.y == 14 && candidate.depth == 14 && !outside);
    assert(fa18_clip_projected_segment(endpoints,
                                       FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156,
                                       &candidate, &outside) == 0);
    assert(candidate.x == 0 && candidate.y == -14 && candidate.depth == 14 && !outside);
    return 0;
}
