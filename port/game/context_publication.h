#ifndef FA18_CONTEXT_PUBLICATION_H
#define FA18_CONTEXT_PUBLICATION_H
#include "view_commands.h"
#include "command_publication.h"

enum ContextPublicationChild {
    CONTEXT_PUBLISH_ZOOM, CONTEXT_PUBLISH_VIEW, CONTEXT_PUBLISH_SELECTION_TONE
};
enum ContextPublicationPhase {
    CONTEXT_PUBLISH_BYTE_TEST, CONTEXT_PUBLISH_BYTE_STORE, CONTEXT_PUBLISH_WORD_STORE,
    CONTEXT_PUBLISH_INDEX_SCALE, CONTEXT_PUBLISH_RECORD_BASE,
    CONTEXT_PUBLISH_RECORD_KIND, CONTEXT_PUBLISH_RECORD_MASK, CONTEXT_PUBLISH_RECORD_COMPARE,
    CONTEXT_PUBLISH_BYTE_ZERO, CONTEXT_PUBLISH_SELECTED_RECORD,
    CONTEXT_PUBLISH_BIT_TEST, CONTEXT_PUBLISH_BIT_SET, CONTEXT_PUBLISH_BIT_CLEAR,
    CONTEXT_PUBLISH_SELECTED_READ, CONTEXT_PUBLISH_SELECTED_COMPARE,
    CONTEXT_PUBLISH_CHOSEN_COMPARE, CONTEXT_PUBLISH_SELECTION_TONE_ID,
    CONTEXT_PUBLISH_WARNING_MASK
};
typedef struct { uint32_t event; int16_t record_offset; } ContextPublicationResult;
typedef struct {
    const ViewCommandHooks *view;
    const CommandPublicationHooks *publication;
    ContextPublicationResult (*consume)(void *,enum ContextPublicationChild);
    void (*observe)(void *,enum ContextPublicationPhase,uint32_t,uint32_t,gaddr);
    void *context;
} ContextPublicationHooks;

/* Complete C1B7A6, C1BEE8 and C1C214 owners with their shared queue exit. */
void publish_context_detail_command(uint8_t event,const ContextPublicationHooks *hooks);
void publish_context_record_command(uint32_t event,int16_t index,const ContextPublicationHooks *hooks);
void publish_context_toggle_command(uint8_t event,gaddr flag,const ContextPublicationHooks *hooks);
/* Complete selected-record helpers C083A6 and C09DD0. */
void set_selected_record_request(uint8_t level,const ContextPublicationHooks *hooks);
void clear_matching_record_selection(const ContextPublicationHooks *hooks);
#endif
