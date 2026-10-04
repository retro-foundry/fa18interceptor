/* Test-only original-byte child oracle. Both whole-call sides keep event
 * service outside the held fixture; actual source instructions and hardware
 * reads/writes run on the original CPU. No production glue invokes handlers.
 * This excludes the generated dispatcher's event-resume bookkeeping from
 * the child authority without changing the underlying source or devices. */
#include "glue.h"
#include "glue_child_call.h"
#include "bus.h"
#include "memory.h"
#include "m68kops.h"
#include <stdio.h>
#include <stdlib.h>
extern int64_t fa18_next_event;
void fa18_render_leaf_source_child(uint32_t ret,uint32_t sp) {
 while(REG_PC!=ret||A(7)!=sp){
  uint32_t pc=REG_PC;uint16_t opcode;
  fa18_next_event=INT64_MAX;fa18_bus_instruction();
  opcode=rd_u16(pc);REG_PPC=pc;REG_IR=opcode;REG_PC=pc+2;
  m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
 }
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
 uint32_t sp=A(7);(void)exit_pc;(void)exit_sp;(void)exited;
 m68ki_push_32(ret);REG_PC=entry;fa18_render_leaf_source_child(ret,sp);return (int32_t)D(0);
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
 return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
