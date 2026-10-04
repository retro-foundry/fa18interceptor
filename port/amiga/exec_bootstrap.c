#include "exec_bootstrap.h"
#include "abi_13.h"
#include <stdio.h>
#include <string.h>
static int fail(char *error,size_t size,const char *why) {
    if (error && size) snprintf(error,size,"Exec handoff: %s",why); return 0;
}
static void word(uint8_t *p,uint16_t value) { p[0]=(uint8_t)(value>>8); p[1]=(uint8_t)value; }
static void empty(const AmigaGuestMemory *m,uint32_t address) {
    uint8_t *p=amiga_guest_range(m,address,12);
    amiga_store_be32(p,address+4); amiga_store_be32(p+4,0); amiga_store_be32(p+8,address);
}
int amiga_exec_bootstrap(const AmigaGuestMemory *m,const AmigaExecBootstrap *s,
                          char *error,size_t error_size) {
    if (!s || !amiga_guest_memory_valid(m) || !s->vectors || !s->vector_count || !s->task_name ||
        s->process_signal_bit>=32 ||
        (s->exec_base&1) || (s->task&1) || (s->initial_sp&1) || (s->return_pc&1) ||
        s->exec_base<s->negative_size || s->positive_size<0x24C || s->process_size<0xBC ||
        s->stack_lower>=s->stack_upper || s->initial_sp<s->stack_lower ||
        (uint64_t)s->initial_sp+4>s->stack_upper)
        return fail(error,error_size,"invalid process/profile");
    size_t name_size=strlen(s->task_name)+1;
    if (name_size>32) return fail(error,error_size,"process name exceeds the verified disk-name profile");
    uint32_t bases[]={s->exec_base-s->negative_size,s->task,s->stack_lower,4,s->task_name_address};
    uint32_t sizes[]={s->negative_size+s->positive_size,s->process_size,s->stack_upper-s->stack_lower,4,(uint32_t)name_size};
    for (unsigned i=0;i<5;++i) {
        if (!amiga_guest_range(m,bases[i],sizes[i])) return fail(error,error_size,"region outside guest RAM");
        for (unsigned j=0;j<i;++j)
            if (bases[i]<(uint64_t)bases[j]+sizes[j] && bases[j]<(uint64_t)bases[i]+sizes[i])
                return fail(error,error_size,"initial process regions overlap");
    }
    for (size_t i=0;i<s->vector_count;++i) {
        unsigned offset=s->vectors[i].offset;
        if (offset<6 || offset%6 || offset>s->negative_size ||
            s->vectors[i].target>0xFFFFFF || (s->vectors[i].target&1))
            return fail(error,error_size,"invalid library vector");
        for (size_t j=0;j<i;++j) if (offset==s->vectors[j].offset)
            return fail(error,error_size,"duplicate library vector");
    }
    /* Nothing above commits data. Owned regions begin clear; no bytes from a
     * prior machine or captured kernel are installed. */
    for (unsigned i=0;i<5;++i) memset(amiga_guest_range(m,bases[i],sizes[i]),0,sizes[i]);
    uint8_t *base=amiga_guest_range(m,s->exec_base,s->positive_size);
    base[AMIGA_NODE_TYPE]=9;
    word(base+AMIGA_LIBRARY_NEG_SIZE,s->negative_size); word(base+AMIGA_LIBRARY_POS_SIZE,s->positive_size);
    word(base+AMIGA_LIBRARY_VERSION,s->version); word(base+AMIGA_LIBRARY_REVISION,s->revision);
    amiga_store_be32(amiga_guest_range(m,4,4),s->exec_base);
    amiga_store_be32(base+AMIGA_EXEC_THIS_TASK,s->task);
    word(base+AMIGA_EXEC_ID_NEST_CNT,0xFFFF);
    empty(m,s->exec_base+AMIGA_EXEC_MEMORY_LIST);
    empty(m,s->exec_base+AMIGA_EXEC_RESOURCE_LIST); empty(m,s->exec_base+AMIGA_EXEC_DEVICE_LIST);
    empty(m,s->exec_base+AMIGA_EXEC_LIBRARY_LIST); empty(m,s->exec_base+AMIGA_EXEC_PORTS);
    empty(m,s->exec_base+AMIGA_EXEC_TASK_READY); empty(m,s->exec_base+AMIGA_EXEC_TASK_WAIT);
    for (unsigned i=0;i<5;++i) empty(m,s->exec_base+AMIGA_EXEC_SOFT_INTS+16*i);
    for (size_t i=0;i<s->vector_count;++i) {
        uint8_t *v=amiga_guest_range(m,s->exec_base-s->vectors[i].offset,6);
        word(v,0x4EF9); amiga_store_be32(v+2,s->vectors[i].target);
    }
    uint8_t *task=amiga_guest_range(m,s->task,s->process_size);
    task[AMIGA_NODE_TYPE]=13; task[AMIGA_TASK_STATE]=2;
    memcpy(amiga_guest_range(m,s->task_name_address,(uint32_t)name_size),s->task_name,name_size);
    amiga_store_be32(task+AMIGA_NODE_NAME,s->task_name_address);
    word(task+AMIGA_TASK_NEST_COUNTS,0xFFFF);
    amiga_store_be32(task+AMIGA_TASK_SIGNALS_ALLOCATED,s->signal_allocated);
    amiga_store_be32(task+AMIGA_TASK_TRAPS_ALLOCATED,s->trap_allocated);
    amiga_store_be32(task+AMIGA_TASK_SAVED_SP,s->initial_sp);
    amiga_store_be32(task+AMIGA_TASK_STACK_LOWER,s->stack_lower);
    amiga_store_be32(task+AMIGA_TASK_STACK_UPPER,s->stack_upper);
    task[AMIGA_PROCESS_MSG_PORT+AMIGA_NODE_TYPE]=4;
    task[AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_SIGNAL_BIT]=s->process_signal_bit;
    amiga_store_be32(task+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_SIGNAL_TASK,s->task);
    empty(m,s->task+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_MESSAGES);
    amiga_store_be32(amiga_guest_range(m,s->initial_sp,4),s->return_pc);
    if (error && error_size) error[0]=0; return 1;
}
