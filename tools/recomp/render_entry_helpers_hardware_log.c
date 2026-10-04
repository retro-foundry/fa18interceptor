#include "render_leaf_helpers_hardware_log.c"
void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario) {fa18_render_leaf_controlled_begin(entry==0xc301f6u?0xc301f0u:entry,scenario);}
void fa18_render_entry_hardware_details(void) {fa18_render_leaf_hardware_details();}
