#ifndef FA18_POSTFLIGHT_FILE_CALLERS_H
#define FA18_POSTFLIGHT_FILE_CALLERS_H
#include "memory.h"
enum PostflightFileChild {
    PFF_ALLOCATE_CHECK, PFF_LOCK_CHECK, PFF_EXAMINE_CHECK, PFF_UNLOCK_CHECK, PFF_FREE_CHECK,
    PFF_RELEASE_TABLE, PFF_CHECK_TABLE, PFF_LOAD_TABLE, PFF_SAVE_TABLE, PFF_OWN_TABLE,
    PFF_OPEN_SAVE, PFF_WRITE_SAVE, PFF_CLOSE_SAVE, PFF_OPEN_LOAD, PFF_READ_LOAD, PFF_CLOSE_LOAD
};
enum PostflightFilePhase {
    PFF_D0_BYTE, PFF_D0_LONG, PFF_EXT_WORD, PFF_EXT_LONG, PFF_A0,
    PFF_STORE_BYTE, PFF_STORE_WORD, PFF_STORE_LONG, PFF_TEST_LONG, PFF_TEST_WORD,
    PFF_CMP_BYTE, PFF_CMP_LONG, PFF_ADD_LONG, PFF_SUB_WORD, PFF_AND_LONG, PFF_LSR_LONG,
    PFF_ADD_MEMORY_BYTE, PFF_SUB_MEMORY_BYTE, PFF_ADD_MEMORY_LONG, PFF_SUB_MEMORY_LONG,
    PFF_PUSH_LOAD_HANDLE
};
typedef struct {
    int32_t (*consume)(void *context,enum PostflightFileChild child);
    void (*observe)(void *context,enum PostflightFilePhase phase,uint32_t value,uint32_t other);
    void *context;
} PostflightFileHooks;
void format_postflight_hex_frame(gaddr frame,const PostflightFileHooks *h);
uint32_t check_postflight_mode_file(gaddr frame,const PostflightFileHooks *h);
void refresh_postflight_mode_file(const PostflightFileHooks *h);
uint32_t save_postflight_mode_file(gaddr frame,const PostflightFileHooks *h);
uint32_t read_postflight_mode_file(gaddr frame,const PostflightFileHooks *h);
#endif
