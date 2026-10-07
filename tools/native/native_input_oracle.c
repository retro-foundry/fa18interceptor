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
#include "../../port/game/native/viewport.c"
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
static uint32_t initial_input_carry=0x51ab12e7u;
static int original_input_entry(NativeFrontend *game,gaddr entry,uint32_t raw) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    if(entry==0xc1ad74) wr_u32(REG_A[7]+4,raw);
    /* Keyboard countermeasure selection must replace the inherited low byte,
     * even when the caller's word/high halves are unrelated. */
    REG_D[4]=initial_input_carry;
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=entry;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<1000000;++steps) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc53c08) {
            gaddr handle=rd_u32(REG_A[7]+4);
            gaddr caller=rd_u32(REG_A[7]);
            if(caller==0xc16ebe && handle==rd_u32(EXTERNAL_INPUT_HANDLE)) REG_D[0]=0;
            else if(caller==0xc16c02 && handle==rd_u32(KEYBOARD_INPUT_HANDLE))
                REG_D[0]=event_child(game,INPUT_KEYBOARD_READ);
            else {fprintf(stderr,"unexpected input handle %06X\n",handle);return 0;}
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53c8c || REG_PC==0xc1715c) {
            if(REG_PC==0xc1715c) REG_D[0]=game->mouse_buttons;
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53b00 || REG_PC==0xc53b18) {
            /* Only Exec Add/RemIntServer is a host boundary. C06BF0 and both
             * game-side callback owners execute all their original stores. */
            if(rd_u32(REG_A[7]+4)!=5 || rd_u32(REG_A[7]+8)!=0xc1abf0) return 0;
            game->input_server_installed=REG_PC==0xc53b00;
            REG_D[0]=0;REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source input failed at %06X\n",REG_PC);return 0;
}
static int original_input(NativeFrontend *game) {return original_input_entry(game,0xc0f3c4,0);}
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
    if(argc==4 || argc==5) {
        /* Optional independent preceding-body output. Native never reads it. */
        if(argc==5) initial_input_carry=(uint32_t)strtoul(argv[4],NULL,0);
        size_t na=0;uint8_t *actual=read_bytes(argv[2],&na);unsigned differences=0;
        if(!actual || na!=0x100000) return 1;
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        if(strcmp(argv[3],"pending")) {
            game->input_keys[0]=(uint8_t)strtoul(argv[3],NULL,0);game->input_count=1;
        }
        m->joy1dat=0;
        if(!original_input(game)) return 1;
        const char *expected_carry=getenv("FA18_INPUT_EXPECT_CARRY");
        if(expected_carry && (uint8_t)REG_D[4]!=(uint8_t)strtoul(expected_carry,NULL,0)) {
            fprintf(stderr,"Input return: source %02X native %02X\n",(uint8_t)REG_D[4],
                (uint8_t)strtoul(expected_carry,NULL,0));return 1;
        }
        printf("Input return byte: %u\n",(uint8_t)REG_D[4]);
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
    unsigned countermeasure_cases=0,fd_cases=0,ejection_cases=0,callback_cases=0,pending_countermeasures=0;
    for(unsigned variant=0;variant<2512;++variant) {
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
        } else if(variant<848) {
            ++fd_cases;
            const unsigned settings=(variant-688)/10;
            game->input_keys[0]=(uint8_t)(0x50+(variant-688)%10);game->input_count=1;
            wr_u8(RECORDER_MODE,0xfd);wr_u8(MODE_SELECT,(settings&1)?1:0);
            wr_u8(KEY_STATE+1,(settings&2)?1:0);wr_u8(KEY_STATE,(settings&4)?1:0);
            wr_u8(KEY_TAKEN,(settings&8)?1:0);
        } else if(variant<912) {
            ++ejection_cases;
            const unsigned settings=variant-848;
            game->input_keys[0]=0x12;game->input_count=1;
            wr_u8(KEY_STATE,(uint8_t[]){0,1,2,0xff}[settings&3]);
            wr_u8(ORIGIN_GATE_A,(settings&4)?1:0);
            wr_u8(BAR_E_FLAG,(uint8_t[]){0,1,0x80,0xff}[(settings>>3)&3]);
            wr_u8(KEY_TAKEN,(settings&32)?1:0);
        } else if(variant<976) {
            ++callback_cases;
            const unsigned settings=variant-912;
            game->input_server_installed=settings&1;
            native_input_enqueue(game,127,!(settings&2));
            wr_u8(RECORDER_MODE,(settings&4)?0xfd:0);
            wr_u8(KEY_STATE,(settings&8)?1:0);
            wr_u8(ORIGIN_DETAIL_MODE,(settings&16)?3:0);
            wr_u8(CONTEXT_GATE,(settings&32)?2:0);
            /* Installation must restore these descriptor fields only. */
            wr_u32(0xc1abf8,0xaabbccdd);wr_u32(0xc1abfc,0x12345678);
            wr_u32(0xc1ac02,0x87654321);
        } else {
            ++pending_countermeasures;
            const unsigned settings=variant-976,kind=settings/256;
            wr_u8(RECORDER_MODE,(uint8_t)(1+kind/2));
            wr_u16(RECORD_WORD_A,(kind&1)?4:2);
            wr_u16(RECORD_WORD_B,0);
            wr_u8(MISSION_LEVEL_A,(uint8_t)settings);
            wr_u8(MISSION_LEVEL_B,(uint8_t)settings);
            /* An already claimed queue makes the incoming D4 byte dead.
             * original_input deliberately supplies a nonzero release byte. */
            wr_u8(KEY_TAKEN,1);
        }
        memcpy(source,game,sizeof *source);memcpy(before,m,sizeof *m);
        if(!original_input(source)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);native_input_process(game);
        if(game->input_count!=source->input_count || game->input_events!=source->input_events ||
           game->input_server_installed!=source->input_server_installed) return 1;
        total_events+=game->input_events;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"input case %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    printf("2512 native pending-input cases match original game non-stack RAM (%u keyboard events, %u recorder drain, %u countermeasure cases, %u recorder FD cases, %u ejection cases, %u callback cases, %u claimed pending countermeasures)\n",total_events,drain_cases,countermeasure_cases,fd_cases,ejection_cases,callback_cases,pending_countermeasures);
    unsigned command_returns=0,preserved=0,selected=0,queued=0,action_returns=0,view_returns=0,unresolved=0;
    for(unsigned test=0;test<4416;++test) {
        static const uint8_t commands[]={0x60,0x61,0xe0,0xe1,0x66,0xe6,0x67,0xe7,
            0x70,0xf0,0x4c,0x4e,0x0c,0x8c,0x24,0x3e};
        const uint8_t controls[]={0x38,0x39,0xb8,0xb9,0x0b,0x8b,0x15,0x45};
        const uint8_t toggles[]={0x14,0x0d,0x41,0x20,0x21,0x26,0x20};
        const int pending=(test>=832 && test<1664) || (test>=2688 && test<3968);
        const uint8_t raw=test<512?commands[test%16]:test<768?0x25:test<800?0x24:
            test<832?controls[(test-800)%8]:pending?0:test<2176?(test&1?0x1a:0x1b):
            test<2688?0x13:toggles[(test-3968)%7];
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        memset(game,0,sizeof *game);game->completed_input_return=(NativeInputReturn){0xe7,NATIVE_INPUT_RETURN_HUD_TEXT};
        wr_u8(COMMAND_EVENT_COUNTER,(uint8_t[]){1,254,128,0}[(test>>5)&3]);
        wr_u8(COMMAND_MODE_GATE,1);wr_u8(COMMAND_ENABLE_GATE,0);
        wr_u8(COMMAND_RETURN_STATE,0);wr_u8(CONTEXT_GATE,0);wr_u8(ORIGIN_ENABLE,0);
        wr_u8(ORIGIN_DETAIL_MODE,test&16?3:0);wr_u8(COMMAND_BLOCK_FLAGS,0);
        wr_u8(MODE_SELECT,1);wr_u8(RECORDER_MODE,0);
        wr_u8(KEY_STATE,0);wr_u8(KEY_STATE+1,0);wr_u8(KEY_STATE+2,0);
        wr_u8(KEY_TAKEN,!!(test&128));wr_u8(KEY_COUNT,test&256?10:0);
        wr_u8(KEY_WRITE,test&4?255:10);
        wr_u8(KEY_TRANSLATED_WRITE,(uint8_t[]){0,7,255,128}[(test>>2)&3]);
        if(test>=512) {
            wr_u8(COMMAND_EVENT_COUNTER,1);wr_u8(ORIGIN_DETAIL_MODE,0);
            wr_u8(KEY_TAKEN,test&1);wr_u8(KEY_COUNT,10);wr_u8(KEY_WRITE,0);
            if(test<768) wr_u8(POST_INPUT_EXPIRED,(uint8_t)(test-512));
            else if(test<800) {
                const unsigned gear=test-768;
                wr_u8(CONTROL_RECORDS+3,(rd_u8(CONTROL_RECORDS+3)&0x7f)|(gear&4?0x80:0));
                wr_u32(COMMAND_GEAR_GATE,(uint32_t[]){0,0x40,0xffffffff,0x12345678}[gear&3]);
                wr_u8(CONTEXT_SELECT,!!(gear&8));wr_u8(TONE_MUTE,!!(gear&16));
            } else if(test<832) {
                wr_u8(KEY_STATE,test&8?1:0);
                if(raw&0x80) wr_u8(ORIGIN_DETAIL_MODE,3);
            } else if(test<1664) {
                const unsigned view=test-832,slot=3+view%13,settings=view/13;
                wr_u8(RECORDER_MODE,3);wr_u16(RECORD_WORD_A,0);
                wr_u16(RECORD_WORD_B,(uint16_t)(1u<<(slot<8?slot+8:slot-8)));
                wr_u8(ORIGIN_ENABLE,(uint8_t[]){0,1,255,3}[settings&3]);
                wr_u8(ORIGIN_DETAIL_MODE,settings&32?1:0);
                wr_u8(ORIGIN_GATE_MODE,!!(settings&16));
                wr_u32(SELECTOR_ORIGIN_MIDDLE,0x04000000);
                wr_u16(VIEW_RECORD,0);
                wr_u8(CONTROL_RECORDS+0x62,(uint8_t[]){0x11,0x20,0x30,0xff}[(settings>>2)&3]);
                wr_u8(VIEW_MODE,(uint8_t[]){0,6,11,255}[(settings>>4)&3]);
                wr_u16(LINE_LAST_ROW,settings&4?0xb3:0x90);
                wr_u8(FIRE_STATE,settings&8?255:0);
            } else if(test<2176) {
                wr_u8(ZOOM_FLAGS,(uint8_t)((test-1664)/2));
                wr_u16(ZOOM_SCALE,(uint16_t[]){0x20,0x40,0x80,0xff00}[(test>>1)&3]);
            } else if(test<2688) {
                const unsigned radar=test-2176;
                const int16_t offset=radar&256?15*512:0;
                wr_u16(VIEW_RECORD,(uint16_t)offset);
                wr_u8(CONTROL_RECORDS+offset+0x63,(uint8_t)radar);
                wr_u8(CONTEXT_SELECT,!!(radar&1));wr_u8(TONE_MUTE,!!(radar&2));
            } else if(test<3968) {
                const unsigned weapon=test-2688,setting=weapon/256;
                wr_u16(RECORD_WORD_A,0x1000);wr_u16(RECORD_WORD_B,0);
                wr_u8(RECORDER_MODE,3);
                wr_u8(MODE_SELECT,(uint8_t[]){0,0,2,125,125}[setting]);
                wr_u8(COMMAND_WEAPON_PAUSE,setting==3?255:0);
                wr_u8(COMMAND_BLOCK_FLAGS,setting==1?(uint8_t)weapon:0);
                wr_u8(CONTROL_RECORDS+0x63,(uint8_t)weapon);
                wr_u8(CONTEXT_SELECT,!!(weapon&1));wr_u8(TONE_MUTE,!!(weapon&2));
            } else {
                const unsigned setting=(test-3968)/7;
                wr_u8(MODE_SELECT,(uint8_t[]){0,2,6,125}[setting&3]);
                wr_u8(CONTEXT_SELECT,!!(setting&4));wr_u8(TONE_MUTE,!!(setting&8));
                wr_u8(CONTROL_RECORDS+0x62,(uint8_t[]){0x11,0x20,0x30,0xff}[(setting>>4)&3]);
            }
        }
        memcpy(source,game,sizeof *source);memcpy(before,m,sizeof *m);
        if(!original_input_entry(source,pending?0xc1ac28:0xc1ad74,raw)) return 1;
        const uint8_t carry=(uint8_t)REG_D[4];
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(pending) native_menu_dispatch_pending(game);else native_menu_dispatch_raw(game,raw);
        const NativeInputReturn output=game->completed_input_return;
        if(output.owner!=NATIVE_INPUT_RETURN_UNKNOWN) {
            if(output.value!=carry) {
                fprintf(stderr,"Command return case %u raw %02X owner %u: source %02X native %02X\n",
                    test,raw,output.owner,carry,output.value);return 1;
            }
            ++command_returns;
            preserved+=output.owner==NATIVE_INPUT_RETURN_HUD_TEXT;
            selected+=output.owner==NATIVE_INPUT_RETURN_COMMAND_SELECTION;
            queued+=output.owner==NATIVE_INPUT_RETURN_COMMAND_QUEUE;
            action_returns+=output.owner==NATIVE_INPUT_RETURN_FLIGHT_ACTION;
            view_returns+=output.owner==NATIVE_INPUT_RETURN_VIEW_ACTION;
        } else ++unresolved;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"Command case %u %06X: source %02X native %02X\n",test,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    if(!preserved || !selected || !queued || !action_returns || !view_returns || unresolved) return 1;
    printf("4416 command parents match non-stack RAM; %u defined returns match (%u preserved, %u selection, %u queue, %u flight action, %u view action; %u action-owned returns remain unresolved)\n",
        command_returns,preserved,selected,queued,action_returns,view_returns,unresolved);
    for(unsigned test=0;test<16;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        memset(game,0,sizeof *game);
        game->screen=(test&1)?NATIVE_MISSIONS:NATIVE_MENU;
        game->input_server_installed=(test&2)!=0;
        wr_u8(COMMAND_EVENT_COUNTER,1);wr_u8(MODE_SELECT,0);
        wr_u8(ORIGIN_DETAIL_MODE,(test&4)?3:0);wr_u8(CONTEXT_GATE,(test&8)?2:0);
        wr_u32(0xc1abf8,0xaabbccdd);wr_u32(0xc1ac02,0x87654321);
        memcpy(source,game,sizeof *source);memcpy(before,m,sizeof *m);
        if(!original_input_entry(source,0xc1ad74,0x46)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);native_menu_key(game,127,1);
        if(!game->input_server_installed || game->input_server_installed!=source->input_server_installed) return 1;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"menu Delete case %u %06X: source %02X native %02X\n",test,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    puts("16 menu/mission Delete command parents match original non-stack RAM and callback installation");
    free(source);free(game);free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
