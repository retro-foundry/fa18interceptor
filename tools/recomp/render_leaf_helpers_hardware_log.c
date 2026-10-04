/* Actual machine clock, unchanged shared hardware observer. Multiple parent
 * blits require a running clock; reads are never replaced with canned values. */
#include "hud_stream_hardware_log.c"
void fa18_render_leaf_hardware_details(void) {
 unsigned i;uint16_t now[4];FA18Machine *m=fa18_machine;
 fa18_hud_blitter_capture(now);
 for(i=0;i<256;++i)if(custom[i]!=m->custom[i])fprintf(stderr,"terminal Custom %03X source %04X C %04X\n",2*i,custom[i],m->custom[i]);
 for(i=0;i<4;++i)if(latches[i]!=now[i])fprintf(stderr,"terminal latch %u source %04X C %04X\n",i,latches[i],now[i]);
 fprintf(stderr,"terminal blits %llu/%llu line %llu/%llu issued %04X/%04X draw %04X/%04X pending %d/%d zero %d/%d cycles %d\n",(unsigned long long)blits,(unsigned long long)m->blits,(unsigned long long)line_blits,(unsigned long long)m->line_blits,issued,bltsize_issued,draw_start,fa18_bltsize_at_draw_start,pending,blit_pending,zero,blit_zero,GET_CYCLES());
}
void fa18_render_leaf_controlled_begin(uint32_t entry,unsigned scenario) {
 fa18_hud_hardware_begin();
 if(entry==0xc2fa78u||entry==0xc2fa7eu||entry==0xc301f0u||entry==0xc304b2u||entry==0xc2fd8cu){
  in_execute=1;fa18_cycle_origin=fa18_machine->cycle+GET_CYCLES();
  if(scenario&1u){
   fa18_custom_write(fa18_machine,0x40,0x100);fa18_custom_write(fa18_machine,0x42,0);
   fa18_custom_write(fa18_machine,0x54,0);fa18_custom_write(fa18_machine,0x56,0x1800);fa18_custom_write(fa18_machine,0x58,0xfc01);
  }
  fa18_next_event=INT64_MAX;
 }
}
