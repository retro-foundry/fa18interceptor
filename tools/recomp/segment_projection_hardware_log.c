#include "render_leaf_helpers_hardware_log.c"
void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario) {
 uint32_t clock_entry=(entry==0xc1ff9cu||entry==0xc1ffa4u||entry==0xc2ed70u||entry==0xc2ee4au)?0xc2fa7eu:entry;
 fa18_render_leaf_controlled_begin(clock_entry,scenario);
}
void fa18_render_entry_hardware_details(void){fa18_render_leaf_hardware_details();}
