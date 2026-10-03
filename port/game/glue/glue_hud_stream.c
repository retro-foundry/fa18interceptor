#include "glue_hud_stream_math.h"
#include "glue_hud_stream.h"
#include "glue_child_call.h"
#include "hud_stream.h"
static HudStreamState working(void) {
    HudStreamState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5)}; return w;
}
static HudStreamState consume(void *context,enum StreamChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc53f44,0xc308e2},{0xc53f44,0xc30904},{0xc53f44,0xc30f56},
        {0xc25a08,0xc31b92},{0xc32ac8,0xc31bae},{0xc25a08,0xc31bce},
        {0xc32ac8,0xc31bea},{0xc25a08,0xc31c02},{0xc32ac8,0xc31c1e},
        {0xc2fa7e,0xc33b04},{0xc2fa7e,0xc33b34}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum StreamPhase phase,enum StreamField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; (void)context;
    switch(phase) {
    case HS_WORD: SET_W(D(r),v); flags_logic_w(v); break;
    case HS_LONG: D(r)=v; flags_logic_l(v); break;
    case HS_POINTER: A(r-HS_REGISTERS)=v; break;
    case HS_ADD_WORD: step_add_word(&D(r),v); break; case HS_ADD_LONG: step_add_long(&D(r),v); break;
    case HS_NEG_WORD: renderer_negate(&D(r),2); break;
    case HS_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HS_COMPARE_WORD: step_compare_word(other,v); break;
    case HS_TEST_BYTE: flags_logic_b(v); break; case HS_STORE_WORD: flags_logic_w(v); break; case HS_STORE_LONG: flags_logic_l(v); break;
    }
}
static const HudStreamHooks hooks={consume,outputs,NULL};
int glue_C308E2(void) { store_stream_cd(working(),&hooks); return glue_return(); }
int glue_C30904(void) { store_stream_ad(working(),&hooks); return glue_return(); }
int glue_C308D8(void) { submit_next_stream_cd(working(),&hooks); return glue_return(); }
int glue_C308F4(void) { submit_table_stream_ad(working(),&hooks); return glue_return(); }
int glue_C30F46(void) { submit_other_stream_acd(working(),&hooks); return glue_return(); }
int glue_C31B76(void) { draw_stream_numeric_fields(working(),&hooks); return glue_return(); }
int glue_C33AD6(void) { draw_bounded_stream_marker(working(),0,&hooks); return glue_return(); }
int glue_C33B06(void) { draw_bounded_stream_marker(working(),1,&hooks); return glue_return(); }
