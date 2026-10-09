#include "pcm_output.h"
#include <SDL.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void amiga_pcm_consume(AmigaPcmOutput *output,int16_t *samples,unsigned frames) {
    unsigned count=frames<output->queued_frames?frames:output->queued_frames;
    unsigned first=AMIGA_PCM_RING_FRAMES-output->read_frame;
    if(first>count) first=count;
    if(first) memcpy(samples,output->ring+2*output->read_frame,4*first);
    if(count>first) memcpy(samples+2*first,output->ring,4*(count-first));
    if(frames>count) memset(samples+2*count,0,4*(frames-count));
    output->read_frame=(output->read_frame+count)%AMIGA_PCM_RING_FRAMES;
    output->queued_frames-=count;
}
static void consume(void *context,Uint8 *bytes,int length) {
    /* SDL serializes this callback with SDL_LockAudioDevice. Only copied PCM
     * crosses threads; the callback never executes game/sample sequencing. */
    amiga_pcm_consume(context,(int16_t *)bytes,(unsigned)length/4);
}

static void little32(uint8_t *p,uint32_t n) {
    for(unsigned i=0;i<4;++i) p[i]=(uint8_t)(n>>(8*i));
}
static int header(AmigaPcmOutput *output) {
    uint8_t bytes[44]={0};
    memcpy(bytes,"RIFF",4);little32(bytes+4,36+output->bytes);
    memcpy(bytes+8,"WAVEfmt ",8);little32(bytes+16,16);
    bytes[20]=1;bytes[22]=2;little32(bytes+24,output->rate);
    little32(bytes+28,output->rate*4);bytes[32]=4;bytes[34]=16;
    memcpy(bytes+36,"data",4);little32(bytes+40,output->bytes);
    return fwrite(bytes,1,sizeof bytes,output->wave)==sizeof bytes;
}
int amiga_pcm_open(AmigaPcmOutput *output,unsigned rate,int audible,const char *wave,
                   char *error,size_t capacity) {
    memset(output,0,sizeof *output);output->rate=rate;
    if(wave) {
        output->wave=fopen(wave,"wb");
        if(!output->wave || setvbuf(output->wave,output->wave_buffer,_IOFBF,sizeof output->wave_buffer) || !header(output)) {
            if(capacity) snprintf(error,capacity,"Cannot create PCM capture %s: %s",wave,strerror(errno));
            amiga_pcm_close(output);return 0;
        }
    }
    if(audible) {
        output->ring=calloc(AMIGA_PCM_RING_FRAMES,4);
        if(!output->ring) {
            if(capacity) snprintf(error,capacity,"Cannot allocate fixed PCM ring");
            amiga_pcm_close(output);return 0;
        }
        SDL_AudioSpec spec={0};spec.freq=(int)rate;spec.format=AUDIO_S16SYS;
        spec.channels=2;spec.samples=1024;spec.callback=consume;spec.userdata=output;
        if(SDL_InitSubSystem(SDL_INIT_AUDIO)==0)
            output->device=SDL_OpenAudioDevice(NULL,0,&spec,NULL,0);
        if(!output->device) {
            if(capacity) snprintf(error,capacity,"Cannot open PCM audio: %s",SDL_GetError());
            amiga_pcm_close(output);return 0;
        }
        SDL_PauseAudioDevice(output->device,0);
    }
    return 1;
}
int amiga_pcm_write(AmigaPcmOutput *output,const int16_t *samples,unsigned frames) {
    output->error=NULL;
    if(frames>UINT32_MAX/4 || (output->wave && output->bytes>UINT32_MAX-36-4*frames)) return 0;
    if(output->device) {
        SDL_LockAudioDevice(output->device);
        if(frames>AMIGA_PCM_RING_FRAMES-output->queued_frames) {
            SDL_UnlockAudioDevice(output->device);
            output->error="Fixed PCM ring exhausted; queued PCM was preserved";
            return 0;
        }
        const unsigned write=(output->read_frame+output->queued_frames)%AMIGA_PCM_RING_FRAMES;
        unsigned first=AMIGA_PCM_RING_FRAMES-write;
        if(first>frames) first=frames;
        if(first) memcpy(output->ring+2*write,samples,4*first);
        if(frames>first) memcpy(output->ring,samples+2*first,4*(frames-first));
        output->queued_frames+=frames;
        SDL_UnlockAudioDevice(output->device);
    }
    if(output->wave) {
        uint8_t bytes[4096];
        unsigned remaining=frames*2;
        while(remaining) {
            unsigned count=remaining>sizeof bytes/2?sizeof bytes/2:remaining;
            for(unsigned i=0;i<count;++i) {
                uint16_t n=(uint16_t)*samples++;
                bytes[2*i]=(uint8_t)n;bytes[2*i+1]=(uint8_t)(n>>8);
            }
            if(fwrite(bytes,2,count,output->wave)!=count) return 0;
            remaining-=count;
        }
    }
    if(output->wave) output->bytes+=4*frames;
    return 1;
}
int amiga_pcm_close(AmigaPcmOutput *output) {
    int result=1;
    if(output->device) SDL_CloseAudioDevice(output->device);
    free(output->ring);
    if(output->wave) {
        if(fseek(output->wave,0,SEEK_SET) || !header(output)) result=0;
        if(fclose(output->wave)) result=0;
    }
    memset(output,0,sizeof *output);return result;
}
