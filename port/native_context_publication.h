#ifndef FA18_NATIVE_CONTEXT_PUBLICATION_H
#define FA18_NATIVE_CONTEXT_PUBLICATION_H
#include "native_scene_records.h"

typedef struct {
    FA18NativeSceneRecords *records;
    FA18ContextCommandState *context;
    FA18CommandQueue *queue;
    const FA18ViewSpanOffsets *spans;
    uint16_t *selection_marker,*target_record;
} FA18NativeContextPublication;
/* Complete C1BEE8, C1BA86 and C1B906, including actual zoom/redraw and
 * queue publication. Index scaling wraps as an original word; only members
 * of the supplied record bank can resolve a viewed reference. Missing
 * owners/data fail with preceding writes retained. No child substitutes. */
int fa18_publish_native_context_record(FA18NativeContextPublication *state,
                                         uint32_t event,int16_t index,
                                         uint32_t *axis,uint32_t *published_event);
int fa18_publish_native_view_key(FA18NativeContextPublication *state,uint32_t event,
                                   uint32_t *axis,uint32_t *published_event);
int fa18_publish_native_zero_view(FA18NativeContextPublication *state,uint32_t event,
                                    uint32_t *axis,uint32_t *published_event);
#endif
