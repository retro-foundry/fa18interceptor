#include "frame_delta.h"
#include "../amiga/sha256.h"
#include <string.h>

static int publish(FA18FrameDelta *trace,const void *data,size_t size) {
    if(trace->failed) return 0;
    if(size>trace->budget-trace->bytes) {
        fputs("Frame delta exceeds capture budget; use a shorter range or explicit budget\n",stderr);
    } else if(fwrite(data,1,size,trace->file)==size) {
        trace->bytes+=size;return 1;
    } else fputs("Cannot write frame delta\n",stderr);
    trace->failed=1;return 0;
}
static int word(FA18FrameDelta *trace,uint32_t value) {
    const uint8_t bytes[]={(uint8_t)value,(uint8_t)(value>>8),(uint8_t)(value>>16),(uint8_t)(value>>24)};
    return publish(trace,bytes,sizeof bytes);
}
int fa18_frame_delta_open(FA18FrameDelta *trace,const char *path,size_t budget) {
    memset(trace,0,sizeof *trace);trace->budget=budget;
    trace->file=fopen(path,"wb");
    if(!trace->file) {perror(path);trace->failed=1;return 0;}
    if(setvbuf(trace->file,trace->file_buffer,_IOFBF,sizeof trace->file_buffer)) {
        fputs("Cannot initialize fixed frame delta file buffer\n",stderr);trace->failed=1;return 0;
    }
    return publish(trace,"FA18_FRAME_DELTA_V1\n",20);
}
int fa18_frame_delta_write(FA18FrameDelta *trace,unsigned iteration,unsigned frame,
    unsigned boundary,uint16_t saved_tick,FA18FlightTraceReader reader,void *context) {
    if(!trace->file || trace->failed) return 0;
    const uint8_t *parts[]={reader(context,0,0x80000),reader(context,0xc00000,0x80000)};
    if(!parts[0] || !parts[1]) {
        fputs("Frame delta reader cannot supply complete host memory\n",stderr);trace->failed=1;return 0;
    }
    if(!word(trace,1) || !word(trace,iteration) || !word(trace,frame) ||
       !word(trace,boundary) || !word(trace,saved_tick)) return 0;
    /* Whole changed 64-byte blocks preserve unchanged neighbouring bytes too.
     * Offsets are monotonically ordered and there is no XOR/address guessing.
     * The decoded full MiB is hashed, not just the changed blocks. */
    for(unsigned part=0;part<2;++part) for(unsigned at=0;at<0x80000;at+=64) {
        const unsigned offset=part*0x80000+at;
        if(memcmp(trace->previous+offset,parts[part]+at,64)) {
            if(!word(trace,offset) || !publish(trace,parts[part]+at,64)) return 0;
            memcpy(trace->previous+offset,parts[part]+at,64);
        }
    }
    char hash[65];amiga_sha256_hex(trace->previous,sizeof trace->previous,hash);
    if(!word(trace,0xffffffffu) || !publish(trace,hash,64)) return 0;
    ++trace->snapshots;return 1;
}
int fa18_frame_delta_close(FA18FrameDelta *trace) {
    if(!trace || !trace->file) return !trace || !trace->failed;
    if(!trace->failed && (!word(trace,0) || !word(trace,trace->snapshots))) trace->failed=1;
    if(fclose(trace->file)) {fputs("Cannot finish frame delta\n",stderr);trace->failed=1;}
    trace->file=NULL;return !trace->failed;
}
