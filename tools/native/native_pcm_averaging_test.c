/* Actual native streams versus unchanged reference accumulator functions.
 * Interval boundaries come from a global rational timeline, not the native
 * cursor loop. This tests pre-filter interpolation, not full Paula timing. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "native/frontend.h"
#include "audio.h"
#include "globals.h"
struct audio_channel_data2 {
    int current_sample,mixvol,sample_accum,sample_accum_time;
    unsigned adk_mask;
};
static struct audio_channel_data2 reference[4];
static struct audio_channel_data2 *audio_data[]={reference,reference+1,reference+2,reference+3,NULL};
#include "original_pcm_averaging.h"
static unsigned gcd(unsigned a,unsigned b) {while(b) {unsigned r=a%b;a=b;b=r;}return a;}
static int16_t actual[4096*2],split[4096*2];
int main(int argc,char **argv) {
    if(argc!=3) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];
    assert(game && native_frontend_open(game,argv[1],argv[2],error,sizeof error));
    const NativePcmResolve resolve=game->audio.resolve;void *owner=game->audio.sample_context;
    gaddr voice=rd_u32(SOUND_VOICES+4*SOUND_PROGRAMMED),samples=rd_u32(voice);
    unsigned differs_from_hold=0,compared=0;
    static const unsigned periods[]={124,300,358,32767},rates[]={44100,48000,709379};
    for(unsigned rate_index=0;rate_index<3;++rate_index) for(unsigned p=0;p<4;++p)
    for(unsigned channel=0;channel<4;++channel) {
        unsigned rate=rates[rate_index],period=periods[p],factor=gcd(3546895,rate);
        uint64_t interval=3546895/factor,byte_time=(uint64_t)period*rate/factor;
        /* Volume 21 bounds the unchanged reference's 32-bit accumulator at
         * every tested rational interval, including signed source samples. */
        for(unsigned mode=0;mode<2;++mode) {
            memset(&game->audio,0,sizeof game->audio);
            game->audio.resolve=resolve;game->audio.sample_context=owner;
            native_audio_bind(&game->audio);
            for(unsigned c=0;c<4;++c) wr_u32(VOICE_SLOTS+4*c,0);
            wr_u32(voice+VOICE_PERIOD,period<<16);wr_u32(voice+VOICE_VOLUME,21u<<16);
            wr_u32(voice+16,0xffffffffu);wr_u32(MASTER_VOLUME,0x3f0000u);
            wr_u32(VOICE_SLOTS+4*channel,voice);native_audio_request_channel((int)channel);
            if(!mode) native_audio_render(&game->audio,actual,4096,rate);
            else for(unsigned frame=0;frame<4096;) {
                unsigned count=frame%53+1;if(count>4096-frame) count=4096-frame;
                native_audio_render(&game->audio,split+2*frame,count,rate);frame+=count;
            }
        }
        assert(!memcmp(actual,split,sizeof actual));
        for(unsigned frame=0;frame<4096;++frame) {
            uint64_t lo=(uint64_t)frame*interval,hi=lo+interval;
            memset(reference,0,sizeof reference);
            for(unsigned c=0;c<4;++c) reference[c].adk_mask=~0u;
            reference[channel].mixvol=21;
            for(uint64_t byte=lo/byte_time;byte<=(hi-1)/byte_time;++byte) {
                uint64_t begin=byte*byte_time,end=begin+byte_time;
                if(begin<lo) begin=lo;if(end>hi) end=hi;
                reference[channel].current_sample=(int8_t)rd_u8(samples+(gaddr)(byte%32));
                anti_prehandler((unsigned long)(end-begin));
            }
            int values[4];samplexx_anti_handler(values,0,4);
            const unsigned side=channel==0 || channel==3?0:1;
            if(actual[2*frame+side]!=values[channel]*2 || actual[2*frame+1-side])
                fprintf(stderr,"PCM average mismatch: rate %u period %u channel %u frame %u actual %d reference %d\n",
                    rate,period,channel,frame,actual[2*frame+side],values[channel]*2);
            assert(actual[2*frame+side]==values[channel]*2 && actual[2*frame+1-side]==0);
            int held=(int8_t)rd_u8(samples+(gaddr)((lo/byte_time)%32))*21*2;
            if(actual[2*frame+side]!=held) ++differs_from_hold;
            ++compared;
        }
    }
    assert(differs_from_hold);
    printf("%u native PCM intervals match unchanged source averaging across 48 streams; %u reject sample holding; source %s\n",
        compared,differs_from_hold,ORIGINAL_ANTI_SOURCE_SHA256);
    native_frontend_close(game);free(game);return 0;
}
