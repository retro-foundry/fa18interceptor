/* Reuse the established source-prefix runner and fixture definitions.
 * Expected behavior still comes from unmodified original CPU instructions;
 * the ordinary-state native component is a separate implementation. */
#define main original_glue_selection_validation_main
#include "command_selection_oracle.c"
#undef main
#include "../../port/command_input.c"
#include "../../port/indexed_controls.c"
#include "../../build/recomp/native_command_input_source.h"

#define SELECT_FIELDS(X) \
 X(event_counter,COMMAND_EVENT_COUNTER) X(origin_mode,ORIGIN_ENABLE) \
 X(modifier,KEY_STATE) X(other_modifier,KEY_STATE+2) \
 X(return_state,COMMAND_RETURN_STATE) X(block_flags,COMMAND_BLOCK_FLAGS) \
 X(message_state,MESSAGE_STATE_C) X(indexed.mode,MODE_SELECT) \
 X(indexed.mode_gate,COMMAND_MODE_GATE) X(indexed.enable_gate,COMMAND_ENABLE_GATE) \
 X(indexed.recorder_mode,RECORDER_MODE) X(indexed.origin_detail,ORIGIN_DETAIL_MODE) \
 X(indexed.function_modifier,KEY_STATE+1) X(indexed.pose_inhibit,CONTEXT_GATE)

static FA18CommandInput native_load(void) {
    FA18CommandInput s;
    memset(&s,0,sizeof s);
#define FIELD(name,address) s.name=rd_u8(address);
    SELECT_FIELDS(FIELD)
#undef FIELD
    s.pending_a=rd_u16(RECORD_WORD_A); s.pending_b=rd_u16(RECORD_WORD_B);
    return s;
}
static void native_store(const FA18CommandInput *s) {
#define FIELD(name,address) wr_u8(address,s->name);
    SELECT_FIELDS(FIELD)
#undef FIELD
    wr_u16(RECORD_WORD_A,s->pending_a); wr_u16(RECORD_WORD_B,s->pending_b);
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    char error[256]; unsigned row,byte,scenario;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):16384;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !expected || !cases ||
       !(selected_entry==0xc1ad74u || selected_entry==0xc1ac28u)) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"native command input: %s\n",error); return 1;
    }
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
        for(byte=0;byte<source_bytes[row].length;++byte)
            if(rd_u8(source_bytes[row].pc+byte)!=source_bytes[row].bytes[byte]) {
                fprintf(stderr,"original input bytes differ at %06X\n",source_bytes[row].pc+byte); return 1;
            }
    for(scenario=0;scenario<cases;++scenario) {
        FA18CommandInput s;
        CommandRequest request;
        uint32_t raw;
        memcpy(m,base,sizeof *m); fixture(scenario);
        raw=rd_u32(REG_A[7]+4); s=native_load();
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        if(selected_entry==0xc1ad74u) {
            if(!fa18_select_keyboard_command(&s,raw,&request)) return 1;
            wr_u32(REG_A[7]-4,REG_A[6]); /* original temporary LINK frame */
        } else if(!fa18_select_pending_command(&s,&request)) return 1;
        native_store(&s);
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET ||
           REG_PC!=glue_command_action_pc(request.action) ||
           (request.action!=COMMAND_INVALID_WORD && REG_D[0]!=request.raw_event) ||
           (request.action==COMMAND_INDEXED && (uint16_t)REG_D[4]!=(uint16_t)request.index) ||
           memcmp(m->chip,expected,FA18_CHIP_SIZE) ||
           memcmp(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE)) {
            unsigned offset;
            fprintf(stderr,"native command input: %06X case %u native action %u PC %06X event %08X/%08X\n",
                    selected_entry,scenario,request.action,REG_PC,REG_D[0],request.raw_event);
            for(offset=0;offset<FA18_SLOW_SIZE;++offset) if(m->slow[offset]!=expected[FA18_CHIP_SIZE+offset]) {
                fprintf(stderr,"RAM %06X source %02X native %02X\n",FA18_SLOW_BASE+offset,m->slow[offset],expected[FA18_CHIP_SIZE+offset]); break;
            }
            return 1;
        }
        /* D5/D6 and D3 are metadata only after the prefix reads them. */
        if(request.action!=COMMAND_INVALID_WORD && request.action!=COMMAND_COUNTER_WAIT &&
           (request.origin_mode!=(uint8_t)REG_D[5] || request.modifier!=(uint8_t)REG_D[6] ||
            (selected_entry==0xc1ad74u && request.detail_state!=(uint8_t)REG_D[3]))) {
            fprintf(stderr,"native command input: request metadata differs in %06X case %u\n",selected_entry,scenario); return 1;
        }
    }
    printf("native command input %06X: %u prefixes matched original actions, event/index/metadata and full Chip/Slow RAM\n",selected_entry,cases);
    printf("visited:"); for(row=0;row<sizeof visited;++row) if(visited[row]) printf(" %06X",0xc1ac28u+row); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
