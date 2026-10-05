/* CPU/ROM/captured state exist only in this original-byte validation oracle. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_postflight_publication_source.h"
#define FA18_PUBLICATION_HELPERS_ONLY
#include "native_context_publication_oracle.c"
#include "../../port/native_record_selection.c"
#include "../../port/native_postflight.c"

static const uint32_t byte_addresses[]={0xc457ba,0xc457bb,0xc458cd,0xc45790,
    0xc458a6,0xc45798,0xc4586f,0xc4582a,0xc4582c,0xc458ad,0xc457ae,0xc457be,
    0xc458b2,0xc45834,0xc45785,0xc457b5,0xc457b4,0xc457ad,0xc45833,0xc458a7,
    0xc458aa,0xc458ab,0xc45848};
static const uint32_t word_addresses[]={0xc459b4,0xc458c2,0xc45782,0xc458c6,0xc4592a,0xc458c0};
static const uint32_t entries[]={0xc09e06,0xc09e98,0xc09ec4,0xc0a002,0xc0a15c,
    0xc0a1e0,0xc0a2f0,0xc0a334,0xc0a364,0xc0a12e,0xc0a3ea};
static unsigned post_case;
static int original_post(void) {
    unsigned step,i;
    for(step=0;step<1000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected post-flight PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),table_bytes[8192];
    SceneState *native=malloc(sizeof *native);
    FA18NativePostflight post; FA18NativeRecordSelection selection;
    FA18NativeRecordViewWork work; FA18NativeContextPublication publication;
    FA18ViewSpanOffsets spans; uint16_t publication_target;
    PortFieldWindow table={.bytes=table_bytes,.byte_count=sizeof table_bytes,.origin=1024};
    PortFieldByte phase_fields[2]; uint8_t b[23],*owners[23],pair,actions[3];
    uint16_t w[6],action_pending,selection_marker; unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,i,j;
    static const uint8_t modes[]={3,4,5,6,7,9,125,8,2,0xff,0x80};
    static const uint8_t phases[]={0,0,0,0xff,1,2,0xfc};
    static const uint16_t flags[]={0,0x40,0x48,0xc0,0x8040,0x8000};
    static const uint16_t gates[]={0,1,0x7fff,0x8000,0xffff,0xfffe};
    static const uint32_t positions[]={0,0x10000,0x10001,0x18000,0x30000,0x30001,0x80000000,0x7fffffff,0xffff0000};
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(entry=0;entry<11;++entry) for(post_case=0;post_case<cases;++post_case) {
        unsigned target=1+post_case%15,slot=post_case%16; uint32_t r=CONTROL_RECORDS+target*512,axis;
        uint16_t event; int ready=0,expected_ready,ok;
        memcpy(m,base,sizeof *m); selected_entry=entries[entry]; scene_fixture(post_case);
        REG_A[1]=CONTROL_RECORDS+slot*512; event=(uint16_t)REG_D[0];
        for(i=0;i<23;++i) wr_u8(byte_addresses[i],(uint8_t)random_value());
        wr_u8(PLAYER_PHASE,phases[post_case%7]); wr_u8(MODE_SELECT,modes[post_case%11]);
        wr_u8(SCHEDULE_STATUS,post_case%7==0?0:0x40); wr_u8(SCHEDULE_BLOCKED,(uint8_t)(post_case%19==0));
        wr_u8(SEQUENCE_PHASE,(uint8_t)(post_case%5)); wr_u8(POST_INPUT_EVENT,(uint8_t)(post_case%7));
        wr_u8(0xc458a7,(uint8_t)((post_case%6)-1)); wr_u8(SCHEDULE_SAVED_VIEW,post_case%3==0?0x80:1);
        wr_u8(CONTEXT_SELECT,(uint8_t)(post_case%3==0));
        wr_u8(KEY_TAKEN,post_case&256?1:0);
        wr_u8(KEY_COUNT,(uint8_t)(post_case/128));
        wr_u8(KEY_WRITE,(uint8_t)(post_case/32));
        wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(post_case/64));
        wr_u8(REDRAW_KEEP_STATE,(uint8_t)(post_case&1));
        wr_u8(SCENE_DISPATCH_ADMITTED,(uint8_t)(post_case%4==0?0x80:post_case%4));
        wr_u8(SCENE_DISPATCH_AUX,(uint8_t)(post_case%3));
        wr_u16(SCENE_DISPATCH_GATE,gates[post_case%6]); wr_u16(SCHEDULE_TARGET,(uint16_t)(target*512));
        wr_u16(SELECTED_RECORD,post_case%3==0?0xffff:(uint16_t)((post_case%16)*512));
        for(i=0;i<16;++i) {
            uint32_t a=CONTROL_RECORDS+i*512;
            wr_u16(a,flags[(post_case/7+i)%6]); wr_u16(a+2,post_case%3==0?0xc080:post_case%3==1?0x80:0);
            wr_u16(a+6,post_case%9==0?0:post_case%9==1?0x70:post_case%9==2?0x6f:post_case%9==3?0x8000:1);
            wr_u16(a+12,post_case%13==0?0:1); wr_u16(a+0x6c,(uint16_t)(post_case%4==0?0x321:post_case%4==1?0x8000:0x320));
            wr_u16(a+0x6e,post_case%3==0?1:0); wr_u16(a+0x4c,gates[(post_case+i)%6]);
            wr_u8(a+0x20,(uint8_t)(post_case%4==0?2:post_case%4==1?0x80:0));
            wr_u8(a+0x21,(uint8_t)(post_case%2)); wr_u8(a+4,(uint8_t)(post_case%5==0?0xc0:post_case%5==1?4:0));
            wr_u8(a+0x3a,(uint8_t)(post_case+i));
            wr_u8(a+0x62,post_case&4?0x30:0x20);
            for(j=0;j<3;++j) wr_u32(a+0x14+4*j,positions[(post_case/11+i+j)%9]);
        }
        wr_u16(r,0x8000); wr_u8(0xc45848,(uint8_t)(post_case%5));
        /* Independent routes remove accidental correlations among phase,
         * activation, braking, saved-view and sequence fixture selectors. */
        if(post_case%32<=1) {
            uint32_t a=CONTROL_RECORDS+4*512;
            wr_u8(PLAYER_PHASE,0); wr_u8(MODE_SELECT,4); wr_u8(SCHEDULE_STATUS,0x40); wr_u8(SCHEDULE_BLOCKED,0);
            wr_u16(a,0x40); wr_u16(a+2,0x80); wr_u16(a+6,1); wr_u16(a+0x6c,0x320);
            wr_u16(CONTROL_RECORDS+8*512,0); wr_u16(CONTROL_RECORDS+10*512,0);
            wr_u8(POST_INPUT_EVENT,(uint8_t)(post_case%32+1)); wr_u8(SCHEDULE_SAVED_VIEW,0);
            wr_u8(CONTEXT_SELECT,(uint8_t)((post_case/32)&1));
        } else if(post_case%32==2) {
            wr_u8(PLAYER_PHASE,0); wr_u8(MODE_SELECT,9); wr_u8(SCHEDULE_STATUS,0x40); wr_u8(SCHEDULE_BLOCKED,0);
            wr_u16(CONTROL_RECORDS,0x40); wr_u16(CONTROL_RECORDS+2,0xc080); wr_u16(CONTROL_RECORDS+0x6e,0);
            wr_u8(SEQUENCE_PHASE,(uint8_t)((post_case/32)&1?3:2));
        }
        for(i=0;i<256;++i) wr_u16(VIEW_PARAMETER_TABLE+2*i,(uint16_t)(post_case%17==0?-128:512+10*i));
        for(i=0;i<1280;++i) wr_u16(VIEW_PARAMETER_TABLE+512+2*i,(uint16_t)random_value());
        for(i=0;i<5;++i) wr_u16(VIEW_PARAMETER_TABLE-128+2*i,(uint16_t)random_value());
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        for(i=0;i<23;++i) { b[i]=rd_u8(byte_addresses[i]); owners[i]=b+i; }
        owners[5]=&native->phase; owners[6]=native->flags+5;
        owners[2]=&native->commands.indexed.cockpit_low_byte; *owners[2]=b[2];
        owners[22]=&native->commands.indexed.pose_entry;
        for(i=0;i<6;++i) w[i]=rd_u16(word_addresses[i]);
        for(i=0;i<sizeof table_bytes;++i) table_bytes[i]=rd_u8(VIEW_PARAMETER_TABLE-1024+i);
        pair=rd_u8(PAIR_OVERRIDE); action_pending=0; memset(actions,0,sizeof actions);
        selection_marker=rd_u16(SELECTION_MARKER);
        selection=(FA18NativeRecordSelection){.records=&native->bank,.selected_record=&native->selected,
            .selection_marker=&selection_marker,.selection_active=&native->selection,.action_pending=&action_pending,
            .origin_enable=owners[14],.action_first=actions,.action_second=actions+1,.action_third=actions+2,.pair_override=&pair};
        phase_fields[0]=(PortFieldByte){.unsigned_word=w+5,.shift=8}; phase_fields[1]=(PortFieldByte){.unsigned_word=w+5};
        work=(FA18NativeRecordViewWork){.carried_axis=REG_D[4]};
        post=(FA18NativePostflight){.records=&native->bank,.selection=&selection,.parameters=&table,.view_work=&work,
            .ops=NULL,.phase_fields=phase_fields,.current_slot=w,.target_record=&native->flight.spawn_gate,
            .dispatch_gate=w+2,.command_word=w+3,.view_heading=w+4,.space_latch=owners[0],.report_latch=owners[1],
            .status=owners[2],.blocked=owners[3],.mode=owners[4],.player_phase=owners[5],.player_flags_f=owners[6],
            .sequence_phase=owners[7],.sequence_step=owners[8],.context_gate=owners[9],.post_input_event=owners[10],
            .saved_view=owners[11],.view_side=owners[12],.saved_context=owners[13],.context_select=owners[14],
            .context_smooth=owners[15],.context_started=owners[16],.context_clear=owners[17],.refresh=owners[18],
            .limit=owners[19],.admitted=owners[20],.aux=owners[21],.ready_mode=owners[22]};
        load_publication(native);
        for(i=0;i<256;++i) spans.values[i]=rd_s8(0xc1bad4u-128+i);
        publication_target=rd_u16(TARGET_RECORD);
        publication=(FA18NativeContextPublication){&native->bank,&native->context,&native->queue,&spans,&selection_marker,&publication_target};
        post.publication=&publication;
        owners[0]=&native->flight.space_command_latch;
        owners[3]=&native->context.recorder_on;
        owners[7]=&native->flight.sequence_phase;
        owners[10]=&native->commands.indexed.origin_gate_a;
        owners[12]=&native->view.detail_index;
        owners[14]=&native->commands.origin_mode;
        owners[15]=&native->commands.indexed.origin_gate_b;
        owners[16]=&native->flight.context_started;
        owners[17]=&native->flight.pause;
        owners[18]=&native->context.view_request;
        for(i=0;i<23;++i) {
            if(byte_addresses[i]>=KEY_RAW-128 && byte_addresses[i]<KEY_RAW-128+FA18_COMMAND_QUEUE_NEIGHBORS &&
               !fa18_bind_command_queue_byte(&native->queue,byte_addresses[i]-(KEY_RAW-128),owners[i])) return 1;
        }
        native->queue.slots[0x21]=(PortFieldByte){.unsigned_word=w+2,.shift=8};
        native->queue.slots[0x22]=(PortFieldByte){.unsigned_word=w+2};
        post.space_latch=owners[0]; post.blocked=owners[3]; post.sequence_phase=owners[7];
        post.post_input_event=owners[10]; post.context_select=owners[14]; post.context_smooth=owners[15];
        post.context_started=owners[16]; post.context_clear=owners[17]; post.refresh=owners[18];
        selection.origin_enable=owners[14]; post.view_heading=&native->context.angle_history;
        post.view_side=owners[12];
        memcpy(before,m,sizeof *m);
        if(!original_post()) { fprintf(stderr,"source post-flight failed %06X case %u\n",selected_entry,post_case); return 1; }
        expected_ready=!(uint16_t)REG_D[0]; axis=REG_D[4];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        ok=entry<9?fa18_schedule_native_postflight(&post,(FA18NativePostflightMode)entry,event):
            entry==9?fa18_restore_native_postflight_view(&post,slot):fa18_native_postflight_ready(&post,&ready);
        if(!ok || axis!=work.carried_axis || (entry==10 && ready!=expected_ready)) {
            fprintf(stderr,"post-flight work mismatch %06X case %u axis %08X/%08X ready %d/%d\n",selected_entry,post_case,axis,work.carried_axis,expected_ready,ready); return 1;
        }
        store_publication(native); wr_u16(TARGET_RECORD,publication_target); wr_u8(PAIR_OVERRIDE,pair); wr_u16(SELECTION_MARKER,selection_marker);
        for(i=0;i<23;++i) wr_u8(byte_addresses[i],*owners[i]);
        for(i=0;i<6;++i) wr_u16(word_addresses[i],i==1?native->flight.spawn_gate:i==4?native->context.angle_history:w[i]);
        /* Bulk compare the identical RAM regions first. Detailed byte reads
         * are needed only to report a mismatch; the ABI-stack exception is
         * exactly the same C7FD00..C7FF00 interval as the exhaustive loop. */
        if(memcmp(m->chip,expected,FA18_CHIP_SIZE) ||
           memcmp(m->slow,expected+FA18_CHIP_SIZE,0x7fd00) ||
           memcmp(m->slow+0x7ff00,expected+FA18_CHIP_SIZE+0x7ff00,FA18_SLOW_SIZE-0x7ff00)) {
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"post-flight %06X case %u byte %06X source %02X native %02X\n",selected_entry,post_case,a,expected[i],rd_u8(a)); return 1;
            }
        }
        }
        memcpy(m->chip,expected,FA18_CHIP_SIZE); memcpy(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
#define CHECK_VIEW(n,a) if(native->view.n!=rd_u8(a)) return 1;
        VIEW_BYTES(CHECK_VIEW)
#undef CHECK_VIEW
        if(!verify_record_owners(native) || publication_target!=rd_u16(TARGET_RECORD) ||
           native->flight.viewed!=native->bank.aircraft+rd_u16(VIEW_RECORD)/512) return 1;
    }
    printf("native post-flight/publication: %u complete calls match all game RAM, typed records and carried axis; actual view publication, zoom, redraw, queue, selection release/readiness/restoration; no child contracts\n",cases*11);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
