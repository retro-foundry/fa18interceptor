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
    {0xFC15E8,0xFC1696,0xFC15E8,"exec.lists",0,fa18_os_exec_lists_step_adapter,NULL}
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
