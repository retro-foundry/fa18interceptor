#ifndef FA18_INPUT_DISPLAY_SETUP_H
#define FA18_INPUT_DISPLAY_SETUP_H
#include "memory.h"
enum InputDisplayChild {
    IDS_CREATE_PORT, IDS_PORT_FAILURE, IDS_CREATE_REQUEST, IDS_DELETE_FAILED_PORT,
    IDS_REQUEST_FAILURE, IDS_OPEN_GAMEPORT, IDS_DELETE_OPEN_REQUEST, IDS_DELETE_OPEN_PORT,
    IDS_OPEN_FAILURE, IDS_CONTROLLER_TYPE, IDS_DELETE_TYPE_REQUEST, IDS_DELETE_TYPE_PORT,
    IDS_TYPE_FAILURE, IDS_TRIGGER, IDS_DELETE_TRIGGER_REQUEST, IDS_DELETE_TRIGGER_PORT,
    IDS_TRIGGER_FAILURE, IDS_READ_GAMEPORT,
    IDS_SEND_CONTROLLER_TYPE, IDS_WAIT_CONTROLLER_TYPE, IDS_GET_CONTROLLER_REPLY, IDS_SEND_TRIGGER,
    IDS_LOAD_TEXT_2, IDS_LOAD_TEXT_2_ALTERNATE, IDS_DUPLICATE_TEXT_2,
    IDS_ALLOCATE_TEXT_4, IDS_CLEAR_TEXT_4, IDS_ALLOCATE_TEXT_6, IDS_CLEAR_TEXT_6,
    IDS_LOAD_TEXT_5, IDS_DUPLICATE_TEXT_6, IDS_LOAD_TEXT_11, IDS_DUPLICATE_TEXT_2_TO_12,
    IDS_LOAD_TEXT_0, IDS_DUPLICATE_TEXT_0, IDS_LOAD_TEXT_8, IDS_DUPLICATE_TEXT_8,
    IDS_WAIT_PUBLICATION, IDS_LOAD_VIEW, IDS_WAIT_ACTIVITY, IDS_WAIT_BLIT,
    IDS_LOAD_STATIC_PALETTE, IDS_WAIT_STATIC_FIRST, IDS_WAIT_STATIC_SECOND,
    IDS_LOAD_DYNAMIC_PALETTE, IDS_WAIT_DYNAMIC_FIRST, IDS_WAIT_DYNAMIC_SECOND,
    IDS_WAIT_CLEAR_PALETTE, IDS_LOAD_CLEAR_PALETTE
};
enum InputDisplayPhase {
    IDS_D0_BYTE, IDS_D0_WORD, IDS_D0_LONG, IDS_D1_LONG, IDS_A0, IDS_A1,
    IDS_STORE_BYTE, IDS_STORE_WORD, IDS_STORE_LONG, IDS_TEST_BYTE, IDS_TEST_WORD, IDS_TEST_LONG,
    IDS_EXT_WORD, IDS_EXT_LONG, IDS_SHIFT_LONG, IDS_SUB_D1_WORD, IDS_SUB_D0_BYTE,
    IDS_ADD_MEMORY_LONG, IDS_AND_D0_LONG, IDS_SUB_D0_LONG, IDS_OR_D0_LONG, IDS_BIT_TEST
};
typedef struct {
    int32_t (*consume)(void *context,enum InputDisplayChild child);
    void (*observe)(void *context,enum InputDisplayPhase phase,uint32_t value,uint32_t other);
    void *context;
} InputDisplayHooks;
void open_gameport_device(gaddr frame,const InputDisplayHooks *h);
void set_gameport_controller_type(gaddr frame,const InputDisplayHooks *h);
void configure_gameport_events(gaddr frame,const InputDisplayHooks *h);
void load_setup_text_resources(gaddr frame,const InputDisplayHooks *h);
/* C1612C continuation. A zero-initialized state starts a new publication.
 * await_child returns zero while a service is pending; consume runs exactly
 * once after it completes. NULL retains the blocking reference contract. */
enum OuterDisplayPhase {
    OUTER_WAIT_PUBLICATION, OUTER_PUBLISH, OUTER_TEST_ACTIVITY,
    OUTER_WAIT_ACTIVITY, OUTER_WAIT_BLIT, OUTER_TEST_COUNT,
    OUTER_STATIC_PALETTE, OUTER_STATIC_FIRST, OUTER_STATIC_SECOND,
    OUTER_DYNAMIC_PALETTE, OUTER_DYNAMIC_FIRST, OUTER_DYNAMIC_SECOND,
    OUTER_DECREMENT_ACTIVITY, OUTER_TEST_CLEAR, OUTER_CLEAR_WAIT,
    OUTER_CLEAR_PALETTE, OUTER_SWAP_PAGE, OUTER_COMPLETE
};
typedef struct { enum OuterDisplayPhase phase; } OuterDisplayState;
int advance_outer_display(gaddr frame,const InputDisplayHooks *h,
    OuterDisplayState *state,
    int (*await_child)(void *context,enum InputDisplayChild child));
void synchronize_outer_display(gaddr frame,const InputDisplayHooks *h);
#endif
