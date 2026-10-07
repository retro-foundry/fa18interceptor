#ifndef FA18_VIEW_COMMANDS_H
#define FA18_VIEW_COMMANDS_H
#include "command_selection.h"
/* Shared view/origin/zoom actions of C1AC28/C1AD74, before queue publication. */
enum ViewCommandChild { VIEW_COMMAND_ZOOM_MAXIMUM, VIEW_COMMAND_REDRAW };
enum ViewCommandPhase {
    VIEW_BYTE_TEST, VIEW_BYTE_STORE, VIEW_WORD_STORE, VIEW_LONG_STORE,
    VIEW_REQUEST_BIT, VIEW_ORIGIN_TEST, VIEW_ORIGIN_READ,
    VIEW_DETAIL_SET, VIEW_ORIGIN_DECREMENT, VIEW_ORIGIN_INCREMENT,
    VIEW_ORIGIN_COMPARE, VIEW_ORIGIN_SET,
    VIEW_MIDDLE_READ, VIEW_MIDDLE_DECREASE, VIEW_MIDDLE_INCREASE,
    VIEW_MIDDLE_COMPARE, VIEW_MIDDLE_SET,
    VIEW_MODE_ZERO, VIEW_MODE_SET, VIEW_MODE_DECREMENT, VIEW_MODE_INCREMENT,
    VIEW_BYTE_COMPARE, VIEW_RECORD_ADDRESS, VIEW_RECORD_TYPE_READ,
    VIEW_RECORD_TYPE_MASK, VIEW_MODE_READ, VIEW_ROW_SET, VIEW_ROW_COMPARE,
    VIEW_SPAN_TABLE, VIEW_SPAN_INDEX, VIEW_SPAN_READ, VIEW_SPAN_SCALE,
    VIEW_ZOOM_OUT_BEGIN, VIEW_ZOOM_OUT_COMPARE, VIEW_ZOOM_IN_COMPARE,
    VIEW_ZOOM_DECREASE, VIEW_ZOOM_INCREASE, VIEW_ZOOM_FLAGS_READ,
    VIEW_ZOOM_FLAGS_MASK, VIEW_ZOOM_FLAGS_CLEAR, VIEW_ZOOM_FLAGS_SET
};
typedef struct {
    uint32_t (*consume)(void *context,enum ViewCommandChild child);
    void (*observe)(void *context,enum ViewCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address);
    void *context;
} ViewCommandHooks;
int is_view_command(enum CommandAction action);
/* C1B7CC detail tail and C1B906 zero-mode tail, before queue publication. */
void set_context_view_detail(unsigned value,const ViewCommandHooks *hooks);
uint32_t select_zero_view_mode(uint32_t event,const ViewCommandHooks *hooks);
/* C1BA86 redraw/row tail, before the shared command queue exit. */
uint32_t finish_view_redraw(uint32_t event,const ViewCommandHooks *hooks);
uint32_t execute_view_command(const CommandRequest *request,const ViewCommandHooks *hooks);
/* View-domain outputs before shared queue publication; middle-coordinate and
 * zoom-scale arithmetic do not replace these outputs in the original. */
enum ViewActionOutputKind { VIEW_ACTION_UNRESOLVED, VIEW_ACTION_PRESERVE,
    VIEW_ACTION_DETAIL, VIEW_ACTION_ORIGIN_LEVEL, VIEW_ACTION_RECORD_TYPE,
    VIEW_ACTION_MODE, VIEW_ACTION_ZOOM_FLAGS };
typedef struct {
    enum ViewActionOutputKind kind;
    uint8_t detail,origin_level,record_type,mode,zoom_flags;
} ViewActionOutput;
typedef struct { uint32_t event; ViewActionOutput output; } ViewCommandExecution;
ViewActionOutput set_context_view_detail_result(unsigned value,const ViewCommandHooks *hooks);
ViewCommandExecution select_zero_view_mode_result(uint32_t event,const ViewCommandHooks *hooks);
ViewCommandExecution finish_view_redraw_result(uint32_t event,const ViewCommandHooks *hooks);
ViewCommandExecution execute_view_command_result(const CommandRequest *request,const ViewCommandHooks *hooks);
#endif
