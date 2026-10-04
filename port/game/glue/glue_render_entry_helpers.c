#include "glue.h"
#include "glue_child_call.h"
#include "glue_render_leaf_helpers.h"
#include "glue_render_entry_helpers.h"

static RenderLeafState entry_child(void *context,enum RenderLeafChild child) {
 if(child==RL_CALL_C2F664) {
  glue_complete_child(0xc2f688u,0xc2f668u);
  return glue_render_leaf_state();
 }
 return glue_render_leaf_hooks()->consume(context,child);
}
static RenderLeafHooks entry_hooks(void) {
 RenderLeafHooks hooks=*glue_render_leaf_hooks();hooks.consume=entry_child;return hooks;
}
int glue_C2F688(void) {
 const RenderLeafHooks *hooks=glue_render_leaf_hooks();
 /* Every original incoming setup selects C2F786 or C2F7E6; all slots and
  * callers are sealed in the source inventory. Preserve the actual A4 load. */
 render_leaf_pixel_shared(glue_render_leaf_state(),hooks,A(4)==0xc2f7e6u?2:1);
 return glue_return();
}
int glue_C2F63A(void) {RenderLeafHooks h=entry_hooks();render_leaf_square_in_view(glue_render_leaf_state(),&h);return glue_return();}
int glue_C2F64E(void) {RenderLeafHooks h=entry_hooks();render_leaf_square(glue_render_leaf_state(),&h);return glue_return();}
int glue_C301F6(void) {render_leaf_polygon(glue_render_leaf_state(),glue_render_leaf_hooks());return glue_return();}
int glue_C330FE(void) {render_leaf_glyph8(glue_render_leaf_state(),glue_render_leaf_hooks());return glue_return();}
