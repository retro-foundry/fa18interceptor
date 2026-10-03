#ifndef FA18_POSTFLIGHT_MESSAGES_H
#define FA18_POSTFLIGHT_MESSAGES_H
#include "memory.h"
enum PostflightMessageChild {
    PM_BOOT_INPUT, PM_BOOT_DISPLAY, PM_OPEN_TEXT, PM_SELECT_TEXT,
    PM_BOOT_TIMER, PM_BOOT_SCENE, PM_BOOT_VIEW, PM_SET_BOUNDS,
    PM_SET_CENTRE, PM_BOOT_MENU, PM_BOOT_RECORDS, PM_INIT_TEXT,
    PM_FORMAT_TEXT, PM_DELAY_TEXT, PM_RESET_SEQUENCE, PM_INDEXED_MESSAGE,
    PM_LOAD_MODE, PM_RECORD_OUTCOME, PM_LOAD_OUTCOME, PM_REFRESH_OUTCOME,
    PM_LOAD_ERROR_TABLE, PM_CLEAR_ERROR, PM_LOAD_INTRO_TABLE, PM_CLEAR_INTRO, PM_LOAD_RETURN_TABLE,
    PM_RESET_RETURN, PM_CLEAR_RETURN, PM_REFRESH_RETURN, PM_CLEAR_STATUS,
    PM_REFRESH_STATUS, PM_LOAD_STATUS_TABLE, PM_CLEAR_STATUS_WAIT,
    PM_LOAD_RETRY_TABLE, PM_CLEAR_RETRY, PM_RESET_RETRY, PM_RESET_COMPLETE, PM_RESET_FINISH
};
enum PostflightMessagePhase {
    PM_D0_BYTE, PM_D0_WORD, PM_D0_LONG, PM_D1_BYTE,
    PM_EXT_WORD, PM_EXT_LONG, PM_TEST_BYTE, PM_TEST_WORD, PM_TEST_LONG,
    PM_STORE_BYTE, PM_STORE_WORD, PM_STORE_LONG,
    PM_A0, PM_A1, PM_CMP_BYTE, PM_CMP_WORD, PM_CMP_LONG,
    PM_ADD_BYTE, PM_ADD_WORD, PM_SUB_WORD, PM_SUB_LONG, PM_SHIFT_LONG,
    PM_OR_D1, PM_BIT_D0, PM_ADD_MEMORY_WORD, PM_ADD_MEMORY_LONG,
    PM_DIRECT_CALLBACK
};
typedef struct {
    int32_t (*consume)(void *context,enum PostflightMessageChild child);
    void (*observe)(void *context,enum PostflightMessagePhase phase,uint32_t value,uint32_t other);
    void *context;
} PostflightMessageHooks;
void initialise_postflight_text(const PostflightMessageHooks *h);
void copy_postflight_text(gaddr frame,const PostflightMessageHooks *h);
void raise_postflight_message_event(const PostflightMessageHooks *h);
void prepare_postflight_messages(gaddr frame,const PostflightMessageHooks *h);
void record_postflight_outcome(gaddr frame,const PostflightMessageHooks *h);
void queue_postflight_text_error(const PostflightMessageHooks *h);
void wait_postflight_text_error(const PostflightMessageHooks *h);
void queue_postflight_intro(const PostflightMessageHooks *h);
void accept_postflight_return(const PostflightMessageHooks *h);
void prepare_postflight_status(gaddr frame,const PostflightMessageHooks *h);
void wait_postflight_status(const PostflightMessageHooks *h);
void prepare_postflight_retry(gaddr frame,const PostflightMessageHooks *h);
void wait_postflight_retry_message(const PostflightMessageHooks *h);
void wait_postflight_retry_input(const PostflightMessageHooks *h);
void advance_postflight_retry(const PostflightMessageHooks *h);
void finish_postflight_retry(const PostflightMessageHooks *h);
#endif
