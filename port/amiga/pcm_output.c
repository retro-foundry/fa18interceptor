#include "pcm_output.h"
#include <SDL.h>
#include <string.h>
#include <errno.h>

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
        if(!output->wave || !header(output)) {
            if(capacity) snprintf(error,capacity,"Cannot create PCM capture %s: %s",wave,strerror(errno));
            amiga_pcm_close(output);return 0;
        }
    }
    if(audible) {
        SDL_AudioSpec spec={0};spec.freq=(int)rate;spec.format=AUDIO_S16SYS;
        spec.channels=2;spec.samples=1024;
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
    if(frames>UINT32_MAX/4 || (output->wave && output->bytes>UINT32_MAX-36-4*frames)) return 0;
    if(output->device && SDL_QueueAudio(output->device,samples,4*frames)) return 0;
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
    if(output->wave) {
        if(fseek(output->wave,0,SEEK_SET) || !header(output)) result=0;
        if(fclose(output->wave)) result=0;
    }
    memset(output,0,sizeof *output);return result;
}
