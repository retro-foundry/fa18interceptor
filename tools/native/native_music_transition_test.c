/* Actual frontend/key/PCM path: title -> ducked menu -> flight -> menu.
 * C0E3E6/C17B96 start music; C11478 ducks it; C0FECE stops it. */
#include "native/frontend.h"
#include "globals.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int music_buffer(NativeFrontend *game,NativePcmBuffer buffer) {
    if(!buffer.data) return 0;
    for(unsigned slot=13;slot<35;++slot) {
        const gaddr voice=rd_u32(SOUND_VOICES+4*slot);
        if(voice && buffer.data==native_storage_span(&game->storage,rd_u32(voice),buffer.bytes))
            return 1;
    }
    return 0;
}
static void music_present(NativeFrontend *game) {
    for(unsigned channel=0;channel<2;++channel) {
        assert(game->audio.streams[channel].playing);
        assert(music_buffer(game,game->audio.streams[channel].current));
    }
}
static void music_absent(NativeFrontend *game) {
    for(unsigned channel=0;channel<4;++channel) {
        assert(!music_buffer(game,game->audio.streams[channel].current));
        assert(!music_buffer(game,game->audio.streams[channel].next));
    }
}
int main(int argc,char **argv) {
    if(argc!=4) return 1;
    const unsigned digit=(unsigned)atoi(argv[3]);
    if(digit!=1 && digit!=2 && digit!=5 && digit!=7) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];
    if(!game || !native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"Music transition startup: %s\n",game?error:"allocation failed");return 1;
    }
    assert(native_pcm_filter_begin(&game->audio.output_filter,48000)); /* Playable main's output profile. */
    const unsigned mode=digit==1?3:digit==2?1:digit==5?9:6;
    int16_t stereo[1920];unsigned flight_checks=0;
    const unsigned end=digit==2?12000:digit==7?11000:9000;
    while(game->ticks<end) {
        const unsigned tick=game->ticks;
        if(tick==1800) {
            assert(game->screen==NATIVE_SPLASH);
            assert(rd_u32(MASTER_VOLUME)==0x3f0000u);
            assert(rd_u32(MASTER_VOLUME_TARGET)==0x3f0000u);
            native_frontend_event(game,'X',1); /* Any ordinary key acknowledges. */
            assert(rd_u32(MASTER_VOLUME_TARGET)==0x1f0000u);
            assert(rd_u8(VOLUME_FADING)==1);
        }
        if(tick==1802) native_frontend_event(game,'X',0);
        if(tick==2400) {
            assert(game->screen==NATIVE_MENU && !game->menu_setup.pending);
            assert(rd_u32(MASTER_VOLUME)==0x1f0000u);
            music_present(game);
            assert(game->audio.channels[0].volume==31 && game->audio.channels[1].volume==31);
        }
        if(tick==3000) native_frontend_event(game,'0'+(int)digit,1);
        if(tick==3002) native_frontend_event(game,'0'+(int)digit,0);
        if(digit!=1 && tick==5000) native_frontend_event(game,13,1);
        if(digit!=1 && tick==5002) native_frontend_event(game,13,0);
        if(digit!=1 && tick==6500) native_frontend_event(game,32,1);
        if(digit!=1 && tick==6502) native_frontend_event(game,32,0);
        if(digit==7 && tick==8000) native_frontend_event(game,13,1);
        if(digit==7 && tick==8002) native_frontend_event(game,13,0);
        if(digit==7 && tick==8500) native_frontend_event(game,32,1);
        if(digit==7 && tick==8502) native_frontend_event(game,32,0);
        /* Original Shift-Escape abandons flight; plain Escape pauses it. */
        if(digit==2 && tick==9000) native_frontend_event(game,304,1);
        if(digit==2 && tick==9001) native_frontend_event(game,27,1);
        if(digit==2 && tick==9003) native_frontend_event(game,27,0);
        if(digit==2 && tick==9004) native_frontend_event(game,304,0);
        native_frontend_tick(game);
        native_audio_render(&game->audio,stereo,960,48000);
        if(game->ticks<1800) {
            assert(game->screen==NATIVE_SPLASH);
            music_present(game);
            assert(rd_u32(MASTER_VOLUME)==0x3f0000u);
        }
        if(game->ticks>=3500 && (digit!=2 || game->ticks<9000)) {
            music_absent(game);++flight_checks;
            assert(rd_u8(MODE_SELECT)==mode);
        }
    }
    assert(flight_checks && game->scene_frames && game->hud_frames);
    if(digit!=2) assert(rd_u32(STAGE_CALLBACK)==0xc10daeu);
    if(digit==2) {
        if(game->screen!=NATIVE_MENU || rd_u8(MODE_SELECT))
            fprintf(stderr,"Return reached %s mode %u stage %06X\n",
                native_frontend_screen(game),rd_u8(MODE_SELECT),rd_u32(STAGE_CALLBACK));
        assert(game->screen==NATIVE_MENU && rd_u8(MODE_SELECT)==0);
        assert(rd_u32(MASTER_VOLUME)==0x1f0000u);
        music_present(game);
    }
    native_frontend_close(game);free(game);
    printf("Title music, first-key ducking, mode %u music-free buffers%s pass\n",
        mode,digit==2?", and music restart on menu return":"");
    return 0;
}
