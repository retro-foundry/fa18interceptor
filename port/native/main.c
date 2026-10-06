#include "../game/native/frontend.h"
#include "../recomp/frame_pacer.h"
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
    const char *adf="local/media/fa18.adf",*save_dir="saves-native",*ppm=NULL,*replay=NULL; int headless=0,running=1,result=1;
    unsigned frames=0,events=0,next=0; KeyEvent keys[1024]; char error[256];
    NativeFrontend *game=calloc(1,sizeof *game); SDL_Window *window=NULL; SDL_Renderer *renderer=NULL; SDL_Texture *texture=NULL; uint32_t pixels[320*256];
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"--headless")) headless=1;
        else if(!strcmp(argv[i],"--help")) { puts("fa18_native [--adf PATH] [--save-dir PATH] [--headless --frames N] [--replay E9K] [--ppm PATH]"); free(game); return 0; }
        else if(i+1<argc && !strcmp(argv[i],"--adf")) adf=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--save-dir")) save_dir=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--frames")) { char *end; unsigned long n=strtoul(argv[++i],&end,10); if(*end || n>10000000) { fputs("Invalid frame count\n",stderr); goto done; } frames=(unsigned)n; }
        else if(i+1<argc && !strcmp(argv[i],"--ppm")) ppm=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--replay")) replay=argv[++i];
        else { fprintf(stderr,"Unknown/incomplete option: %s\n",argv[i]); goto done; }
    }
    if(headless && !frames) { fputs("Headless runs require --frames N\n",stderr); goto done; }
    if(replay) {
        FILE *file=fopen(replay,"r"); char line[128];
        if(!file) { fprintf(stderr,"Cannot open replay: %s\n",replay); goto done; }
        if(!fgets(line,sizeof line,file) || strncmp(line,"E9K_INPUT_V1",12)) { fclose(file); fputs("Invalid replay header\n",stderr); goto done; }
        while(fgets(line,sizeof line,file)) { KeyEvent event; int a,b; if(events==1024 || sscanf(line,"F %u K %d %d %d %d",&event.frame,&event.key,&a,&b,&event.down)!=5 || (events && event.frame<keys[events-1].frame)) { fclose(file); fputs("Invalid replay row\n",stderr); goto done; } keys[events++]=event; }
        fclose(file);
    }
    if(!game || !native_frontend_open(game,adf,save_dir,error,sizeof error)) { fprintf(stderr,"%s\n",game?error:"Allocation failed"); goto done; }
    if(!headless) {
        SDL_SetMainReady(); if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER)) goto sdl_error;
        window=SDL_CreateWindow("F/A-18 Interceptor - native intro/menu",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,768,SDL_WINDOW_RESIZABLE);
        if(!window) goto sdl_error;
        renderer=SDL_CreateRenderer(window,-1,0); if(!renderer) goto sdl_error;
        SDL_RenderSetLogicalSize(renderer,320,256); texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,320,256); if(!texture) goto sdl_error;
    }
    FA18FramePacer pacer; fa18_frame_pacer_init(&pacer,SDL_GetPerformanceCounter(),SDL_GetPerformanceFrequency());
    while(running && (!frames || game->ticks<frames)) {
        while(next<events && keys[next].frame<=game->ticks) { if(keys[next].down) native_frontend_key(game,keys[next].key); ++next; }
        if(!headless) { SDL_Event event; while(SDL_PollEvent(&event)) { if(event.type==SDL_QUIT) running=0; if(event.type==SDL_KEYDOWN && !event.key.repeat) { int key=event.key.keysym.sym; if(key>='a' && key<='z') key-=32; if(key==SDLK_RETURN) key='\r'; if(key==SDLK_BACKSPACE) key='\b'; native_frontend_key(game,key); } } }
        native_frontend_tick(game);
        if(!headless) {
            for(unsigned i=0;i<320*256;++i) { uint16_t c=game->palette[game->indices[i]]; pixels[i]=0xff000000u|(((c>>8)&15)*17u<<16)|(((c>>4)&15)*17u<<8)|((c&15)*17u); }
            if(SDL_UpdateTexture(texture,NULL,pixels,320*sizeof *pixels) || SDL_RenderClear(renderer) || SDL_RenderCopy(renderer,texture,NULL,NULL)) goto sdl_error;
            SDL_RenderPresent(renderer); uint64_t now=SDL_GetPerformanceCounter(),deadline=fa18_frame_pacer_next(&pacer,now);
            if(deadline>now) SDL_Delay((uint32_t)((deadline-now)*1000/SDL_GetPerformanceFrequency()));
        }
    }
    if(ppm && !write_ppm(ppm,game)) { fprintf(stderr,"Cannot write PPM: %s\n",ppm); goto done; }
    printf("{\"frames\":%u,\"screen\":\"%s\",\"glyphs\":%u,\"cpu_emulation\":false,\"chipset_emulation\":false}\n",game->ticks,native_frontend_screen(game),game->glyphs);
    result=0; goto done;
sdl_error:
    fprintf(stderr,"SDL: %s\n",SDL_GetError());
done:
    SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    if(game) { native_frontend_close(game); free(game); } return result;
}
