/* All 16 native view actions and both actual children against original bytes. */
#define main reference_view_action_validation_main
#include "view_commands_oracle.c"
#undef main
#include "../../port/view_command_controls.c"
#include "../../build/recomp/native_view_command_input_source.h"

#define VIEW_BYTES(X) \
 X(detail_index,ORIGIN_DETAIL_INDEX) X(update_mask,UPDATE_MASK) X(fire_state,FIRE_STATE) \
 X(mode,VIEW_MODE) X(mode_auxiliary,0xc457a8u) X(mode_companion,0xc457a9u) \
 X(refresh_request,0xc45891u) X(zoom_flags,ZOOM_FLAGS) \
 X(redraw_first,REDRAW_FIRST) X(gauge_refresh,GAUGE_REFRESH) \
 X(grid_z_redraws,GRID_Z_REDRAWS) X(grid_x_redraws,GRID_X_REDRAWS) \
 X(redraw_bar_a,BAR_REDRAWS_A) X(redraw_bar_aux,0xc45841u) \
 X(display_update,DISPLAY_UPDATE) X(redraw_keep_state,REDRAW_KEEP_STATE)
#define VIEW_WORDS(X) \
 X(target_mark,TARGET_MARK) X(span_origin,SPAN_ORIGIN) X(span_origin_y,SPAN_ORIGIN_Y) \
 X(line_last_row,LINE_LAST_ROW) X(zoom_scale,ZOOM_SCALE) \
 X(redraw_state_word,REDRAW_STATE_WORD) X(emitted_requests,PENDING_COMMAND_WORD_B)
#define VIEW_LONGS(X) X(origin_middle,SELECTOR_ORIGIN_MIDDLE) X(redraw_state_long,REDRAW_STATE_LONG)
#define FLIGHT_BYTES(X) \
 X(redraw_b,BAR_REDRAWS_B) X(redraw_c,BAR_REDRAWS_C) X(redraw_d,BAR_REDRAWS_D) \
 X(scale_redraws,SCALE_REDRAWS) X(info_redraws,INFO_REDRAWS) \
 X(weapon_mode_redraws,COMMAND_WEAPON_MODE_REDRAWS) X(weapon_redraws,WEAPON_REDRAWS) \
 X(pause,PAUSE_A)
#define COMMAND_BYTES(X) \
 X(origin_mode,ORIGIN_ENABLE) X(indexed.origin_detail,ORIGIN_DETAIL_MODE) \
 X(indexed.origin_gate_b,ORIGIN_GATE_B)

typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18FlightCommandRecord records[2];
} NativeView;
typedef struct { NativeView state; unsigned viewed; } ViewSnapshot;
typedef struct { unsigned kind; ViewSnapshot before; } ViewChild;
typedef struct { NativeView *state; unsigned count; ViewChild children[2]; } ViewTrace;
static ViewTrace *active_trace;

static void snapshot(const NativeView *state,ViewSnapshot *out) {
    memset(out,0,sizeof *out); out->state=*state;
    out->viewed=state->flight.viewed==&state->records[1];
    out->state.flight.commands=NULL; out->state.flight.viewed=NULL;
    out->state.view.flight=NULL;
}
static int child(NativeView *state,unsigned kind) {
    ViewChild *row;
    if(!active_trace || state!=active_trace->state || active_trace->count>=2) return 0;
    row=&active_trace->children[active_trace->count++]; row->kind=kind;
    snapshot(state,&row->before);
    return kind?fa18_request_native_cockpit_redraw(&state->view):fa18_set_native_zoom_maximum(&state->view);
}
static int zoom(FA18ViewCommandState *state) {
    if(!active_trace || state!=&active_trace->state->view) return 0;
    return child(active_trace->state,0);
}
static int redraw(FA18ViewCommandState *state) {
    if(!active_trace || state!=&active_trace->state->view) return 0;
    return child(active_trace->state,1);
}
#define fa18_set_native_zoom_maximum zoom
#define fa18_request_native_cockpit_redraw redraw
#include "../../port/view_command_input.c"
#undef fa18_set_native_zoom_maximum
#undef fa18_request_native_cockpit_redraw

static void load(NativeView *state) {
    unsigned i;
    memset(state,0,sizeof *state);
#define FIELD(name,address) state->view.name=rd_u8(address);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) state->view.name=rd_u16(address);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) state->view.name=rd_u32(address);
    VIEW_LONGS(FIELD)
#undef FIELD
#define FIELD(name,address) state->flight.name=rd_u8(address);
    FLIGHT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) state->commands.name=rd_u8(address);
    COMMAND_BYTES(FIELD)
#undef FIELD
    for(i=0;i<2;++i) state->records[i].equipment_kind=rd_u8(CONTROL_RECORDS+0x200*i+0x62);
    state->flight.commands=&state->commands;
    state->flight.viewed=&state->records[rd_u16(VIEW_RECORD)?1:0];
    state->view.flight=&state->flight;
}
static void store(const NativeView *state) {
#define FIELD(name,address) wr_u8(address,state->view.name);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u16(address,state->view.name);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u32(address,state->view.name);
    VIEW_LONGS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u8(address,state->flight.name);
    FLIGHT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u8(address,state->commands.name);
    COMMAND_BYTES(FIELD)
#undef FIELD
}
static const struct {uint32_t entry,ret;} edges[]={{0xc08324,0xc1b9e0},{0xc082b8,0xc1ba8c}};
static unsigned char control_visited[sizeof source_bytes/sizeof source_bytes[0]];
static int original(ViewTrace *trace,uint32_t event) {
    unsigned step,next=0,i;
    uint32_t active_return=0;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        if(active_return && pc==active_return && REG_A[7]==0xc7ff00u) active_return=0;
        if(!active_return && pc==0xc1c23c) return next==trace->count;
        if(!active_return && next<trace->count) {
            ViewChild *row=&trace->children[next];
            if(pc==edges[row->kind].entry && rd_u32(REG_A[7])==edges[row->kind].ret) {
                NativeView current; ViewSnapshot got;
                load(&current); snapshot(&current,&got);
                if(memcmp(&got,&row->before,sizeof got) || REG_D[0]!=event) {
                    fprintf(stderr,"view child %u input/state mismatch at %06X\n",row->kind,pc); return 0;
                }
                ++next; active_return=edges[row->kind].ret;
            }
        }
        if(!active_return && !(pc>=0xc1b77c && pc<0xc1bb7a)) return 0;
        if(!active_return) visited[pc-action_base]=1;
        else {
            for(i=0;i<sizeof source_bytes/sizeof source_bytes[0];++i) if(pc==source_bytes[i].pc) break;
            if(i==sizeof source_bytes/sizeof source_bytes[0]) return 0;
            control_visited[i]=1;
        }
        { uint16_t opcode=rd_u16(pc);
          REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
          m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]); }
    }
    return 0;
}

int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):2048,scenario,row,byte;
    unsigned children=0; char error[256]; FA18ViewSpanOffsets spans;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    for(selected_action=COMMAND_PENDING_EMPTY;selected_action<=COMMAND_INDEXED;++selected_action)
        if(glue_command_action_pc(selected_action)==selected_entry) break;
    if(!state || !rom || !m || !base || !before || !expected || !cases ||
       !fa18_is_view_input_command(selected_action)) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"native view commands: %s\n",error); return 1;
    }
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
        for(byte=0;byte<source_bytes[row].length;++byte)
            if(rd_u8(source_bytes[row].pc+byte)!=source_bytes[row].bytes[byte]) return 1;
    for(row=0;row<256;++row) spans.values[row]=rd_s8(0xc1bad4u-128+row);
    for(scenario=0;scenario<cases;++scenario) {
        NativeView native; ViewTrace trace; uint32_t event;
        CommandRequest request;
        memcpy(m,base,sizeof *m); fixture(scenario);
        wr_u8(REDRAW_KEEP_STATE,(scenario&1024)?1:0);
        load(&native); memset(&trace,0,sizeof trace); trace.state=&native; active_trace=&trace;
        request=(CommandRequest){selected_action,REG_D[0],(uint8_t)REG_D[5],(uint8_t)REG_D[6],0,0};
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        if(!fa18_apply_view_input_command(&native.view,&request,&spans,&event)) return 1;
        store(&native);
        for(row=0;row<trace.count;++row) wr_u32(0xc7fefcu,edges[trace.children[row].kind].ret);
        children+=trace.count;
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!original(&trace,request.raw_event) || REG_D[0]!=event || REG_A[7]!=0xc7ff00u ||
           memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE)) {
            unsigned offset;
            fprintf(stderr,"native view command %06X case %u: PC %06X event %08X/%08X SP %06X\n",
                    selected_entry,scenario,REG_PC,REG_D[0],event,REG_A[7]);
            for(offset=0;offset<FA18_SLOW_SIZE;++offset) if(m->slow[offset]!=expected[FA18_CHIP_SIZE+offset]) {
                fprintf(stderr,"RAM %06X original %02X native %02X\n",FA18_SLOW_BASE+offset,m->slow[offset],expected[FA18_CHIP_SIZE+offset]); break;
            }
            return 1;
        }
    }
    printf("native view command %06X: %u actions matched full RAM, event and ordered child states; %u real children\n",selected_entry,cases,children);
    printf("visited:"); for(row=0;row<sizeof visited;++row) if(visited[row]) printf(" %06X",action_base+row); putchar('\n');
    printf("control_visited:");
    for(row=0;row<sizeof control_visited;++row) if(control_visited[row]) printf(" %06X",source_bytes[row].pc);
    putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
