/* C0EFD4 pre-input evidence, without retaining a MiB of RAM per update.
 * Record cores include every byte through +A4; drawing hashes cover every
 * byte of both 320x200 four-plane pages in C1612C draw/display role order.
 * Timer/readout bytes remain explicit: no HUD masks or fitted clock offsets. */
#include "flight_trace.h"
#include "../amiga/sha256.h"
#include <stdarg.h>
#include <string.h>

typedef struct { const char *name; uint32_t address; size_t size; } TraceField;
static const TraceField fields[]={
    {"stage",0xc1820c,4}, {"game_tick",0xc458da,2},
    {"phase",0xc458de,2}, {"mouse_coordinates",0xc45776,4},
    {"selected_record",0xc459c0,2},
    {"mode",0xc458a6,1}, {"target_record",0xc458dc,2},
    {"controls",0xc4582e,3}, {"observer",0xc45c32,24},
    {"camera_matrix",0xc45c20,18}, {"view_matrix",0xc45bd8,18},
    {"view_pan_rotate",0xc45a94,4}, {"view_attitude",0xc45a88,12},
    {"view_side",0xc458b2,1}, {"message_line",0xc4580a,26},
    {"message_drawn",0xc45ae4,2}, {"info_request",0xc45886,1},
    {"info_redraws",0xc4583c,1}, {"info_page",0xc459c4,2},
    {"info_delay",0xc45887,1}, {"cockpit_flags",0xc458cc,2},
    {"post_input_aux",0xc45795,1}, {"update_hud_mode",0xc45836,1},
    {"context_select",0xc45785,1},
    {"origin_detail_mode",0xc458ae,1},
    {"view_hold",0xc45891,1}, {"gauge_refresh",0xc45837,1},
    {"timer_flags",0xc458ce,2}, {"sample_seconds",0xc45af2,4},
    {"sample_fraction",0xc45af6,4}, {"previous_seconds",0xc45b02,4},
    {"previous_fraction",0xc45b06,4}, {"elapsed_total",0xc45b10,4},
    {"partial_total",0xc45b0e,2}, {"notified_total",0xc45b14,4},
    {"elapsed_sample",0xc45b0a,4}, {"poll_seconds",0xc45afa,4},
    {"poll_fraction",0xc45afe,4}, {"rate_index",0xc458be,1},
    {"primary_count",0xc45884,1}, {"secondary_count",0xc45885,1},
    {"view_mode",0xc457a7,1}
};
typedef struct { char text[12288]; size_t size; int failed; } TraceRow;
static void append(TraceRow *row,const char *format,...) {
    va_list args;
    if(row->failed) return;
    va_start(args,format);
    const int count=vsnprintf(row->text+row->size,sizeof row->text-row->size,format,args);
    va_end(args);
    if(count<0 || (size_t)count>=sizeof row->text-row->size) row->failed=1;
    else row->size+=(size_t)count;
}
static void hex(TraceRow *row,const uint8_t *data,size_t size) {
    static const char digits[]="0123456789abcdef";
    if(!data || size>(sizeof row->text-row->size-1)/2) {row->failed=1;return;}
    for(size_t i=0;i<size;++i) {
        row->text[row->size++]=digits[data[i]>>4];
        row->text[row->size++]=digits[data[i]&15];
    }
    row->text[row->size]=0;
}
static uint32_t integer(FA18FlightTraceReader reader,void *context,uint32_t address,size_t size) {
    const uint8_t *data=reader(context,address,size);
    uint32_t value=0;
    if(data) for(size_t i=0;i<size;++i) value=value<<8|data[i];
    return value;
}
static int publish(FA18FlightTrace *trace,const TraceRow *row) {
    if(trace->failed) return 0;
    if(row->failed) fputs("Flight trace row exceeds diagnostic buffer\n",stderr);
    else if(row->size>trace->budget-trace->bytes)
        fputs("Flight trace exceeds capture budget; use a shorter run or an explicit budget\n",stderr);
    else if(fwrite(row->text,1,row->size,trace->file)==row->size) {
        trace->bytes+=row->size;return 1;
    } else fputs("Cannot write flight trace\n",stderr);
    trace->failed=1;return 0;
}
int fa18_flight_trace_open(FA18FlightTrace *trace,const char *path,size_t budget) {
    memset(trace,0,sizeof *trace);trace->budget=budget;
    trace->file=fopen(path,"wb");
    if(!trace->file) {perror(path);trace->failed=1;return 0;}
    TraceRow row={0};
    append(&row,"{\"format\":\"FA18_FLIGHT_TRACE_V2\",\"boundary\":\"C0EFD4/pre-input\","
                "\"record_address\":%u,\"record_stride\":512,\"record_size\":164,"
                "\"record_count\":16,\"plane_bytes\":8000,\"fields\":[",0xc46184u);
    for(size_t i=0;i<sizeof fields/sizeof fields[0];++i)
        append(&row,"%s{\"name\":\"%s\",\"address\":%u,\"size\":%zu}",
            i?",":"",fields[i].name,fields[i].address,fields[i].size);
    append(&row,"]}\n");
    return publish(trace,&row);
}
int fa18_flight_trace_write(FA18FlightTrace *trace,unsigned iteration,unsigned frame,
                            unsigned width,unsigned height,
                            FA18FlightTraceReader reader,void *context) {
    if(!trace->file || trace->failed) return 0;
    TraceRow row={0};
    append(&row,"{\"iteration\":%u,\"frame\":%u,\"records\":[",iteration,frame);
    for(unsigned i=0;i<16;++i) {
        append(&row,"%s\"",i?",":"");hex(&row,reader(context,0xc46184u+i*512,164),164);
        append(&row,"\"");
    }
    append(&row,"],\"fields\":[");
    for(size_t i=0;i<sizeof fields/sizeof fields[0];++i) {
        append(&row,"%s\"",i?",":"");hex(&row,reader(context,fields[i].address,fields[i].size),fields[i].size);
        append(&row,"\"");
    }
    const uint32_t draw=integer(reader,context,0xc4566c,2);
    const uint32_t table=integer(reader,context,0xc456b6,4);
    int valid=draw<2 && table==0xc4566eu+16*draw && width==320 && height==200;
    const uint8_t *planes[8]={0};
    if(valid) for(unsigned role=0;role<2;++role) for(unsigned plane=0;plane<4;++plane) {
        const uint32_t address=integer(reader,context,0xc4566eu+16*(draw^role)+4*plane,4);
        planes[role*4+plane]=reader(context,address,8000);
        if(!planes[role*4+plane]) valid=0;
    }
    append(&row,"],\"width\":%u,\"height\":%u,\"draw_page\":%u,\"page_table\":%u,\"pages_valid\":%s,\"pages\":[",
        width,height,draw,table,valid?"true":"false");
    if(valid) for(unsigned i=0;i<8;++i) {
        char digest[65];amiga_sha256_hex(planes[i],8000,digest);
        append(&row,"%s\"%s\"",i?",":"",digest);
    }
    append(&row,"]}\n");
    if(!publish(trace,&row)) return 0;
    ++trace->rows;return 1;
}
int fa18_flight_trace_close(FA18FlightTrace *trace) {
    if(!trace->file) return !trace->failed;
    TraceRow row={0};
    append(&row,"{\"end\":true,\"rows\":%u}\n",trace->rows);
    if(!trace->failed) publish(trace,&row);
    if(fclose(trace->file)) {fputs("Cannot finish flight trace\n",stderr);trace->failed=1;}
    trace->file=NULL;return !trace->failed;
}
