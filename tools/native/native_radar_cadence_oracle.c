/* External oracle: complete C31226 radar owner on unmodified live inputs.
 * Native C and original instructions must agree on every non-stack byte.
 * Original/ROM execution and any phase probes stay outside gameplay. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"

static unsigned points;
static unsigned prefixes;
typedef struct { const char *kind; int16_t x,y,x1,y1; uint16_t colour; } RadarPaint;
static RadarPaint paints[64];
static unsigned paint_count;
static void radar_paint(const char *kind,int16_t x,int16_t y,int16_t x1,int16_t y1,uint16_t colour) {
    if(paint_count==sizeof paints/sizeof paints[0]) abort();
    paints[paint_count++]=(RadarPaint){kind,x,y,x1,y1,colour};
}
static void radar_head(void *context) {
    const int16_t origin=rd_s16(SPAN_ORIGIN_Y),row=rd_s16(REDRAW_STATE_WORD);
    if(!strcmp(context,"tuple")) {
        for(unsigned i=0;i<2;++i) {
            gaddr tuple=0xc3128au+8*i;
            int16_t x=(int16_t)(rd_s16(tuple)+origin),x1=(int16_t)(rd_s16(tuple+4)+origin);
            if(x<0 || x>=320 || x1<0 || x1>=320) continue;
            radar_paint("line",x,(int16_t)(rd_s16(tuple+2)+row),x1,(int16_t)(rd_s16(tuple+6)+row),3);
        }
    } else {
        static const int16_t offsets[][2]={{157,168},{159,168},{158,168},{158,167}};
        /* C31310 checks each of the first two x positions. The last two
         * points inherit the second one's successful visibility gate. */
        for(unsigned i=0;i<4;++i) {
            int16_t x=(int16_t)(offsets[i][0]+origin);
            if(i<2 && (x<0 || x>=320)) return;
            radar_paint("point",x,(int16_t)(offsets[i][1]+row),0,0,12);
        }
    }
}
static void radar_prefix(const PostflightVariantWork *work,void *context) {
    (void)context;
    if(work->table) {
        ++prefixes;
        /* The prefix has cleared these pixels but has not overwritten the
         * retained table yet. Record the original ordered erase operations. */
        for(unsigned i=0;i<11;++i) {
            int16_t x=rd_s16(work->table+4*i),y=rd_s16(work->table+4*i+2);
            if(x==-1) break;
            radar_paint(x<0?"pair":"point",(int16_t)((uint16_t)x&0x7fff),y,0,0,0);
        }
    }
}
static void radar_point(const PostflightVariantWork *work,int marked,void *context) {
    (void)marked;(void)context;
    if(!work->submitted_point) return;
    radar_paint(work->submit_pair?"pair":"point",work->submit_x,work->submit_y,0,0,rd_u16(CURRENT_COLOUR));
    printf("%s{\"record_offset\":%u,\"x\":%d,\"y\":%d,\"pair\":%d,\"colour\":%u,\"phase\":%u}",
        points++?",":"",(uint16_t)(work->record-CONTROL_RECORDS),
        work->submit_x,work->submit_y,work->submit_pair,rd_u16(CURRENT_COLOUR),rd_u8(0xc45883u));
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=(argc==2 || argc==3)?file_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected ||
       !fa18_os_wait_blit_signature_matches(rom)) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
    memcpy(before,machine,sizeof *before);
    const unsigned phase=rd_u8(0xc45883u),selected=rd_u16(SELECTED_RECORD);
    const PostflightVariantHooks tuple={.after_head=radar_head,.after_prefix=radar_prefix,.after_submit=radar_point,.context="tuple"};
    const PostflightVariantHooks fixed={.after_head=radar_head,.after_prefix=radar_prefix,.after_submit=radar_point,.context="fixed"};
    const PostflightDispatchHooks hooks={.tuple_hooks=&tuple,.fixed_hooks=&fixed};
    printf("{\"phase_before\":%u,\"selected_record\":%u,\"points\":[",phase,selected);
    host_draw_postflight_renderer_dispatch_with_hooks(&hooks);
    const unsigned after=rd_u8(0xc45883u);
    memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
    memcpy(machine,before,sizeof *before);
    if(!hud_original(0xc31226u)) return 1;
    for(unsigned i=0;i<0xffc00;++i) {
        const uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"Radar owner RAM %06X original=%02X native=%02X\n",
                i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);return 1;
        }
    }
    if(argc==3) {
        FILE *out=fopen(argv[2],"wb");
        if(!out || fwrite(expected,1,0x100000,out)!=0x100000 || fclose(out)) return 1;
    }
    printf("],\"phase_after\":%u,\"prefix_calls\":%u,\"complete_non_stack_ram_matching\":true,\"paints\":[",after,prefixes);
    for(unsigned i=0;i<paint_count;++i) {
        const RadarPaint *p=&paints[i];
        printf("%s{\"kind\":\"%s\",\"x\":%d,\"y\":%d,\"x1\":%d,\"y1\":%d,\"colour\":%u}",
            i?",":"",p->kind,p->x,p->y,p->x1,p->y1,p->colour);
    }
    puts("]}");
    free(expected);free(before);free(machine);free(data);free(rom);free(state);
    return 0;
}
