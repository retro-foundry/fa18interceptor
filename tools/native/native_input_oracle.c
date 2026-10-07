/* Real C0F3C4 game instructions versus native input composition. Only OS
 * descriptor acquisition/release and physical mouse samples are supplied by
 * identical host contracts. All original command bodies execute normally. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "globals.h"
#include "../../port/game/native/menu.c"
#include "../../port/game/native/input.c"
extern int64_t fa18_next_event;

/* Top-level menu composition is outside this input-only comparison. */
void native_frontend_clear_text(void) {abort();}
void native_frontend_start_menu(NativeFrontend *game) {(void)game;abort();}
void native_frontend_enlist(NativeFrontend *game) {(void)game;abort();}
void native_frontend_save_log(NativeFrontend *game) {(void)game;abort();}
static uint8_t *read_bytes(const char *path,size_t *size) {
    FILE *f=fopen(path,"rb");long n;uint8_t *p;
    if(!f || fseek(f,0,SEEK_END) || (n=ftell(f))<0 || fseek(f,0,SEEK_SET)) return NULL;
    p=malloc((size_t)n);if(!p || fread(p,1,(size_t)n,f)!=(size_t)n || fclose(f)) return NULL;
    *size=(size_t)n;return p;
}
static int original_input(NativeFrontend *game) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    /* Keyboard countermeasure selection must replace the inherited low byte,
     * even when the caller's word/high halves are unrelated. */
    REG_D[4]=0x51ab12e7u;
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc0f3c4;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<1000000;++steps) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc53c08) {
            gaddr handle=rd_u32(REG_A[7]+4);
            if(handle==0x6400) REG_D[0]=0;
            else if(handle==0x6500) REG_D[0]=event_child(game,INPUT_KEYBOARD_READ);
            else {fprintf(stderr,"unexpected input handle %06X\n",handle);return 0;}
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53c8c || REG_PC==0xc1715c) {
            if(REG_PC==0xc1715c) REG_D[0]=game->mouse_buttons;
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source input failed at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=read_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=read_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc>=2?read_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    NativeFrontend *game=calloc(1,sizeof *game),*source=malloc(sizeof *source);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected || !game || !source) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    if(argc==4) {
        size_t na=0;uint8_t *actual=read_bytes(argv[2],&na);unsigned differences=0;
        if(!actual || na!=0x100000) return 1;
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        game->input_keys[0]=(uint8_t)strtoul(argv[3],NULL,0);game->input_count=1;m->joy1dat=0;
        if(!original_input(game)) return 1;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t original=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual[i]!=original) {
                if(differences<10) fprintf(stderr,"actual input %06X source %02X native %02X\n",
                    i<0x80000?i:0xc00000+i-0x80000,original,actual[i]);
                ++differences;
            }
        }
        printf("Actual native input parent: %u compared RAM differences\n",differences);
        return differences!=0;
    }
    unsigned total_events=0,drain_cases=0;
    unsigned countermeasure_cases=0,fd_cases=0;
    for(unsigned variant=0;variant<848;++variant) {
        static const uint8_t keys[]={0x0c,0x8c,0x4c,0xcc,0x4d,0xcd,0x4e,0xce,0x4f,0xcf,
            0x40,0xc0,0x24,0x20,0x13,0x44,0x37,0x38,0x39,0xb8,0xb9,0x50,0x55,0x59};
        static const uint16_t joy[]={0,1,2,0x100,0x200,0x301,0x102,0x303};
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        memset(game,0,sizeof *game);game->mouse_buttons=(variant>>4)&3;
        game->joystick_directions=joy[(variant>>2)&7];m->joy1dat=game->joystick_directions;
        wr_u32(EXTERNAL_INPUT_HANDLE,0x6400);wr_u32(KEYBOARD_INPUT_HANDLE,0x6500);
        wr_u16(RAW_KEY_LATCH,0);wr_u8(KEY_TAKEN,0);wr_u8(KEY_COUNT,0);
        wr_u8(KEY_WRITE,0);wr_u8(KEY_TRANSLATED_WRITE,0);wr_u8(KEY_STATE,0);
        wr_u8(COMMAND_EVENT_COUNTER,1);wr_u8(COMMAND_MODE_GATE,1);wr_u8(COMMAND_ENABLE_GATE,0);
        wr_u8(COMMAND_RETURN_STATE,0);wr_u8(CONTEXT_GATE,0);wr_u8(ORIGIN_ENABLE,0);
        wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(CONTEXT_SELECT,0);wr_u8(COMMAND_BLOCK_FLAGS,0);
        wr_u8(MODE_SELECT,1);wr_u8(RECORDER_MODE,0);wr_u8(PAUSE_A,(variant>>5)&1);
        wr_u8(CONTEXT_STARTED,(variant>>6)&1);wr_u8(STICK_Y_HELD,variant&1);wr_u8(STICK_X_HELD,variant&2);
        wr_u16(RECORD_WORD_A,0);wr_u16(RECORD_WORD_B,0);
        wr_u16(PENDING_COMMAND_WORD_A,0x8000);wr_u16(PENDING_COMMAND_WORD_B,0x10);
        if(variant<96) {
            game->input_keys[0]=keys[variant%24];game->input_count=1;
            /* Ordinary view-mode keys, empty source, filtered key and raw latch. */
            if(variant>=72) game->input_keys[0]=(uint8_t[]){0x3e,0x1e,0x2d,0x2f,0x3d,0x1d,0x3f,0x70}[variant&7];
            if(variant%24==16) game->input_keys[0]=0x0b; /* map voices still unconnected */
            if(variant&8) {game->input_keys[1]=0x8c;game->input_count=2;}
            if(variant==95) {wr_u16(RAW_KEY_LATCH,1);wr_u16(RAW_KEY_WORD,0x4d);}
        } else if(variant<144) {
            ++drain_cases;
            wr_u8(RECORDER_MODE,(uint8_t)(1+(variant%3)));
            wr_u16(RECORD_WORD_A,0x4400); /* Space and radar words */
            wr_u16(RECORD_WORD_B,0x10); /* zero view; mode 3 drains both */
        } else if(variant<688) {
            ++countermeasure_cases;
            const unsigned measure=(variant-144)/256;
            game->input_keys[0]=measure==1?0x33:0x23;game->input_count=1;
            wr_u8(MISSION_LEVEL_A,(uint8_t)(variant-144));
            wr_u8(MISSION_LEVEL_B,(uint8_t)(variant-144));
            if(variant>=656) {
                wr_u8(KEY_STATE,1);wr_u8(MODE_SELECT,6);
                wr_u16(COMMAND_SPAWN_GATE,(variant&1)?1:0);
                wr_u8(CONTROL_RECORDS+0x201,(variant&2)?0x40:0);
                wr_u8(CONTROL_RECORDS+0x401,(variant&4)?0x40:0);
                wr_u8(CONTROL_RECORDS+0x601,(variant&8)?0x40:0);
                wr_u8(SOUND_FLAGS-1,(uint8_t)(variant&16?1:0));
            }
        } else {
            ++fd_cases;
            const unsigned settings=(variant-688)/10;
            game->input_keys[0]=(uint8_t)(0x50+(variant-688)%10);game->input_count=1;
            wr_u8(RECORDER_MODE,0xfd);wr_u8(MODE_SELECT,(settings&1)?1:0);
            wr_u8(KEY_STATE+1,(settings&2)?1:0);wr_u8(KEY_STATE,(settings&4)?1:0);
            wr_u8(KEY_TAKEN,(settings&8)?1:0);
        }
        memcpy(source,game,sizeof *source);memcpy(before,m,sizeof *m);
        if(!original_input(source)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);native_input_process(game);
        if(game->input_count!=source->input_count || game->input_events!=source->input_events) return 1;
        total_events+=game->input_events;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"input case %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    printf("848 native pending-input cases match original game non-stack RAM (%u keyboard events, %u recorder drain, %u countermeasure cases, %u recorder FD cases)\n",total_events,drain_cases,countermeasure_cases,fd_cases);
    free(source);free(game);free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
