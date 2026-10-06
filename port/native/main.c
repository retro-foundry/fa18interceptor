#include "../game/native/frontend.h"
#include "../game/native/menu.h"
#include "../game/globals.h"
#include "../game/memory.h"
#include "../recomp/frame_pacer.h"
#include "replay.h"
#include "../amiga/pcm_output.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { unsigned frame; int key,down; } KeyEvent;
static int write_ppm(const char *path,NativeFrontend *game) {
    FILE *file=fopen(path,"wb"); if(!file) return 0;
    fprintf(file,"P6\n320 256\n255\n");
    for(unsigned i=0;i<320*256;++i) { uint16_t c=game->palette[game->indices[i]]; uint8_t rgb[3]={(uint8_t)(((c>>8)&15)*17),(uint8_t)(((c>>4)&15)*17),(uint8_t)((c&15)*17)}; if(fwrite(rgb,1,3,file)!=3) { fclose(file); return 0; } }
    return fclose(file)==0;
}
int main(int argc,char **argv) {
    const char *adf="local/media/fa18.adf",*save_dir="saves-native",*ppm=NULL,*replay=NULL,*data_out=NULL; int headless=0,running=1,result=1;
    unsigned frames=0,events=0,next=0,iterations=0; KeyEvent keys[1024]; char error[256];
    const char *input=NULL;NativeReplay loop={0};
    const char *wave=NULL;AmigaPcmOutput audio_output={0};int16_t samples[960*2];
    NativeFrontend *game=calloc(1,sizeof *game); SDL_Window *window=NULL; SDL_Renderer *renderer=NULL; SDL_Texture *texture=NULL; uint32_t pixels[320*256];
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"--headless")) headless=1;
        else if(!strcmp(argv[i],"--help")) { puts("fa18_native [--adf PATH] [--save-dir PATH] [--headless --frames N] [--replay E9K] [--input FA18_LOOP_INPUT_V1|FA18_GAME_INPUT_V1 --iterations N] [--ppm PATH] [--data-out PATH] [--wav PATH]"); free(game); return 0; }
        else if(i+1<argc && !strcmp(argv[i],"--adf")) adf=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--save-dir")) save_dir=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--frames")) { char *end; unsigned long n=strtoul(argv[++i],&end,10); if(*end || n>10000000) { fputs("Invalid frame count\n",stderr); goto done; } frames=(unsigned)n; }
        else if(i+1<argc && !strcmp(argv[i],"--ppm")) ppm=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--replay")) replay=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--input")) input=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--iterations")) { char *end;unsigned long n=strtoul(argv[++i],&end,10);if(*end || !n || n>10000000) { fputs("Invalid iteration limit\n",stderr);goto done; } iterations=(unsigned)n; }
        else if(i+1<argc && !strcmp(argv[i],"--data-out")) data_out=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--wav")) wave=argv[++i];
        else { fprintf(stderr,"Unknown/incomplete option: %s\n",argv[i]); goto done; }
    }
    if(headless && !frames) { fputs("Headless runs require --frames N\n",stderr); goto done; }
    if(iterations && !input) { fputs("Iteration limit requires --input\n",stderr);goto done; }
    if(input && !native_replay_load(&loop,input,error,sizeof error)) { fputs(error,stderr);goto done; }
    if(input && !iterations) iterations=loop.end;
    if(input && iterations>loop.end) { fputs("Iteration limit exceeds recorded end\n",stderr);goto done; }
    if(replay) {
        FILE *file=fopen(replay,"r"); char line[128];
        if(!file) { fprintf(stderr,"Cannot open replay: %s\n",replay); goto done; }
        if(!fgets(line,sizeof line,file) || strncmp(line,"E9K_INPUT_V1",12)) { fclose(file); fputs("Invalid replay header\n",stderr); goto done; }
        while(fgets(line,sizeof line,file)) { KeyEvent event; int a,b; if(events==1024 || sscanf(line,"F %u K %d %d %d %d",&event.frame,&event.key,&a,&b,&event.down)!=5 || (events && event.frame<keys[events-1].frame)) { fclose(file); fputs("Invalid replay row\n",stderr); goto done; } keys[events++]=event; }
        fclose(file);
    }
    if(!game || !native_frontend_open(game,adf,save_dir,error,sizeof error)) { fprintf(stderr,"%s\n",game?error:"Allocation failed"); goto done; }
    if(input) { game->begin_update=native_replay_update;game->update_context=&loop; }
    if(!headless) {
        SDL_SetMainReady(); if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER)) goto sdl_error;
        window=SDL_CreateWindow("F/A-18 Interceptor - native intro/menu",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,768,SDL_WINDOW_RESIZABLE);
        if(!window) goto sdl_error;
        renderer=SDL_CreateRenderer(window,-1,0); if(!renderer) goto sdl_error;
        SDL_RenderSetLogicalSize(renderer,320,256); texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,320,256); if(!texture) goto sdl_error;
    }
    FA18FramePacer pacer; fa18_frame_pacer_init(&pacer,SDL_GetPerformanceCounter(),SDL_GetPerformanceFrequency());
    if(!amiga_pcm_open(&audio_output,48000,!headless,wave,error,sizeof error)) {
        fputs(error,stderr);goto done;
    }
    while(running && (!frames || game->ticks<frames) && (!iterations || loop.iteration<iterations)) {
        while(next<events && keys[next].frame<=game->ticks) { native_frontend_event(game,keys[next].key,keys[next].down); ++next; }
        if(!headless) { SDL_Event event; while(SDL_PollEvent(&event)) { if(event.type==SDL_QUIT) running=0; if((event.type==SDL_KEYDOWN || event.type==SDL_KEYUP) && !event.key.repeat) { int key=event.key.keysym.sym; if(key>='a' && key<='z') key-=32; if(key==SDLK_RETURN) key='\r'; if(key==SDLK_BACKSPACE) key='\b'; if(key>=SDLK_F1 && key<=SDLK_F10) key=282+key-SDLK_F1; if(key==SDLK_UP) key=273; if(key==SDLK_DOWN) key=274; if(key==SDLK_RIGHT) key=275; if(key==SDLK_LEFT) key=276; if(key==SDLK_LSHIFT) key=304; if(key==SDLK_RSHIFT) key=303; native_frontend_event(game,key,event.type==SDL_KEYDOWN); } } }
        native_frontend_tick(game);
        native_audio_render(&game->audio,samples,960,48000);
        if(!amiga_pcm_write(&audio_output,samples,960)) {
            fprintf(stderr,"Cannot publish native PCM audio: %s\n",SDL_GetError());goto done;
        }
        if(!headless) {
            for(unsigned i=0;i<320*256;++i) { uint16_t c=game->palette[game->indices[i]]; pixels[i]=0xff000000u|(((c>>8)&15)*17u<<16)|(((c>>4)&15)*17u<<8)|((c&15)*17u); }
            if(SDL_UpdateTexture(texture,NULL,pixels,320*sizeof *pixels) || SDL_RenderClear(renderer) || SDL_RenderCopy(renderer,texture,NULL,NULL)) goto sdl_error;
            SDL_RenderPresent(renderer); uint64_t now=SDL_GetPerformanceCounter(),deadline=fa18_frame_pacer_next(&pacer,now);
            if(deadline>now) SDL_Delay((uint32_t)((deadline-now)*1000/SDL_GetPerformanceFrequency()));
        }
    }
    if(ppm && !write_ppm(ppm,game)) { fprintf(stderr,"Cannot write PPM: %s\n",ppm); goto done; }
    if(data_out) {
        FILE *file=fopen(data_out,"wb");
        if(!file) { perror(data_out); goto done; }
        int written=fwrite(game->storage.buffers,1,sizeof game->storage.buffers,file)==sizeof game->storage.buffers
            && fwrite(game->storage.source,1,sizeof game->storage.source,file)==sizeof game->storage.source;
        if(fclose(file) || !written) { fprintf(stderr,"Cannot write native data: %s\n",data_out); goto done; }
    }
    printf("{\"frames\":%u,\"screen\":\"%s\",\"mode\":%u,\"glyphs\":%u,\"record_updates\":%u,\"scene_frames\":%u,\"terrain_polygons\":%u,\"model_calls\":%u,\"hud_frames\":%u,\"control_frames\":%u,\"scene_selected\":%s,\"stage\":\"%06X\",\"game_tick\":%u,\"timer_pending\":%s,\"timer_yields\":%u,\"display_publications\":%u,\"display_yields\":%u,\"display_pending\":%s,\"displayed_page\":%u,\"postflight_callbacks\":%u,\"postflight_resets\":%u,\"input_passes\":%u,\"input_events\":%u,\"input_queued\":%u,\"update_iterations\":%u,\"replay_iterations\":%u,\"replay_events\":%zu,\"replay_started\":%s,\"voice_ticks\":%u,\"voice_publications\":%u,\"voice_levels\":[[%d,%d],[%d,%d],[%d,%d],[%d,%d]],\"sample_requests\":%u,\"sample_frames\":%u,\"nonzero_sample_frames\":%u,\"audio_device\":%s,\"cpu_emulation\":false,\"chipset_emulation\":false}\n",game->ticks,native_frontend_screen(game),native_menu_selected_mode(game),game->glyphs,game->record_updates,game->scene_frames,game->terrain_polygons,game->model_calls,game->hud_frames,game->control_frames,game->scene_selected?"true":"false",rd_u32(STAGE_CALLBACK),rd_u16(UPDATE_TICK),game->flight_timer_pending?"true":"false",game->timer_yields,game->display_publications,game->display_yields,game->display_pending?"true":"false",game->displayed_page,game->postflight_callbacks,game->postflight_resets,game->input_passes,game->input_events,game->input_count,game->update_iterations,loop.iteration,loop.next,loop.started?"true":"false",game->audio.ticks,game->audio.publications,game->audio.channels[0].period,game->audio.channels[0].volume,game->audio.channels[1].period,game->audio.channels[1].volume,game->audio.channels[2].period,game->audio.channels[2].volume,game->audio.channels[3].period,game->audio.channels[3].volume,game->audio.sample_requests,game->audio.sample_frames,game->audio.nonzero_frames,audio_output.device?"true":"false");
    if(running && iterations && loop.iteration<iterations) {
        fprintf(stderr,"Native input stopped at iteration %u of %u: --frames limit reached%s\n",
                loop.iteration,iterations,loop.started?"":" before main-menu anchor");goto done;
    }
    result=0; goto done;
sdl_error:
    fprintf(stderr,"SDL: %s\n",SDL_GetError());
done:
    if(!amiga_pcm_close(&audio_output)) {fputs("Cannot finish native WAV capture\n",stderr);result=1;}
    native_replay_close(&loop);
    SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    if(game) { native_frontend_close(game); free(game); } return result;
}
