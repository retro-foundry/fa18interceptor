/* Exercise the real runner from the original flight call site using the
 * source-derived component inputs. This is a frame fixture, not a recording. */
#include "machine.h"
#include "m68kcpu.h"
#include "bus.h"
#include "memory.h"
#include "globals.h"
#include "glue_flight_record_calls.h"
#include "recomp_ports.h"
#include "recomp_runtime.h"
static int fixture_loader(FA18Machine *,const uint8_t *,size_t,
                          const uint8_t *,size_t,char *,size_t);
#define fa18_machine_load_state fixture_loader
#define main fa18_autopilot_runner_main
#include "../../port/recomp/recomp_main.c"
#undef main
#undef fa18_machine_load_state

int fa18_autopilot_frame_fixture;
static uint32_t seed=0xc0f5f8u;
static uint32_t selected_entry=0xc2c392u;
static uint32_t random_value(void) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed;
}
extern int64_t fa18_next_event;
extern int fa18_write_log_active;
extern void fa18_structural_reset_write_log(void);
extern uint64_t fa18_structural_native_edge_calls(uint32_t,uint32_t);
extern void fa18_flight_record_actions_fixture_begin(const char *);
#include "record_autopilot_fixture.h"

static int fixture_loader(FA18Machine *m,const uint8_t *state,size_t size,
                          const uint8_t *rom,size_t rom_size,char *error,size_t error_size) {
    const char *value=getenv("FA18_RECORD_ACTION_CASE");
    unsigned scenario=value?(unsigned)strtoul(value,NULL,10):0,i;
    int saved_cycles; int64_t saved_event;
    if(!fa18_machine_load_state(m,state,size,rom,rom_size,error,error_size)) return 0;
    saved_cycles=GET_CYCLES(); saved_event=fa18_next_event;
    /* Ten PRNG draws per preceding component scenario. */
    for(i=0;i<scenario*10u;++i) random_value();
    fixture(scenario);
    SET_CYCLES(saved_cycles); fa18_next_event=saved_event;
    wr_u16(0xc65000u,0x180);
    wr_u16(0xc459b4u,1);
    wr_u16(0xc458ccu,rd_u16(0xc458ccu)|64);
    wr_u16(0xc458dau,0);
    wr_u8(0xc45788u,0);
    REG_PC=0xc22d88u; /* Original JSR C25B66; native child is selected there. */
    fa18_autopilot_frame_fixture=1;
    return 1;
}

static int locate(void) {
    const uint32_t entries[]={0xc2ca26u,0xc2ca92u,0xc2caa0u,0xc2cb86u,0xc2cb82u,0xc2cbbcu};
    unsigned found=0,seen=0,scenario,i;
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    char error[256];
    if(!state || !rom || !m || !base ||
       !fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    fa18_meter_start(1); fa18_bus_timing=0;
    for(scenario=0;scenario<49600 && found<6;++scenario) {
        memcpy(m,base,sizeof *m); fixture(scenario);
        fa18_structural_reset_write_log(); fa18_write_log_active=0;
        fa18_flight_record_actions_fixture_begin("steering fixture locator");
        glue_record_action_reference();
        for(i=0;i<6;++i) if(!(seen&(1u<<i)) &&
           fa18_structural_native_edge_calls(0xc2c392u,entries[i])) {
            printf("%06X %u\n",entries[i],scenario); seen|=1u<<i; ++found;
        }
    }
    free(state); free(rom); free(base); free(m);
    return found==6?0:1;
}

int main(int argc,char **argv) {
    if(argc==2 && !strcmp(argv[1],"--locate")) return locate();
    return fa18_autopilot_runner_main(argc,argv);
}
