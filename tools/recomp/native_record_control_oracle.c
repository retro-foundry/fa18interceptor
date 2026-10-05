#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_control_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/native_record_control.c"

enum { DIRECTORY=0xc2366a, MESSAGE_ROWS=0xc23622, COMMAND_REF=0xc61000, COMMAND_BYTES=0xc62000 };
static unsigned control_case,source_children[3],native_children[3];
static uint16_t expected_messages[64];
static unsigned source_messages,native_messages;
static int tone(void *context,FA18NativeRecordControl *s,unsigned program) {
    (void)context; (void)s; if(program!=8) return 0; ++native_children[0]; return 1;
}
static int post(void *context,FA18NativeRecordControl *s,uint16_t code) {
    (void)context; (void)s;
    if(native_messages>=source_messages || code!=expected_messages[native_messages++]) return 0;
    ++native_children[1]; return 1;
}
static int scene(void *context,FA18NativeRecordControl *s,unsigned slot,uint32_t *axis) {
    (void)context; ++native_children[2];
    s->records->records[slot].word_6c=(uint16_t)control_case;
    *axis=0x12340000u|control_case; return 1;
}
static int original_control(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc3316e || pc==0xc25704 || pc==0xc28722) {
            uint32_t ret=rd_u32(REG_A[7]);
            if(pc==0xc3316e) { if(REG_D[0]!=8 || ret!=0xc2325c) return 0; ++source_children[0]; }
            if(pc==0xc25704) {
                if(ret!=0xc232bc && ret!=0xc234e6 && ret!=0xc23538 && ret!=0xc23620) return 0;
                if(source_messages>=64) return 0;
                expected_messages[source_messages++]=(uint16_t)REG_D[0]; ++source_children[1];
                SET_W(REG_D[0],(uint16_t)REG_D[0]&0xff00u);
            }
            if(pc==0xc28722) {
                if(ret!=0xc23414 && ret!=0xc23506) return 0;
                ++source_children[2]; wr_u16(REG_A[1]+0x6c,(uint16_t)control_case);
                REG_D[4]=0x12340000u|control_case;
            }
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected control PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
static void control_fixture(unsigned slot,unsigned companion) {
    unsigned i; uint32_t r=CONTROL_RECORDS+slot*512,t=CONTROL_RECORDS+companion*512;
    static const uint8_t indices[]={0,1,3,6,7,8,0xff,0xfe};
    static const uint16_t positions[]={0,1,19,20,21,508,509,0xffff};
    wr_u8(0xc45888,control_case%3==0); wr_u16(0xc45946,(uint16_t)random_value());
    wr_u16(r+0x56,(uint16_t)random_value());
    wr_u8(MODE_SELECT,control_case%3==0?125:control_case%3==1?2:5);
    wr_u8(POST_INPUT_EVENT,control_case%11==0); wr_u8(0xc45793,control_case%5!=0);
    wr_u8(0xc4579c,control_case%7==0?0xff:0); wr_u8(0xc45799,indices[(control_case/3)%8]);
    wr_u8(0xc4579a,(control_case/32)%3==0?0xff:(control_case/32)%3==1?0xfe:0);
    wr_u16(0xc4fda2,positions[(control_case/24)%8]); wr_u16(STREAM_MODE,(uint16_t)slot);
    wr_u16(0xc458dc,control_case%2?slot:(slot+1)%16); wr_u8(0xc45785,control_case%2);
    wr_u16(r+2,(control_case/7)%4==0?0:(control_case/7)%4==1?0x100:(control_case/7)%4==2?0x800:0x900);
    wr_u16(CONTROL_RECORDS+2,rd_u16(CONTROL_RECORDS+2)|(control_case%13==0?0x800:0));
    wr_u8(r+5,(control_case/8)%2==0?0:3); wr_u16(CONTROL_RECORDS+0x6e,control_case%5);
    for(i=0;i<9;++i) wr_u16(t+0x92+2*i,(uint16_t)random_value());
    wr_u32(t+0x14,random_value()); wr_u32(t+0x18,random_value()); wr_u32(t+0x1c,random_value());
    for(i=0;i<18;++i) {
        unsigned j;
        int index=(int)i-2; uint32_t row=DIRECTORY+(uint32_t)(int32_t)(index*8);
        /* One real command stream per round ensures end/next loops terminate. */
        uint32_t tag=control_case%5==0?0:index==1 || control_case%5==1?COMMAND_REF+4*i:0x80000003;
        wr_u32(row,(control_case/16)%2==0?0x103u+(uint32_t)i:0x8003u+(uint32_t)i); wr_u32(row+4,tag);
        wr_u32(COMMAND_REF+4*i,COMMAND_BYTES+1024*i-2);
        for(j=0;j<640;++j) wr_u8(COMMAND_BYTES+1024*i-64+j,(uint8_t)(j%128));
        if(control_case%17==0 && index==(int8_t)rd_u8(0xc45799))
            wr_u8(COMMAND_BYTES+1024*i+(int16_t)rd_u16(0xc4fda2),0xff);
    }
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE); SceneState *native=malloc(sizeof *native);
    uint8_t messages[1024],metadata[1024],commands[18][640]; FA18NativeControlStream streams[18];
    FA18NativeRecordControlAssets assets; FA18NativeRecordControlOps ops={tone,post,scene,NULL};
    FA18NativeRecordControl control; FA18NativeRecordViewWork work;
    uint16_t bias,position,slot_word,viewed; uint8_t alert,mode,index,pending,gate,event,enable,origin,phase,selector,stream_view,redraw;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,i,j,entry_index;
    const uint32_t entries[]={0xc23228,0xc233aa,0xc23578};
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j)
            if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    for(entry_index=0;entry_index<3;++entry_index) for(control_case=0;control_case<cases;++control_case) {
        unsigned slot=(control_case/64)%16,companion=control_case%29==0?slot:(slot+1)%16; uint32_t carried;
        selected_entry=entries[entry_index]; memcpy(m,base,sizeof *m); scene_fixture(control_case); control_fixture(slot,companion);
        /* Fixtures must never replace instructions in the source authority. */
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        REG_A[1]=CONTROL_RECORDS+slot*512; REG_A[2]=CONTROL_RECORDS+companion*512;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        for(i=0;i<1024;++i) { messages[i]=rd_u8(MESSAGE_ROWS-512+i); metadata[i]=rd_u8(DIRECTORY-512+i); }
        for(i=0;i<18;++i) {
            for(j=0;j<640;++j) commands[i][j]=rd_u8(COMMAND_BYTES+1024*i-64+j);
            uint32_t tag=rd_u32(DIRECTORY+(uint32_t)(int32_t)(((int)i-2)*8)+4);
            streams[i]=(FA18NativeControlStream){.kind=!tag?FA18_CONTROL_STREAM_EMPTY:(int32_t)tag<0?FA18_CONTROL_STREAM_CODE:FA18_CONTROL_STREAM_COMMANDS,
                .code=(uint8_t)tag,.commands={.bytes=commands[i],.byte_count=640,.origin=64}};
        }
        assets=(FA18NativeRecordControlAssets){.streams=streams,.count=18,.first_index=-2,
            .messages={.bytes=messages,.byte_count=1024,.origin=512},.metadata={.bytes=metadata,.byte_count=1024,.origin=512}};
        bias=rd_u16(0xc45946); position=rd_u16(0xc4fda2); slot_word=rd_u16(STREAM_MODE); viewed=rd_u16(0xc458dc);
        alert=rd_u8(0xc45888); mode=rd_u8(MODE_SELECT); index=rd_u8(0xc45799); pending=rd_u8(0xc4579a);
        gate=rd_u8(0xc4579c); event=rd_u8(POST_INPUT_EVENT); enable=rd_u8(0xc45793); origin=rd_u8(0xc45785);
        phase=rd_u8(0xc457a6); selector=rd_u8(0xc458b2); redraw=rd_u8(0xc45858);
        stream_view=rd_u8(0xc458b0);
        work=(FA18NativeRecordViewWork){native->bank.records,REG_D[4]};
        control=(FA18NativeRecordControl){.records=&native->bank,.assets=&assets,.ops=&ops,.view_work=&work,
            .magnitude_bias=&bias,.stream_position=&position,.current_slot=&slot_word,.target_slot=&viewed,
            .magnitude_alert=&alert,.mode=&mode,.stream_index=&index,.stream_pending=&pending,.stream_gate=&gate,
            .post_input_event=&event,.playback_enable=&enable,.origin_enable=&origin,.sequence_phase=&phase,
            .view_selector=&selector,.stream_view_state=&stream_view,.scene_redraw=&redraw};
        memcpy(before,m,sizeof *m);
        source_messages=native_messages=0;
        if(!original_control()) { fprintf(stderr,"source control stopped entry %06X case %u\n",selected_entry,control_case); return 1; }
        carried=REG_D[4]; memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!(entry_index==0?fa18_update_native_record_control(&control,slot,companion):
             entry_index==1?fa18_update_native_record_stream(&control,slot):fa18_next_native_record_stream(&control,slot))) {
            fprintf(stderr,"native control failed entry %06X case %u\n",selected_entry,control_case); return 1;
        }
        if(work.carried_axis!=carried) { fprintf(stderr,"control carried axis differs %06X case %u\n",selected_entry,control_case); return 1; }
        if(source_messages!=native_messages) return 1;
        store_scene(native); wr_u8(0xc45888,alert); wr_u8(0xc45799,index); wr_u8(0xc4579a,pending);
        wr_u16(0xc4fda2,position); wr_u8(0xc45785,origin); wr_u8(0xc457a6,phase); wr_u8(0xc458b2,selector); wr_u8(0xc45858,redraw);
        wr_u8(0xc458b0,stream_view);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"control %06X case %u byte %06X source %02X native %02X\n",selected_entry,control_case,a,expected[i],rd_u8(a)); return 1;
            }
        }
        if(!verify_record_owners(native)) return 1;
    }
    for(i=0;i<3;++i) if(source_children[i]!=native_children[i]) return 1;
    printf("native record control: %u complete calls match all game RAM and carried axis; tone/message/scene contracts %u/%u/%u\n",cases*3,source_children[0],source_children[1],source_children[2]);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
