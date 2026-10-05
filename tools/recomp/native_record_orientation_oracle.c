/* Musashi and captured RAM are independent validation inputs only. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_orientation_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/flight.c"
#include "../../port/two_angle_matrix.c"
#include "../../port/native_record_orientation.c"
#include "../../port/disk.h"

enum { TRIG_BASE=0xc3e5e8, TRIG_MIN=-32768, TRIG_BYTES=65536 };
typedef struct {
    FA18FlightTrigData data;
    uint8_t bytes[TRIG_BYTES];
} TrigFixture;

static void load_trig_fixture(TrigFixture *fixture) {
    unsigned i;
    for(i=0;i<TRIG_BYTES;++i) fixture->bytes[i]=rd_u8(TRIG_BASE+TRIG_MIN+i);
    fixture->data=(FA18FlightTrigData){.bytes=fixture->bytes,.byte_count=TRIG_BYTES,.quarter_offset=-TRIG_MIN};
}
static int orientation_source(void) {
    unsigned step,i;
    for(step=0;step<1000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected orientation PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
static int verify_trig_asset(void) {
    FA18Disk disk; FA18Hunks hunks; FA18FlightTrigData data;
    size_t size=0; uint8_t *image; unsigned i; int16_t sine,cosine;
    if(!fa18_disk_open(&disk,"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf")) return 0;
    image=fa18_disk_read(&disk,"F-18 Interceptor",&size);
    if(!image || !fa18_hunks_load(&hunks,image,size) || fa18_load_native_trig_data(&hunks,&data)!=0) return 0;
    for(i=0;i<0x70a;++i) if(data.bytes[data.quarter_offset+i]!=rd_u8(TRIG_BASE+i)) return 0;
    if(fa18_flight_lookup_trig_data(&data,0,&sine,&cosine)!=0 || sine!=0 || cosine!=0x4000) return 0;
    /* The binding must read current asset values, including the prefix. */
    hunks.segments[63].data[data.quarter_offset]=0x12;
    hunks.segments[63].data[data.quarter_offset+1]=0x34;
    if(fa18_flight_lookup_trig_data(&data,0,&sine,&cosine)!=0 || sine!=0x1234) return 0;
    printf("trig asset: 1802 original Hunk-63 quarter-table bytes match; live binding verified\n");
    fa18_hunks_free(&hunks); free(image); fa18_disk_close(&disk); return 1;
}
static int verify_lookup_domain(TrigFixture *fixture) {
    unsigned angle,i; int16_t sine,cosine;
    /* Fill only read-only lookup data. The full record proof restores it. */
    for(i=0;i<TRIG_BYTES;++i) wr_u8(TRIG_BASE+TRIG_MIN+i,(uint8_t)random_value());
    load_trig_fixture(fixture);
    for(angle=0;angle<65536;++angle) {
        REG_D[4]=angle; REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=0xc2e6da;
        SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
        if(!orientation_source() || fa18_flight_lookup_trig_data(&fixture->data,(uint16_t)angle,&sine,&cosine)!=0 ||
           sine!=(int16_t)REG_D[4] || cosine!=(int16_t)REG_D[5]) {
            fprintf(stderr,"lookup differs angle %04X\n",angle); return 0;
        }
    }
    for(i=0;i<TRIG_BYTES;++i) if(fixture->bytes[i]!=rd_u8(TRIG_BASE+TRIG_MIN+i)) return 0;
    puts("trig domain: all 65536 word angles match the actual original lookup; data unchanged");
    return 1;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    SceneState *native=malloc(sizeof *native); TrigFixture *trig=malloc(sizeof *trig);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,j;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc2d954;
    if(!state || !rom || !m || !base || !before || !expected || !native || !trig || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j)
            if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    if(!verify_trig_asset() || !verify_lookup_domain(trig)) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        uint16_t angles[3]; unsigned record=scenario%16; int ok;
        memcpy(m,base,sizeof *m); scene_fixture(scenario);
        REG_A[1]=CONTROL_RECORDS+512*record;
        for(i=0;i<3;++i) {
            static const uint16_t boundary[]={0,1,0x7080,0x7081,0x8000,0xffff,0x1234,0xfffe};
            angles[i]=scenario<32?boundary[(scenario/4+i)%8]:(uint16_t)random_value();
            if(scenario%13==i) angles[i]=0;
            REG_D[4+i]=angles[i]|(random_value()&0xffff0000u);
        }
        /* Test arbitrary adjacent words and every signed product, including
         * $8000*$8000 and wrapped accumulation, independently of ROM trig. */
        if(scenario%4==1) for(i=0;i<TRIG_BYTES;++i) wr_u8(TRIG_BASE+TRIG_MIN+i,(uint8_t)random_value());
        if(scenario%4==2) for(i=0;i<TRIG_BYTES;i+=2) wr_u16(TRIG_BASE+TRIG_MIN+i,0x8000);
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        load_trig_fixture(trig); memcpy(before,m,sizeof *m);
        if(!orientation_source()) { fprintf(stderr,"original orientation failed case %u\n",scenario); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(selected_entry==0xc2d954) ok=fa18_publish_native_record_orientation(native->bank.records+record,angles,&trig->data);
        else if(selected_entry==0xc2d94e) ok=fa18_reset_native_record_orientation(native->bank.records+record,angles,&trig->data);
        else return 1;
        if(!ok) return 1;
        store_scene(native);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(address>=0xc7fd00 && address<0xc7ff00) continue;
            if(rd_u8(address)!=expected[i]) {
                fprintf(stderr,"orientation %06X case %u byte %06X source %02X native %02X\n",
                    selected_entry,scenario,address,expected[i],rd_u8(address)); return 1;
            }
        }
        if(!verify_record_owners(native)) return 1;
    }
    printf("native orientation %06X: %u complete calls match all game RAM and live matrix owners; no child contracts\n",selected_entry,cases);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(trig); free(native); free(expected); free(before); free(base); free(m); return 0;
}
