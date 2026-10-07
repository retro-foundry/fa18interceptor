/* Complete original result/restart/config parents. Only actual OS services
 * use the shared host file backend; every game-side file decision executes. */
#define FA18_QUALIFICATION_ORACLE_LIBRARY
#define FA18_QUALIFICATION_SAVE_BACKEND
#include "native_qualification_oracle.c"
#define FA18_CONFIG_NATIVE_RUNTIME
#include "native_file_service_oracle.h"
static int source_result(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;REG_A[6]=0xc7ff70;
    wr_u32(REG_A[7],0xc70000);REG_PC=entry;
    m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(file_oracle_service()) continue;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"result source did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns),*rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);uint8_t *expected=malloc(0x100000);
    NativeFrontend *game=calloc(1,sizeof *game);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected||!game) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;native_clock_set(12358);
    const gaddr entries[]={0xc11078,0xc110a4,0xc0f946,0xc0f974,0xc0f992};
    uint32_t disk_tag=0;
    static const char *const protected_paths[]={"config"};
    for(unsigned test=0;test<77;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        unsigned owner=test%5,variant=test/5;gaddr entry=entries[owner];
        wr_u8(MODE_SELECT,9);wr_u8(RECORDER_MODE,0);wr_u8(PLAYER_PHASE,variant==2?0xef:0xff);
        wr_u16(POST_INPUT_COUNTDOWN,variant==0?2:0xffff);
        wr_u8(VIEWPORT_MODE,0);wr_u8(VIEWPORT_TARGET,variant==2?15:0);wr_u8(MODE_TABLE_CHANGED,1);
        if(test>=15 && test<39) {
            owner=1;entry=entries[owner];wr_u16(POST_INPUT_COUNTDOWN,0xffff);
            wr_u8(MODE_SELECT,(uint8_t)(4+(test-15)%4));
            wr_u8(PLAYER_PHASE,(uint8_t[]){0xfe,0xfd,2}[((test-15)/4)%3]);
            wr_u16(0xc458da,(uint16_t)((test-15)/12));
        }
        if(test>=39 && test<63) {
            owner=1;entry=entries[owner];
            const unsigned variant=(test-39)/4;
            const uint8_t levels[]={0,1,2,3,0x7f,0xff};
            const uint8_t attempts[]={0,2,3,0xfe,0xff,0x80};
            wr_u8(MODE_SELECT,(uint8_t)(4+(test-39)%4));wr_u8(PLAYER_PHASE,0xfc);
            wr_u16(POST_INPUT_COUNTDOWN,0xffff);
            wr_u8(SCENE_DISPATCH_LIMIT,levels[variant]);
            wr_u8(rd_u32(MODE_TABLE)+18+rd_u8(MODE_SELECT),attempts[variant]);
            wr_u16(rd_u32(MODE_TABLE)+0x38,variant&1?0xffff:0);
        }
        if(test>=63) {
            owner=5;entry=0xc1643a;
            const uint16_t statuses[]={1,0xffff,0,0,0,0,0,0,2,3};
            wr_u16(MENU_TABLE_STATUS,test<73?statuses[test-63]:0);
            wr_u16(MENU_FILE_READY,test==65?0:1);
            if(test==67) memcpy(m->slow+0x8028,"df0:absent",11);
            if(test>=73) {
                owner=6;entry=0xc162e4;wr_u16(MENU_FILE_READY,0);
                if(test==74) memcpy(m->slow+0x801d,"df0:absent",11);
            }
        }
        memcpy(before,m,sizeof *m);
        if(!file_oracle_reset(game)) return 1;
        if(!disk_tag) disk_tag=amiga_be32(game->disk.image);
        const uint32_t tag=(test==69 || test==75)?0xffffffffu:(test==70 || test==76)?0x42414400u:disk_tag;
        amiga_store_be32(game->disk.image,tag);
        if(test==68) {game->files.read_only_prefixes=protected_paths;game->files.read_only_prefix_count=1;}
        if(!source_result(entry)) return 1;
        uint8_t expected_log[78];if(!file_oracle_read_log(expected_log)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(!file_oracle_reset(game)) return 1;
        if(test==68) {game->files.read_only_prefixes=protected_paths;game->files.read_only_prefix_count=1;}
        game->screen=NATIVE_SCENE_SETUP;game->record_updates=0;
        if(owner==5) native_frontend_save_log(game);
        else if(owner==6) native_frontend_refresh_log(game);
        else stage(game,entry);
        uint8_t native_log[78];if(!file_oracle_read_log(native_log) || memcmp(native_log,expected_log,78)) {
            fprintf(stderr,"result case %u persisted bytes differ\n",test);return 1;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"result case %u parent %06X address %06X: source %02X native %02X\n",
                        test,entry,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
        if(owner==4 && (game->screen!=NATIVE_MENU || game->record_updates!=1)) return 1;
    }
    puts("77 result/restart/config parents match original RAM and persisted bytes; complete C0EF08/C162E4/C1631C/C16386/C1643A owners execute");
    file_oracle_close(game);free(game);free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
