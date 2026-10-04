/* Retain the shared ordered-write and terminal-state observer. Controlled
 * parent blit tests additionally use the original machine's CPU clock and
 * submit a real initial blit, so BBUSY settles from actual elapsed cycles.
 * No read result, instruction or production hardware implementation changes. */
#include "hud_stream_hardware_log.c"
void fa18_hud_render_controlled_begin(uint32_t entry,unsigned scenario) {
    fa18_hud_hardware_begin();
    if(entry!=0xc3003au && entry!=0xc304fau)return;
    in_execute=1;fa18_cycle_origin=fa18_machine->cycle+GET_CYCLES();
    if(scenario&1u){
        fa18_custom_write(fa18_machine,0x40,0x100);
        fa18_custom_write(fa18_machine,0x42,0);
        fa18_custom_write(fa18_machine,0x54,0);
        fa18_custom_write(fa18_machine,0x56,0x1800);
        fa18_custom_write(fa18_machine,0x58,0xfc01);
    }
    fa18_next_event=INT64_MAX;
}
