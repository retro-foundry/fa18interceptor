/* Test-only ordered Custom-write and terminal chipset proof. The generated
 * machine copy renames only the Custom-write definition so internal bus calls
 * reach this observer. Original service and hardware results remain intact. */
#include "../../build/recomp/hud_stream_original_machine.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { uint32_t reg; uint16_t value; } HardwareWrite;
static HardwareWrite writes[4096],reference[4096];
static unsigned count,reference_count;
static int active;
static uint16_t custom[256],flags[4];
static uint64_t blits,line_blits;
static uint16_t latches[4],issued,draw_start;
static int pending,zero;
unsigned fa18_hud_hardware_count(void) { return reference_count; }
extern void fa18_hud_blitter_capture(uint16_t out[4]);
extern void fa18_hud_blitter_restore(void);
/* RAM/CPU snapshots alone leave a previous call's BBUSY state behind. Both
 * sides and every fixture start from the original loaded private machine
 * state, including its data latches. This does not manufacture a wait return. */
void fa18_hud_machine_restore(void) {
    static int saved;
    static int64_t clocks[6]; static int state[5]; static uint16_t draw[2];
    if(!saved) {
        clocks[0]=line_start; clocks[1]=last_boundary; clocks[2]=blit_end;
        clocks[3]=total_lines; clocks[4]=last_input_line; clocks[5]=fa18_cycle_origin;
        state[0]=frame_done; state[1]=line_started; state[2]=in_execute; state[3]=blit_pending; state[4]=blit_zero;
        draw[0]=bltsize_issued; draw[1]=fa18_bltsize_at_draw_start; saved=1;
    }
    line_start=clocks[0]; last_boundary=clocks[1]; blit_end=clocks[2];
    total_lines=clocks[3]; last_input_line=clocks[4]; fa18_cycle_origin=clocks[5];
    frame_done=state[0]; line_started=state[1]; in_execute=state[2]; blit_pending=state[3]; blit_zero=state[4];
    bltsize_issued=draw[0]; fa18_bltsize_at_draw_start=draw[1];
    fa18_hud_blitter_restore(); fa18_bus_reset(); fa18_next_event=INT64_MAX;
}
void fa18_custom_write(FA18Machine *m,uint32_t reg,uint16_t value) {
    if(active) {
        if(count==4096) { fputs("stream-store Custom-write proof exhausted\n",stderr); exit(3); }
        writes[count].reg=reg; writes[count].value=value; ++count;
    }
    fa18_original_custom_write(m,reg,value);
}
void fa18_hud_hardware_begin(void) { fa18_hud_machine_restore(); count=0; active=1; }
void fa18_hud_hardware_reference(void) {
    FA18Machine *m=fa18_machine;
    reference_count=count; memcpy(reference,writes,count*sizeof *writes);
    memcpy(custom,m->custom,sizeof custom); flags[0]=m->dmacon; flags[1]=m->intena; flags[2]=m->intreq; flags[3]=m->adkcon;
    blits=m->blits; line_blits=m->line_blits; active=0;
    fa18_hud_blitter_capture(latches); issued=bltsize_issued; draw_start=fa18_bltsize_at_draw_start; pending=blit_pending; zero=blit_zero;
}
int fa18_hud_hardware_check(void) {
    unsigned i; uint16_t current[4]; FA18Machine *m=fa18_machine; active=0;
    fa18_hud_blitter_capture(current);
    if(count!=reference_count) { fprintf(stderr,"Custom-write count source %u C %u\n",reference_count,count); return 0; }
    for(i=0;i<count;++i) if(writes[i].reg!=reference[i].reg || writes[i].value!=reference[i].value) {
        fprintf(stderr,"Custom write %u source %03X=%04X C %03X=%04X\n",i,reference[i].reg,reference[i].value,writes[i].reg,writes[i].value); return 0;
    }
    if(memcmp(custom,m->custom,sizeof custom) || flags[0]!=m->dmacon || flags[1]!=m->intena || flags[2]!=m->intreq || flags[3]!=m->adkcon || blits!=m->blits || line_blits!=m->line_blits || memcmp(latches,current,sizeof latches) || issued!=bltsize_issued || draw_start!=fa18_bltsize_at_draw_start || pending!=blit_pending || zero!=blit_zero) {
        fputs("stream-store terminal Custom/effective register or blit counter mismatch\n",stderr); return 0;
    }
    return 1;
}
