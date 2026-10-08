/* Component ownership check using the actual frontend's sample resolver.
 * Synthetic voice inputs exercise the boundary; normal-key PCM replays are
 * checked separately and never receive these component states. */
#include "native/frontend.h"
#include "audio.h"
#include "globals.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv) {
    if(argc!=3) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);
    NativeStorage *other=calloc(1,sizeof *other);
    char error[256];
    if(!game || !other || !native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"PCM owner startup: %s\n",game && other?error:"allocation failed");return 1;
    }
    static const gaddr bases[]={0x7000,0xc60000};
    const gaddr voice=rd_u32(SOUND_VOICES+4*SOUND_PROGRAMMED);
    assert(voice);
    for(unsigned bank=0;bank<2;++bank) {
        free_all_voices();
        /* Consume the empty-slot request before starting the next buffer;
         * a coalesced stop/start intentionally keeps its current buffer. */
        int16_t stereo[2];native_audio_render(&game->audio,stereo,1,48000);
        int8_t *owned=(int8_t *)native_storage_span(&game->storage,bases[bank],32);
        int8_t *decoy=(int8_t *)native_storage_span(other,bases[bank],32);
        owned[1]=7;decoy[1]=127;
        /* C500D8 publishes the odd address; the sample fetch aligns it. */
        wr_u32(voice,bases[bank]+1);wr_u32(voice+4,32);
        wr_u32(voice+VOICE_PERIOD,310u<<16);wr_u32(voice+VOICE_VOLUME,21u<<16);
        wr_u32(voice+16,0xffffffffu);wr_u32(MASTER_VOLUME,0x3f0000u);
        wr_u32(VOICE_SLOTS,voice);native_audio_request_channel(0);
        native_audio_render(&game->audio,stereo,1,48000);
        assert(game->audio.streams[0].current.data==owned && game->audio.streams[0].current.bytes==32);
        /* Keep this short observation inside the current byte: no new game
         * voice request occurs while the other data owner is bound. */
        game->audio.streams[0].cursor=1;game->audio.streams[0].phase=0;
        game->audio.streams[0].period=65535;game->audio.channels[0].period=-1;
        const unsigned requests=game->audio.sample_requests;
        native_storage_bind(other);
        native_audio_render(&game->audio,stereo,1,48000);
        assert(stereo[0]==7*21*2 && !stereo[1]);
        owned[1]=-9;
        native_audio_render(&game->audio,stereo,1,48000);
        assert(stereo[0]==-9*21*2 && !stereo[1]);
        assert(game->audio.sample_requests==requests);
        native_storage_bind(&game->storage);
    }
    native_frontend_close(game);free(other);free(game);
    puts("Two owned PCM banks preserve aligned, signed, live bytes across unrelated global data bindings");
    return 0;
}
