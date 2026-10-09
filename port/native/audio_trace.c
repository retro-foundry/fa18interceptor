/* Read-only audio evidence. Streams keep their existing host buffer owners;
 * this sink never advances voices, selects samples or schedules playback. */
#include "audio_trace.h"
#include "../amiga/sha256.h"
#include "../game/globals.h"
#include "../game/memory.h"
#include <stdarg.h>
#include <string.h>
typedef struct { char text[4096];size_t size;int failed; } AudioRow;
static void append(AudioRow *row,const char *format,...) {
    if(row->failed) return;
    va_list args;va_start(args,format);
    int count=vsnprintf(row->text+row->size,sizeof row->text-row->size,format,args);
    va_end(args);
    if(count<0 || (size_t)count>=sizeof row->text-row->size) row->failed=1;
    else row->size+=(size_t)count;
}
static int write_row(NativeAudioTrace *trace,const AudioRow *row) {
    if(trace->failed) return 0;
    if(row->failed || row->size>trace->budget-trace->bytes) {
        fputs("Native audio trace exceeds its row/capture budget\n",stderr);
    } else if(fwrite(row->text,1,row->size,trace->file)==row->size) {
        trace->bytes+=row->size;return 1;
    } else fputs("Cannot write native audio trace\n",stderr);
    trace->failed=1;return 0;
}
int native_audio_trace_open(NativeAudioTrace *trace,const char *path,size_t budget) {
    memset(trace,0,sizeof *trace);trace->budget=budget;
    trace->file=fopen(path,"wb");
    if(!trace->file) {perror(path);trace->failed=1;return 0;}
    if(setvbuf(trace->file,trace->file_buffer,_IOFBF,sizeof trace->file_buffer)) {
        fputs("Cannot initialize native audio trace buffer\n",stderr);trace->failed=1;return 0;
    }
    AudioRow row={0};append(&row,"{\"format\":\"FA18_NATIVE_AUDIO_V1\"}\n");
    return write_row(trace,&row);
}
void native_audio_trace_event(void *context,const NativeAudioEvent *event) {
    NativeAudioTrace *trace=context;AudioRow row={0};char hash[65]="";
    if(trace->failed) return;
    if(event->buffer.data) amiga_sha256_hex(event->buffer.data,event->buffer.bytes,hash);
    append(&row,"{\"kind\":\"%s\",\"tick\":%u,\"sample_frame\":%llu,\"channel\":%u,\"voice\":%u,\"active\":%s,\"samples\":%u,\"bytes\":%u,\"period\":%u,\"volume\":%u,\"sha256\":\"%s\"}\n",
        event->kind==NATIVE_AUDIO_REQUEST?"request":"stop",event->tick,
        (unsigned long long)event->output_frame,event->channel,event->voice,
        event->sample.active?"true":"false",event->sample.samples,event->sample.bytes,
        (uint16_t)event->sample.output.period,(uint16_t)event->sample.output.volume,hash);
    if(write_row(trace,&row)) {
        if(event->kind==NATIVE_AUDIO_REQUEST) ++trace->requests;else ++trace->stops;
    }
}
int native_audio_trace_boundary(NativeAudioTrace *trace,NativeFrontend *game,unsigned iteration) {
    AudioRow row={0};
    append(&row,"{\"kind\":\"boundary\",\"tick\":%u,\"iteration\":%u,\"mode\":%u,\"stage\":%u,\"sample_frames\":%u,\"master_volume\":\"%08x\",\"voices\":[",
        game->ticks,iteration,rd_u8(MODE_SELECT),rd_u32(STAGE_CALLBACK),
        game->audio.sample_frames,rd_u32(MASTER_VOLUME));
    for(unsigned channel=0;channel<4;++channel) {
        gaddr address=rd_u32(VOICE_SLOTS+4*channel);
        append(&row,"%s{\"address\":%u,\"record\":",channel?",":"",address);
        if(!address) append(&row,"null}");
        else {
            append(&row,"\"");
            for(unsigned byte=0;byte<64;++byte) append(&row,"%02x",rd_u8(address+byte));
            append(&row,"\"}");
        }
    }
    append(&row,"],\"channels\":[");
    for(unsigned channel=0;channel<4;++channel)
        append(&row,"%s{\"period\":%u,\"volume\":%u,\"playing\":%s,\"cursor\":%u,\"phase\":%llu}",
            channel?",":"",(uint16_t)game->audio.channels[channel].period,
            (uint16_t)game->audio.channels[channel].volume,game->audio.streams[channel].playing?"true":"false",
            game->audio.streams[channel].cursor,(unsigned long long)game->audio.streams[channel].phase);
    append(&row,"]}\n");
    if(!write_row(trace,&row)) return 0;
    ++trace->boundaries;return 1;
}
int native_audio_trace_close(NativeAudioTrace *trace) {
    if(!trace->file) return !trace->failed;
    AudioRow row={0};append(&row,"{\"end\":true,\"requests\":%u,\"stops\":%u,\"boundaries\":%u}\n",
        trace->requests,trace->stops,trace->boundaries);
    if(!trace->failed) write_row(trace,&row);
    if(fclose(trace->file)) {fputs("Cannot close native audio trace\n",stderr);trace->failed=1;}
    trace->file=NULL;return !trace->failed;
}
