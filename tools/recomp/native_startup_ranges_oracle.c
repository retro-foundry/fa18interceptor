/* Sealed original startup instructions; native components never use this ABI. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/command_queue.c"
#include "../../port/startup_ranges.c"
#include "../../port/command_effects.h"
#include "../../build/recomp/native_startup_ranges_source.h"

typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18ContextCommandState context;
    FA18CommandEffects effects;
    FA18CommandQueue queue;
    FA18NativeStartupRanges startup;
    uint16_t other[52];
    PortFieldByte words[104];
    uint8_t auxiliary;
} StartupState;
static unsigned char startup_seen[sizeof startup_source_bytes/sizeof startup_source_bytes[0]];

static void bind_word_fields(PortFieldByte *bytes,uint16_t *word) {
    bytes[0]=(PortFieldByte){.unsigned_word=word,.shift=8};
    bytes[1]=(PortFieldByte){.unsigned_word=word};
}
static int load_startup(StartupState *s) {
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128]; unsigned i;
    memset(s,0,sizeof *s);
    s->flight.commands=&s->commands; s->view.flight=&s->flight; s->context.view=&s->view;
    for(i=0;i<sizeof neighbors;++i) neighbors[i]=rd_u8(KEY_RAW-128+i);
    for(i=0;i<128;++i) keys[i]=rd_u8(KEY_TABLE+i);
    if(!fa18_initialize_command_queue(&s->queue,&s->context,neighbors,sizeof neighbors,keys,sizeof keys)) return 0;
    if(!fa18_bind_command_queue_byte(&s->queue,0x34,&s->auxiliary)) return 0;
    for(i=0;i<52;++i) {
        s->other[i]=rd_u16(0xc458c0+2*i);
        bind_word_fields(s->words+2*i,s->other+i);
    }
    /* These references use actual already ported owners. All other supplied
     * words remain explicitly caller-owned imports, including the record
     * offset whose full native geometry/identity graph is not yet connected. */
    s->flight.spawn_gate=rd_u16(COMMAND_SPAWN_GATE);
    bind_word_fields(s->words+2,&s->flight.spawn_gate);
    s->flight.command_word=rd_u16(COMMAND_WORD);
    bind_word_fields(s->words+6,&s->flight.command_word);
    s->commands.indexed.cockpit_high_byte=rd_u8(COCKPIT_FLAGS);
    s->commands.indexed.cockpit_low_byte=rd_u8(COCKPIT_FLAGS+1);
    s->words[12]=(PortFieldByte){.byte=&s->commands.indexed.cockpit_high_byte};
    s->words[13]=(PortFieldByte){.byte=&s->commands.indexed.cockpit_low_byte};
    s->effects.message_state=rd_u16(0xc458ce);
    bind_word_fields(s->words+14,&s->effects.message_state);
    s->view.redraw_state_word=rd_u16(REDRAW_STATE_WORD);
    bind_word_fields(s->words+24,&s->view.redraw_state_word);
    s->view.redraw_state_long=rd_u32(REDRAW_STATE_LONG);
    for(i=0;i<4;++i) s->words[0x58+i]=(PortFieldByte){.longword=&s->view.redraw_state_long,.shift=24-8*i};
    return fa18_bind_native_startup_ranges(&s->startup,&s->queue,s->words,104);
}
static void store_startup(const StartupState *s) {
    unsigned i;
    for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) {
        uint8_t value;
        if(!port_read_field_byte(s->queue.slots+i,&value)) abort();
        wr_u8(KEY_RAW-128+i,value);
    }
    for(i=0;i<104;++i) {
        uint8_t value;
        if(!port_read_field_byte(s->words+i,&value)) abort();
        wr_u8(0xc458c0+i,value);
    }
}
static int original_startup(void) {
    unsigned step,i;
    for(step=0;step<512;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof startup_source_bytes/sizeof startup_source_bytes[0];++i)
            if(startup_source_bytes[i].pc==pc) break;
        if(i==sizeof startup_source_bytes/sizeof startup_source_bytes[0]) return 0;
        startup_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,j;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc090c2;
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof startup_source_bytes/sizeof startup_source_bytes[0];++i)
        for(j=0;j<startup_source_bytes[i].length;++j)
            if(rd_u8(startup_source_bytes[i].pc+j)!=startup_source_bytes[i].bytes[j]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        StartupState native; int ok;
        memcpy(m,base,sizeof *m);
        for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) wr_u8(KEY_RAW-128+i,(uint8_t)random_value());
        /* Include both sentinels, especially exclusive-end C45928. */
        for(i=0;i<108;++i) wr_u8(0xc458beu+i,(uint8_t)random_value());
        for(i=0;i<15;++i) REG_DA[i]=random_value();
        SET_CYCLES(100000000); REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000);
        m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31)); REG_PC=selected_entry;
        fa18_recomp_abort=0; fa18_next_event=INT64_MAX;
        if(!load_startup(&native)) return 1;
        memcpy(before,m,sizeof *m);
        if(!original_startup()) { fprintf(stderr,"original startup case %u failed\n",scenario); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        ok=selected_entry==0xc090c2?fa18_clear_native_startup_ranges(&native.startup):
            selected_entry==0xc090f2?fa18_enable_native_startup_ranges(&native.startup):0;
        store_startup(&native);
        if(!ok) return 1;
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(rd_u8(address)!=expected[i]) {
                fprintf(stderr,"startup %06X case %u byte %06X source %02X native %02X\n",
                    selected_entry,scenario,address,expected[i],rd_u8(address)); return 1;
            }
        }
    }
    printf("native startup %06X: %u complete calls match all RAM with no exclusions or child contracts\n",selected_entry,cases);
    printf("visited:"); for(i=0;i<sizeof startup_source_bytes/sizeof startup_source_bytes[0];++i)
        if(startup_seen[i]) printf(" %06X",startup_source_bytes[i].pc);
    puts(""); free(expected); free(before); free(base); free(m); return 0;
}
