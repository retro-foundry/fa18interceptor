#include "service_dispatch_adapter.h"
#include "../amiga/service_dispatch.h"
#include "machine.h"
#include "m68kcpu.h"
#include "graphics_glue.h"
#include "graphics_wait_bovp.h"
#include "graphics_blitter_ownership.h"
#include "exec_glue.h"
#include "exec_task_lookup.h"
#include "exec_lists_adapter.h"
#include "exec_task_services_adapter.h"
#include "exec_supervisor.h"
#include "exec_memory_adapter.h"
#include "exec_scheduler_adapter.h"
#include "exec_interrupt_adapter.h"
#include "potgo_glue.h"
#include <stdio.h>
#include <stdlib.h>
#define ADAPTER(name) static int name##_adapter(void *context) { (void)context; return name(); }
ADAPTER(fa18_os_vbeam_step)
ADAPTER(fa18_os_wait_blit_step)
ADAPTER(fa18_os_wait_bovp_step)
ADAPTER(fa18_os_blitter_ownership_step)
ADAPTER(fa18_os_exec_interrupt_step)
ADAPTER(fa18_os_exec_get_msg_step)
ADAPTER(fa18_os_potgo_step)
ADAPTER(fa18_os_exec_find_task_step)
ADAPTER(fa18_os_exec_find_name_step)
ADAPTER(fa18_os_exec_lists_step)
ADAPTER(fa18_os_exec_messages_step)
ADAPTER(fa18_os_exec_signals_step)
ADAPTER(fa18_os_exec_task_protection_step)
ADAPTER(fa18_os_exec_supervisor_step)
ADAPTER(fa18_os_exec_memory_step)
ADAPTER(fa18_os_exec_scheduler_step)
ADAPTER(fa18_os_exec_irq_roots_step)
ADAPTER(fa18_os_exec_int_servers_step)
ADAPTER(fa18_os_exec_soft_interrupts_step)
/* Pinned 1.3 ABI identifiers, isolated from the neutral dispatcher. Disabled
 * until the reference runner verifies the corresponding source signature.
 * A future clean-start profile can activate proven implementations directly. */
static AmigaService services[FA18_SERVICE_COUNT]={
    {0xFC5ECE,0xFC5EDE,0xFC5ECE,"graphics.VBeamPos",0,fa18_os_vbeam_step_adapter,NULL},
    {0xFC5A58,0xFC5A7C,0xFC5A58,"graphics.WaitBlit",0,fa18_os_wait_blit_step_adapter,NULL},
    {0xFC5E58,0xFC5E9E,0xFC5E58,"graphics.WaitBOVP",0,fa18_os_wait_bovp_step_adapter,NULL},
    {0xFC64BC,0xFC653C,0xFC64BC,"graphics.blitter_ownership",0,fa18_os_blitter_ownership_step_adapter,NULL},
    {0xFC1428,0xFC1446,0xFC1428,"exec.interrupt_protection",0,fa18_os_exec_interrupt_step_adapter,NULL},
    {0xFC1BEA,0xFC1C18,0xFC1BEA,"exec.GetMsg",0,fa18_os_exec_get_msg_step_adapter,NULL},
    {0xFE44F2,0xFE4524,0xFE44F2,"potgo.WritePotgo",0,fa18_os_potgo_step_adapter,NULL},
    {0xFC1DB0,0xFC1E04,0xFC1DB0,"exec.FindTask",0,fa18_os_exec_find_task_step_adapter,NULL},
    {0xFC1696,0xFC16BE,0xFC1696,"exec.FindName",0,fa18_os_exec_find_name_step_adapter,NULL},
    {0xFC15E8,0xFC1696,0xFC15E8,"exec.lists",0,fa18_os_exec_lists_step_adapter,NULL},
    {0xFC1B76,0xFC1BEA,0xFC1B76,"exec.PutMsg",0,fa18_os_exec_messages_step_adapter,NULL},
    {0xFC1C18,0xFC1C5A,0xFC1C18,"exec.ReplyMsg/WaitPort",0,fa18_os_exec_messages_step_adapter,NULL},
    {0xFC1E54,0xFC1F74,0xFC1E54,"exec.signals",0,fa18_os_exec_signals_step_adapter,NULL},
    {0xFC1FCA,0xFC2048,0xFC1FCA,"exec.signal/trap_allocation",0,fa18_os_exec_signals_step_adapter,NULL},
    {0xFC1F74,0xFC1FBE,0xFC1F74,"exec.task_protection",0,fa18_os_exec_task_protection_step_adapter,NULL},
    {0xFC08E6,0xFC08F6,0xFC08E6,"exec.Supervisor",0,fa18_os_exec_supervisor_step_adapter,NULL},
    {0xFC090E,0xFC092C,0xFC090E,"exec.Supervisor_privilege",0,fa18_os_exec_supervisor_step_adapter,NULL},
    {0xFC1FBE,0xFC1FCA,0xFC1FBE,"exec.Permit_callback",0,fa18_os_exec_supervisor_step_adapter,NULL},
    {0xFC16D8,0xFC195A,0xFC16D8,"exec.memory",0,fa18_os_exec_memory_step_adapter,NULL},
    {0xFC0E9C,0xFC10C6,0xFC0EC2,"exec.scheduler",0,fa18_os_exec_scheduler_step_adapter,NULL},
    {0xFC0C88,0xFC0E9C,0xFC0D14,"exec.hardware_interrupts",0,fa18_os_exec_irq_roots_step_adapter,NULL},
    {0xFC11CA,0xFC1298,0xFC11CA,"exec.interrupt_vectors/servers",0,fa18_os_exec_int_servers_step_adapter,NULL},
    {0xFC1338,0xFC1428,0xFC135C,"exec.server_dispatch/Cause/soft_interrupts",0,fa18_os_exec_soft_interrupts_step_adapter,NULL}
};
void fa18_services_reset(void) {
    for (unsigned i=0;i<FA18_SERVICE_COUNT;++i) services[i].enabled=0;
}
void fa18_service_enable(unsigned service,int enabled) {
    if (service>=FA18_SERVICE_COUNT) abort();
    services[service].enabled=enabled!=0;
}
int fa18_services_step(void) {
    int result=amiga_services_step(services,FA18_SERVICE_COUNT,REG_PC,REG_PPC,&fa18_machine->runtime_guard);
    if (result<0) {
        fprintf(stderr,"invalid service registry at PC=%06X cycle=%llu\n",REG_PC,
                (unsigned long long)fa18_machine_now());
        exit(EXIT_FAILURE);
    }
    return result;
}
int fa18_services_requires_outer_dispatch(void) {
    return (services[FA18_SERVICE_EXEC_SCHEDULER].enabled &&
        fa18_os_exec_scheduler_requires_outer_dispatch(REG_PC)) ||
        (services[FA18_SERVICE_EXEC_IRQ_ROOTS].enabled &&
        fa18_os_exec_irq_requires_outer_dispatch(REG_PC));
}
