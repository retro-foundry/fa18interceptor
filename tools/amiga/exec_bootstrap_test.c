/* Packed guest structures and atomic validation, independent of a game/SDK. */
#include "exec_bootstrap.h"
#include "abi_13.h"
#include "hunk.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint8_t ram[8192],before[8192]; char error[160];
    AmigaGuestBank bank={0,sizeof ram,AMIGA_MEMORY_CHIP,ram};
    AmigaGuestMemory memory={&bank,1};
    AmigaLibraryVector vectors[]={{6,0xFC08E6},{12,0xFC1DB0}};
    AmigaExecBootstrap profile={
        .exec_base=0x300,.task=0x800,.stack_lower=0x1000,.stack_upper=0x1800,
        .initial_sp=0x17FC,.return_pc=0xEF0000,.signal_allocated=0xFFFF,
        .trap_allocated=0x80000000,.task_name_address=0x1900,
        .negative_size=12,.positive_size=0x24C,.version=34,.revision=2,.process_size=0xBC,
        .process_signal_bit=8,.task_name="test",.vectors=vectors,.vector_count=2};
    memset(ram,0xA5,sizeof ram); memcpy(before,ram,sizeof ram);
    AmigaExecBootstrap bad=profile; bad.stack_lower=profile.task;
    assert(!amiga_exec_bootstrap(&memory,&bad,error,sizeof error) && !memcmp(ram,before,sizeof ram));
    bad=profile; bad.initial_sp=0x1800;
    assert(!amiga_exec_bootstrap(&memory,&bad,error,sizeof error) && !memcmp(ram,before,sizeof ram));
    bad=profile; bad.process_signal_bit=32;
    assert(!amiga_exec_bootstrap(&memory,&bad,error,sizeof error) && !memcmp(ram,before,sizeof ram));
    vectors[1].offset=6;
    assert(!amiga_exec_bootstrap(&memory,&profile,error,sizeof error) && !memcmp(ram,before,sizeof ram));
    vectors[1].offset=12; vectors[1].target=0x1000000;
    assert(!amiga_exec_bootstrap(&memory,&profile,error,sizeof error) && !memcmp(ram,before,sizeof ram));
    vectors[1].target=0xFC1DB0;
    assert(amiga_exec_bootstrap(&memory,&profile,error,sizeof error));
    assert(amiga_be32(ram+4)==profile.exec_base && amiga_be32(ram+0x300+AMIGA_EXEC_THIS_TASK)==profile.task);
    assert(amiga_be16(ram+0x300+AMIGA_EXEC_ID_NEST_CNT)==0xFFFF);
    assert(amiga_be16(ram+0x300+AMIGA_LIBRARY_VERSION)==34);
    assert(amiga_be16(ram+0x2FA)==0x4EF9 && amiga_be32(ram+0x2FC)==0xFC08E6);
    assert(ram[0x800+AMIGA_NODE_TYPE]==13 && ram[0x800+AMIGA_TASK_STATE]==2);
    assert(amiga_be32(ram+0x800+AMIGA_TASK_STACK_LOWER)==0x1000);
    assert(amiga_be32(ram+0x17FC)==0xEF0000 && !strcmp((char *)ram+0x1900,"test"));
    uint32_t list=0x800+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_MESSAGES;
    assert(amiga_be32(ram+list)==list+4 && !amiga_be32(ram+list+4) && amiga_be32(ram+list+8)==list);
    assert(ram[0x800+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_SIGNAL_BIT]==8);
    assert(amiga_be32(ram+0x800+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_SIGNAL_TASK)==profile.task);
    assert(ram[0x700]==0xA5 && ram[0x1800]==0xA5 && ram[0x1905]==0xA5);
    puts("Exec process handoff: packed vectors/fields/lists and atomic rejection pass"); return 0;
}
