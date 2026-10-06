/* C08F26-C090C0, C0F920-C0F944, C0F992-C0FA02. */
#include "scene_bootstrap.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const SceneBootstrapHooks *hooks,enum SceneBootstrapPhase phase,uint32_t value) {
    SceneBootstrapEvent event={phase,value};
    if(hooks->observe) hooks->observe(hooks->context,&event);
}
static void require_hooks(const SceneBootstrapHooks *hooks) {
    if(!hooks || !hooks->consume) abort();
}
void prepare_scene_storage(const SceneBootstrapHooks *hooks) {
    unsigned slot,i;
    require_hooks(hooks);
    hooks->consume(hooks->context,BOOTSTRAP_CLEAR_STARTUP);
    hooks->consume(hooks->context,BOOTSTRAP_ENABLE_RECORDS);
    wr_u8(SCENE_DISPATCH_LIMIT,rd_u8(SCENE_DISPATCH_LIMIT_PREVIOUS));
    wr_u16(0xc4fda2u,0); wr_u16(0xc4fda0u,0); wr_u32(MESSAGE_QUEUE,0);
    wr_u8(UPDATE_MASK,0xff); wr_u8(MESSAGE_STATE_C,0xff); wr_u8(CONTEXT_STATE,0xff);
    wr_u16(POST_INPUT_COUNTDOWN,5); wr_u32(0xc4573eu,0x1b8);
    observe(hooks,BOOTSTRAP_BUFFERS,0);
    hooks->consume(hooks->context,BOOTSTRAP_CLEAR_BUFFERS);
    /* Source clears 41 longs per 512-byte control slot, then every long
     * of sixteen 32-byte workspace records. Preserve the control padding. */
    for(slot=0;slot<16;++slot)
        for(i=0;i<41;++i) wr_u32(CONTROL_RECORDS+slot*512u+i*4u,0);
    for(slot=0;slot<16;++slot)
        for(i=0;i<8;++i) wr_u32(WORKSPACE_RECORDS+slot*32u+i*4u,0);
    observe(hooks,BOOTSTRAP_RECORDS_CLEARED,0);
    hooks->consume(hooks->context,BOOTSTRAP_PREPARE_PLAYER);
    wr_u16(0xc459a6u,0x140); wr_u16(0xc459a8u,0x140);
    wr_u8(SCENE_POSE_ENTRY,0); wr_u8(0xc45857u,1); wr_u16(HISTORY_RECORD,0x800);
    observe(hooks,BOOTSTRAP_POSITION,0);
    hooks->consume(hooks->context,BOOTSTRAP_START_POSITION);
    hooks->consume(hooks->context,BOOTSTRAP_SET_OBSERVER);
    wr_u32(0xc45664u,0x03000000u);
    wr_u32(0xc45c4au,0); wr_u32(0xc45c4eu,0); wr_u32(0xc45c52u,0);
    wr_u16(0xc45a94u,0x1c20); wr_u16(0xc45a96u,0);
    wr_u16(0xc45984u,0xa7); wr_u16(0xc45986u,0x32); wr_u16(0xc45988u,0x320);
    wr_u32(READOUT_SOURCE_VALID,0xffffffffu); wr_u32(0xc45b02u,0xffffffffu);
    wr_u8(0xc45855u,0xff); wr_u8(0xc458beu,15); wr_u16(0xc45ae8u,0x7fff);
    wr_u32(0xc456fau,0x00800000u);
    wr_u16(0xc45a3eu,0xa8); wr_u16(0xc45a40u,0xfc); wr_u16(0xc45a42u,0x80);
    wr_u8(0xc457ddu,0x80);
    for(i=0;i<22;++i) wr_u16(0xc4e7d0u+i*2u,(uint16_t)i);
    for(i=0;i<48;++i) wr_u8(TEXT_LINE+i,0x30);
    for(i=0;i<28;++i) wr_u8(0xc4580au+i,0x20);
    observe(hooks,BOOTSTRAP_INITIALIZED,0);
    hooks->consume(hooks->context,BOOTSTRAP_PLACE_VIEW);
    hooks->consume(hooks->context,BOOTSTRAP_BUILD_GATES);
}
void bootstrap_scene(const SceneBootstrapHooks *hooks) {
    prepare_scene_storage(hooks);
    hooks->consume(hooks->context,BOOTSTRAP_UPDATE_RECORDS);
    hooks->consume(hooks->context,BOOTSTRAP_REFRESH_CONTEXT);
}
void reset_sequence_after_bootstrap(const SceneBootstrapHooks *hooks) {
    require_hooks(hooks);
    hooks->consume(hooks->context,BOOTSTRAP_RUN);
    wr_u8(CONTEXT_REQUEST,0); wr_u8(MODE_SELECT,0); wr_u8(RECORDER_MODE,0);
    wr_u32(STAGE_CALLBACK,0xc0fbe0u);
    observe(hooks,BOOTSTRAP_RESET_CALLBACK,0xc0fbe0u);
}
void begin_sequence_after_bootstrap(const SceneBootstrapHooks *hooks) {
    uint8_t mode;
    gaddr callback;
    require_hooks(hooks);
    hooks->consume(hooks->context,BOOTSTRAP_FREE_VOICES);
    hooks->consume(hooks->context,BOOTSTRAP_RUN);
    if(rd_u8(MODE_SELECT)==2) wr_u8(MODE_SELECT,0x7d);
    mode=rd_u8(RECORDER_MODE);
    observe(hooks,BOOTSTRAP_FOLLOWUP_MODE,mode);
    if(mode==3) {
        wr_u8(POST_INPUT_EVENT,1); wr_u16(POST_INPUT_COUNTDOWN,2);
        wr_u8(VIEWPORT_TARGET,0); wr_u8(MODE_SELECT,0x7f); callback=0xc0fa04u;
    } else {
        wr_u8(RECORDER_MODE,0);
        hooks->consume(hooks->context,BOOTSTRAP_LOAD_MENU_TABLE); callback=0xc0fcb4u;
    }
    wr_u32(STAGE_CALLBACK,callback);
    observe(hooks,BOOTSTRAP_FOLLOWUP_CALLBACK,callback);
}
