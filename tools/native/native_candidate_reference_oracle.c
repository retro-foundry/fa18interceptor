/* C26EBE reference output and memory compared with complete original bytes.
 * The input is a real mission before-state; seeded caller variants stay in
 * this reference-process component test, never in playable flight. */
#define main records_reference_main
#include "native_records_oracle.c"
#undef main

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!saved||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {
        fprintf(stderr,"%s\n",error);return 1;
    }
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    memcpy(saved,m,sizeof *saved);
    unsigned probes=0,early=0,levels=0;
    for(unsigned test=0;test<64;++test) {
        memcpy(m,saved,sizeof *saved);
        const unsigned slot=test%16,profile=test/16;
        const gaddr record=CONTROL_RECORDS+512*slot;
        const gaddr incoming=CONTROL_RECORDS+512*((slot+3)%16);
        wr_u16(SCRIPT_RECORD,(uint16_t)(512*slot));
        wr_u16(VIEW_RECORD,(uint16_t)(profile&1?512*slot:0));
        if(profile==3) wr_u8(record+0x62,0x20); /* Original early-return gate. */
        int32_t point[3];
        for(unsigned i=0;i<3;++i)
            point[i]=(int32_t)(rd_u32(record+20+4*i)-(profile==1?rd_u32(record+62+4*i):0));
        memcpy(before,m,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_A[3]=incoming;REG_A[6]=0xc7ff80;
        REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);REG_PC=0xc26ebe;
        REG_D[2]=(uint32_t)point[0];REG_D[3]=(uint32_t)point[1];REG_D[4]=(uint32_t)point[2];
        m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 1;
        const uint32_t result=REG_D[0];const gaddr reference=REG_A[3];
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *before);
        CandidateUpdateWork work={.reference_record=incoming};
        int actual=update_candidate_record(&work,point[0],point[1],point[2]);
        if((uint32_t)actual!=result || work.reference_record!=reference) {
            fprintf(stderr,"Candidate case %u: result source=%08X native=%08X; reference source=%06X native=%06X\n",
                test,result,(uint32_t)actual,reference,work.reference_record);return 1;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t byte=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(byte!=expected[i]) {
                fprintf(stderr,"Candidate case %u: RAM %06X source=%02X native=%02X\n",
                    test,i<0x80000?i:0xc00000+i-0x80000,expected[i],byte);return 1;
            }
        }
        probes+=work.had_probe!=0;early+=work.path==CANDIDATE_UPDATE_EARLY;
        levels+=work.path==CANDIDATE_UPDATE_TERMINAL || work.path==CANDIDATE_UPDATE_LEVEL;
    }
    if(!probes||!early||!levels) {
        fprintf(stderr,"Candidate fixtures missed an output path: probe=%u early=%u level=%u\n",probes,early,levels);return 1;
    }
    printf("64 complete C26EBE result/reference/RAM cases match original: probe=%u early=%u level=%u\n",probes,early,levels);
    free(expected);free(before);free(saved);free(m);free(data);free(rom);free(state);return 0;
}
