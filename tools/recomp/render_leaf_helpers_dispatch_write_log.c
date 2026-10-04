/* Dispatch baseline uses the same original sandbox commit policy. The
 * unchanged shared logger retains every actual source Custom packet. */
#include "structural_write_log.c"

void fa18_render_leaf_commit_sandbox_reference(void) {
 size_t i;
 fa18_write_log_active=0;
 for(i=0;i<custom_count;++i)
  fa18_custom_write(fa18_machine,custom_log[i].reg,custom_log[i].value);
}
