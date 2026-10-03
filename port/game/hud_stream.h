#ifndef FA18_HUD_STREAM_H
#define FA18_HUD_STREAM_H
#include "memory.h"
enum StreamField { HS_PRIMARY,HS_BASE,HS_CONTROL,HS_SECONDARY,HS_DESTINATION,HS_UNUSED,HS_SIZE,HS_OFFSET,
    HS_REGISTERS,HS_POINTERS,HS_STREAM,HS_RECORD,HS_SCREEN,HS_PLANES };
typedef struct {
    uint32_t primary,base,control,secondary,destination,unused,size,offset;
    gaddr registers,pointers,stream,record,screen,planes;
} HudStreamState;
enum StreamChild { HS_WAIT_NEXT,HS_WAIT_TABLE,HS_WAIT_OTHER,
    HS_BCD_FIRST,HS_DIGITS_FIRST,HS_BCD_SECOND,HS_DIGITS_SECOND,HS_BCD_THIRD,HS_DIGITS_THIRD,
    HS_LINE_FIRST,HS_LINE_SECOND };
enum StreamPhase { HS_WORD,HS_LONG,HS_POINTER,HS_ADD_WORD,HS_ADD_LONG,HS_NEG_WORD,HS_EXT_LONG,
    HS_COMPARE_WORD,HS_TEST_BYTE,HS_STORE_WORD,HS_STORE_LONG };
typedef struct {
    HudStreamState (*consume)(void *context,enum StreamChild child);
    void (*observe)(void *context,enum StreamPhase phase,enum StreamField field,uint32_t value,uint32_t other);
    void *context;
} HudStreamHooks;
void store_stream_cd(HudStreamState w,const HudStreamHooks *h);
void store_stream_ad(HudStreamState w,const HudStreamHooks *h);
void submit_next_stream_cd(HudStreamState w,const HudStreamHooks *h);
void submit_table_stream_ad(HudStreamState w,const HudStreamHooks *h);
void submit_other_stream_acd(HudStreamState w,const HudStreamHooks *h);
void draw_stream_numeric_fields(HudStreamState w,const HudStreamHooks *h);
void draw_bounded_stream_marker(HudStreamState w,int second,const HudStreamHooks *h);
#endif
