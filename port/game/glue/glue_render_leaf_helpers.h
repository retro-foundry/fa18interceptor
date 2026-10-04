#ifndef FA18_GLUE_RENDER_LEAF_HELPERS_H
#define FA18_GLUE_RENDER_LEAF_HELPERS_H
#include "render_leaf_helpers.h"
RenderLeafState glue_render_leaf_state(void);
const RenderLeafHooks *glue_render_leaf_hooks(void);
int glue_C2F5C0(void);
int glue_C2F5C0_complete_step(void);
int glue_C2F5C0_owns(uint32_t pc);
int glue_C2F5D4(void);
int glue_C2F5D4_complete_step(void);
int glue_C2F5D4_owns(uint32_t pc);
int glue_C2F5F4(void);
int glue_C2F5F4_complete_step(void);
int glue_C2F5F4_owns(uint32_t pc);
int glue_C2F60A(void);
int glue_C2F60A_complete_step(void);
int glue_C2F60A_owns(uint32_t pc);
int glue_C2F66E(void);
int glue_C2F66E_complete_step(void);
int glue_C2F66E_owns(uint32_t pc);
int glue_C310E2(void);
int glue_C310E2_complete_step(void);
int glue_C310E2_owns(uint32_t pc);
int glue_C301F0(void);
int glue_C301F0_complete_step(void);
int glue_C301F0_owns(uint32_t pc);
int glue_C304B2(void);
int glue_C304B2_complete_step(void);
int glue_C304B2_owns(uint32_t pc);
int glue_C32806(void);
int glue_C32806_complete_step(void);
int glue_C32806_owns(uint32_t pc);
int glue_C2FA78(void);
int glue_C2FA78_complete_step(void);
int glue_C2FA78_owns(uint32_t pc);
int glue_C2FA7E(void);
int glue_C2FA7E_complete_step(void);
int glue_C2FA7E_owns(uint32_t pc);
int glue_C2FD8C(void);
int glue_C2FD8C_complete_step(void);
int glue_C2FD8C_owns(uint32_t pc);
void glue_render_leaf_extent_segment(uint32_t entry);
void glue_render_leaf_first_plane_wait(void);
#endif
