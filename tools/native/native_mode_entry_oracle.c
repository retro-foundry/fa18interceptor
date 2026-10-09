/* Original C0F3C4/C0F5F8 versus an actual native input/stage interval.
 * Descriptor reads and timer requests retain the existing host contracts. */
#define main input_fixture_main
#include "native_input_oracle.c"
#undef main
#include "native_file_service_oracle.h"
static unsigned host_tick;
static uint16_t retained_sort;
static unsigned retained_sorts,cached_sort_entries,far_sort_entries;
static int startup_scene;
static int original_stage(void) {
    const int trace=getenv("FA18_MODE_STAGE_TRACE")!=NULL;
    const char *forced_factor=getenv("FA18_MODE_CONTEXT_FACTOR");
    uint32_t upper_writer=0;
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_PC=startup_scene?0xc0f812:0xc0f5f8;
    m68k_set_reg(M68K_REG_SR,0x2700);
    SET_CYCLES(1000000000);
    for(unsigned step=0;step<2000000;++step) {
        /* Execute C0F812's actual LINK -14 and its complete C08F26 call.
         * The native interval ends before C0F81C's unrelated text setup. */
        if(startup_scene && REG_PC==0xc0f81c && REG_A[7]==REG_A[6]-14) return 1;
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(file_oracle_service()) continue;
        /* A comparison-only dependency probe. Never supplied to native code. */
        if(forced_factor && REG_PC==0xc1c860)
            REG_D[3]=(uint32_t)strtoul(forced_factor,NULL,0);
        if(REG_PC==0xc1e4a6) {retained_sort=(uint16_t)(REG_D[3]>>16);++retained_sorts;}
        if(REG_PC==0xc1e3a0) ++cached_sort_entries;
        if(REG_PC==0xc1e38e) ++far_sort_entries;
        if(trace && (REG_PC==0xc1c860 || REG_PC==0xc1e328 || REG_PC==0xc1e48c || REG_PC==0xc1e4a6))
            fprintf(stderr,"Stage sort %06X: A6=%06X A7=%06X A4=%06X choice=%02X D3=%08X upper_writer=%06X request=%02X context=%02X\n",
                REG_PC,REG_A[6],REG_A[7],REG_A[4],rd_u8(REG_A[6]-0x2c),
                REG_D[3],upper_writer,rd_u8(UPDATE_MASK),rd_u8(CONTEXT_SELECT));
        if(REG_PC==0xc53c78) {
            wr_u32(MENU_TIME_REQUEST+32,host_tick/50);
            wr_u32(MENU_TIME_REQUEST+36,(host_tick%50)*20000u);
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        const int cycles=GET_CYCLES();
        const uint32_t pc=REG_PC,upper=REG_D[3]&0xffff0000u;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        if(trace && (REG_D[3]&0xffff0000u)!=upper) upper_writer=pc;
        fa18_machine->cycle+=cycles-GET_CYCLES();
    }
    fprintf(stderr,"Mode stage did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    if(argc<4) return 1;
    startup_scene=argc==5 && !strcmp(argv[4],"startup");
    size_t ns=0,nr=0,nb=0,na=0;char error[256];
    uint8_t *state=read_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=read_bytes("local/system/kick13.rom",&nr);
    uint8_t *before=read_bytes(argv[1],&nb),*after=read_bytes(argv[2],&na);
    FA18Machine *m=calloc(1,sizeof *m);NativeFrontend *game=calloc(1,sizeof *game);
    if(!state || !rom || !before || !after || nb!=0x100000 || na!=nb || !m || !game) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    uint32_t plane_offsets[4];
    for(unsigned i=0;i<4;++i)
        plane_offsets[i]=rd_u32(PAGE0_PLANE_TABLE+4*i)-rd_u32(PAGE0_PLANE_TABLE);
    memcpy(m->chip,before,0x80000);memcpy(m->slow,before+0x80000,0x80000);
    /* Absolute host allocation is free; fixed renderer plane offsets must
     * agree with the retained original bitmap, including their ordering. */
    for(unsigned page=0;page<2;++page) for(unsigned i=0;i<4;++i) {
        gaddr table=PAGE0_PLANE_TABLE+16*page;
        if(rd_u32(table+4*i)-rd_u32(table)!=plane_offsets[i]) {
            fputs("Native plane spacing differs from the original bitmap\n",stderr);return 1;
        }
    }
    m->joy1dat=0;
    if(!startup_scene) for(int i=4;i<argc;++i) native_input_enqueue_raw(game,(uint8_t)strtoul(argv[i],NULL,10));
    host_tick=(unsigned)strtoul(argv[3],NULL,10);
    const char *retained_before=getenv("FA18_MODE_RETAINED_BEFORE");
    const char *retained_after=getenv("FA18_MODE_RETAINED_AFTER");
    if((retained_before!=NULL)!=(retained_after!=NULL)) return 1;
    if(retained_before) retained_sort=(uint16_t)strtoul(retained_before,NULL,10);
    const gaddr stage=rd_u32(STAGE_CALLBACK);
    if(!file_oracle_reset(game)) return 1;
    if((!startup_scene && !original_input(game)) || !original_stage()) return 1;
    unsigned differences=0;
    for(unsigned i=0;i<0xff000;++i) {
        const uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
        if(actual==after[i]) continue;
        /* The C0F920 bootstrap reaches these native record-motion locals.
         * Every drawing byte and every game-side global stays compared. */
        if((i>=0x46fc && i<0x4718) || (i>=0x47ca && i<0x4800)) continue;
        if(differences<20) fprintf(stderr,"entry %06X source %02X native %02X\n",
            i<0x80000?i:0xc00000+i-0x80000,actual,after[i]);
        ++differences;
    }
    printf("Actual input/stage %06X: %u compared RAM differences\n",stage,differences);
    if(retained_after) {
        const uint16_t expected=(uint16_t)strtoul(retained_after,NULL,10);
        printf("Stage retained sort: %u lists, %u cached entries, %u fixed-far entries, source=%04X native=%04X\n",
            retained_sorts,cached_sort_entries,far_sort_entries,retained_sort,expected);
        if(retained_sort!=expected) ++differences;
    }
    if(file_oracle_writes) printf("Complete original file owners reached DOS Write %u time(s)\n",file_oracle_writes);
    file_oracle_close(game);
    free(game);free(m);free(after);free(before);free(rom);free(state);
    return differences!=0;
}
