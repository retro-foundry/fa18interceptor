/* Complete original depth-sort parents and retained planar lookup outputs.
 * Caller variants exist only in this external reference-process test. */
#define main records_reference_main
#include "native_records_oracle.c"
#undef main

static int same_ram(const uint8_t *expected,unsigned test) {
    for(unsigned i=0;i<0xff000;++i) {
        const uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"Depth case %u RAM %06X source=%02X native=%02X\n",
                test,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 0;
        }
    }
    return 1;
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!saved||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);memcpy(saved,m,sizeof *saved);
    for(unsigned test=0;test<128;++test) {
        memcpy(m,saved,sizeof *saved);
        const unsigned profile=test/16,count=profile==7?27:1+test%7;
        const int all=(test&8)!=0;
        const uint32_t incoming=0x53a90000u+test*0x20103u;
        wr_u8(SORT_LISTS_ON,(test%16)!=0);wr_u8(SORT_LIST_NEXT,(uint8_t)(test%4));wr_u8(SORT_LIST_COUNT,3);
        wr_u16(PROJECTION_WORDS,(uint16_t)(test*173));wr_u16(PROJECTION_WORDS+4,(uint16_t)(test*239));
        wr_u32(PROJECTION_Y,test&4?0x00800000u:0xffffbc17u);
        for(unsigned list=0;list<3;++list) {
            const gaddr address=0xc6e000u+list*0x400;
            wr_u32(SORT_LISTS+6*list,address);wr_u16(SORT_LISTS+6*list+4,(uint16_t)count);
            for(unsigned i=0;i<count;++i) {
                const gaddr entry=address+24*i;
                for(unsigned k=0;k<24;++k) wr_u8(entry+k,0);
                uint16_t flags=(uint16_t)(i%5);
                if(profile==0 || (profile==4 && i%3==0)) flags|=0x40;
                if(profile==3 || profile==5 || (profile==4 && i%3==2)) flags|=(uint16_t)(0x10+(i%4)*0x100);
                wr_u16(entry,flags);
                wr_u16(entry+6,(uint16_t)(731+i*277));wr_u16(entry+8,(uint16_t)(profile==6?0x8000:41+i*31));
                wr_u16(entry+10,(uint16_t)(159+i*389));wr_u32(entry+12,0x79130000u+list*0x10203+i*0x20131);
                if(profile==1 || (profile==4 && i%3==1)) wr_u16(entry+16,(uint16_t)(profile==1?17+i:2+i));
                if(profile==5) wr_u32(CONTROL_RECORDS+(i%4)*512+0x10,0x7fffffffu);
            }
        }
        wr_u8(0xc7ff54,(uint8_t)all);wr_u16(0xc7fe9c,0x6bad);
        memcpy(before,m,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[3]=incoming;REG_A[6]=0xc7ff80;REG_A[7]=0xc7ff00;
        wr_u32(REG_A[7],0xc70000);REG_PC=0xc1e328;m68k_set_reg(M68K_REG_SR,0x2700);
        fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 1;
        const uint16_t retained=rd_u16(0xc7fe9c);
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *before);
        DisplaySortResult result={.planar_factor=incoming,.retained_word=0x6bad,.has_factor=1};
        sort_display_list_retained(all,&result);
        if(result.retained_word!=retained || !same_ram(expected,test)) {
            fprintf(stderr,"Depth case %u retained source=%04X native=%04X\n",test,retained,result.retained_word);return 1;
        }
    }
    for(unsigned test=0;test<256;++test) {
        memcpy(m,saved,sizeof *saved);
        const int16_t x=(int16_t)(test*127),y=(int16_t)(test*181),z=(int16_t)(test*73);
        wr_u16(BOUND_SHIFT,(uint16_t)(test%16));wr_u8(POSITION_VALID,(uint8_t)(test&1));
        wr_u32(POSITION_LEVEL,test&4?0x7fffffffu:0xff765432u);
        wr_u32(PROJECTION_Y,test&2?0x00800000u:0xff123456u);
        memcpy(before,m,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[2]=(uint32_t)(int32_t)x;REG_D[3]=(uint32_t)(int32_t)y;
        REG_D[4]=(uint32_t)(int32_t)z;REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);REG_PC=0xc1d91a;
        m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 1;
        const uint32_t length=REG_D[1],factor=REG_D[3];
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *before);DistanceResult result=target_distance_result(x,y,z);
        if((uint32_t)result.length!=length || result.planar_factor!=factor || !same_ram(expected,test)) {
            fprintf(stderr,"Distance case %u length source=%08X native=%08X factor source=%08X native=%08X\n",
                test,length,(uint32_t)result.length,factor,result.planar_factor);return 1;
        }
    }
    puts("128 complete C1E328 retained-output/RAM cases and 256 C1D91A distance/factor/RAM cases match original");
    free(expected);free(before);free(saved);free(m);free(data);free(rom);free(state);return 0;
}
