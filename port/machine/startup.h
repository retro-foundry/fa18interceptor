#ifndef FA18_MACHINE_STARTUP_H
#define FA18_MACHINE_STARTUP_H
#include "machine.h"
/* Explicit program-handoff state. The embedding OS profile supplies verified
 * device and CPU values. This API does not decode UAE state or initialize an
 * OS, copy captured RAM, load assets, or execute reset/boot ROM instructions. */
typedef struct {
    uint32_t d[8],a[7],usp,isp,pc;
    uint16_t sr;
    uint16_t custom[0x100];
    FA18Cia cia[2];
    unsigned vpos,hpos;
} FA18MachineStartup;
/* Clears all guest banks, starts the shared timeline, and enforces ROM-free
 * access from initialization onward. Assets and OS structures are installed
 * separately before preparing the first frame. Invalid input leaves the machine alone.
 * The caller-owned profile must not overlap the machine storage. */
int fa18_machine_init(FA18Machine *machine,const FA18MachineStartup *startup,
                      char *error,size_t error_size);
/* Prepare predicted DMA after assets, OS vectors and Copper data are installed.
 * A stopped newly initialized machine is required. This consumes no guest time. */
int fa18_machine_prepare_run(FA18Machine *machine,char *error,size_t error_size);
#endif
