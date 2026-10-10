#include "../game/native/frontend.h"
#include "../game/native/menu.h"
#include "../game/native/clock.h"
#include "../game/globals.h"
#include "../game/memory.h"
#include "../recomp/frame_pacer.h"
#include "replay.h"
#include "frame_capture.h"
#include "flight_trace.h"
#include "frame_delta.h"
#include "audio_trace.h"
#include "host_input.h"
#include "../amiga/pcm_output.h"
#include "../amiga/sdl_memory.h"
#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef struct { unsigned frame; char kind; int a,b,c,d; } HostEvent;
typedef struct { uint64_t origin,frequency; } HostClock;
static uint64_t host_microseconds(void *context) {
    const HostClock *clock=context;
    const uint64_t elapsed=SDL_GetPerformanceCounter()-clock->origin;
    return (elapsed/clock->frequency)*1000000u+
        (elapsed%clock->frequency)*1000000u/clock->frequency;
}
typedef struct {
    NativeFrameCapture *capture;
    NativeReplay *replay;
    FA18FlightTrace *trace;
    FA18FrameDelta *delta;
    unsigned delta_first,delta_count;
    unsigned delta_completed,delta_phase;
    uint16_t delta_saved_tick;
} FrameDiagnostics;
static const uint8_t *trace_bytes(void *context,uint32_t address,size_t size) {
    NativeStorage *storage=context;
    if(address<0x80000u && size<=0x80000u-address) return storage->buffers+address;
    if(address>=0xc00000u && address<0xc80000u && size<=0xc80000u-address)
        return storage->source+address-0xc00000u;
    return NULL;
}
static void observe_frame(NativeFrontend *game,enum NativeFrameBoundary boundary,
                          uint16_t saved_tick,void *context) {
    FrameDiagnostics *diagnostics=context;
    const unsigned iteration=diagnostics->replay?diagnostics->replay->iteration:game->update_iterations;
    if(diagnostics->delta && iteration>=diagnostics->delta_first &&
       iteration-diagnostics->delta_first<diagnostics->delta_count &&
       (boundary==NATIVE_FRAME_INPUT_BEGIN || boundary==NATIVE_FRAME_BODY_BEGIN ||
        boundary==NATIVE_FRAME_BODY_END || boundary==NATIVE_FRAME_OWNER_EXIT))
    {
        const int expected=iteration==diagnostics->delta_first+diagnostics->delta_completed &&
            ((diagnostics->delta_phase==0 && boundary==NATIVE_FRAME_INPUT_BEGIN) ||
             (diagnostics->delta_phase==1 && boundary==NATIVE_FRAME_BODY_BEGIN) ||
             (diagnostics->delta_phase==2 && (boundary==NATIVE_FRAME_BODY_END || boundary==NATIVE_FRAME_OWNER_EXIT)));
        if(!expected) {
            fputs("Frame delta requires consecutive complete flight bodies\n",stderr);diagnostics->delta->failed=1;
        } else {
            if(boundary==NATIVE_FRAME_BODY_BEGIN) diagnostics->delta_saved_tick=saved_tick;
            const uint16_t stamp=diagnostics->delta_phase==2?diagnostics->delta_saved_tick:saved_tick;
            if(fa18_frame_delta_write(diagnostics->delta,iteration,game->ticks,(unsigned)boundary,
                                     stamp,trace_bytes,&game->storage)) {
                if(++diagnostics->delta_phase==3) {
                    diagnostics->delta_phase=0;++diagnostics->delta_completed;
                }
            }
        }
    }
    if(diagnostics->capture) native_frame_capture(game,boundary,saved_tick,diagnostics->capture);
    if(diagnostics->trace && !diagnostics->trace->failed && boundary==NATIVE_FRAME_INPUT_BEGIN) {
        /* frontend.c PLANE_BYTES/display.c own the host's 320x200 bitmap;
         * the native runtime does not allocate an Amiga ViewPort structure. */
        if(iteration) fa18_flight_trace_write(diagnostics->trace,iteration,game->ticks,
            320,200,trace_bytes,&game->storage);
    }
}
static void deliver_event(NativeFrontend *game,const HostEvent *event) {
    if(event->kind=='K') native_frontend_event(game,event->a,event->d);
    else if(event->kind=='m') native_frontend_mouse(game,event->b,event->c);
    else native_frontend_button(game,(unsigned)event->b,event->c);
}
static int write_ppm(const char *path,NativeFrontend *game) {
    FILE *file=fopen(path,"wb"); if(!file) return 0;
    fprintf(file,"P6\n320 256\n255\n");
    for(unsigned i=0;i<320*256;++i) { uint16_t c=game->palette[game->indices[i]]; uint8_t rgb[3]={(uint8_t)(((c>>8)&15)*17),(uint8_t)(((c>>4)&15)*17),(uint8_t)((c&15)*17)}; if(fwrite(rgb,1,3,file)!=3) { fclose(file); return 0; } }
    return fclose(file)==0;
}
int main(int argc,char **argv) {
    if(!amiga_sdl_memory_install()) {fputs("Cannot install fixed SDL memory arena before startup\n",stderr);return 1;}
    const char *adf="local/media/fa18.adf",*save_dir="saves-native",*ppm=NULL,*replay=NULL,*data_out=NULL; int headless=0,running=1,result=1;
    unsigned frames=0,iterations=0;
    size_t events=0,next=0,event_capacity=0; HostEvent *host_events=NULL; char error[256];
    const char *input=NULL;NativeReplay loop={0};
    const char *input_anchors=NULL,*input_anchor_report=NULL;
    const char *clock_mode=NULL,*clock_report=NULL;
    HostClock host_clock={0};
    const char *wave=NULL;AmigaPcmOutput audio_output={0};int16_t samples[960*2];
    unsigned audio_rate=48000;
    const char *frame_times=NULL;FILE *timing=NULL;int hidden=0,recorded_input_only=0;
    char timing_buffer[4096];
    const char *memory_report=NULL;AmigaSdlMemoryStats startup_memory={0},gameplay_memory={0};
    SDL_RendererInfo renderer_info={0};uint64_t previous_frame_start=0;
    NativeFrameCapture capture={0};capture.replay=&loop;capture.count=1;
    const char *flight_trace=NULL;FA18FlightTrace trace={0};FrameDiagnostics diagnostics={0};
    const char *delta_path=NULL;FA18FrameDelta *delta=NULL;
    const char *audio_trace_path=NULL;NativeAudioTrace audio_trace={0};
    unsigned capture_budget_mib=512;
    NativeFrontend *game=calloc(1,sizeof *game); SDL_Window *window=NULL; SDL_Renderer *renderer=NULL; SDL_Texture *texture=NULL; uint32_t pixels[320*256];
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"--headless")) headless=1;
        else if(!strcmp(argv[i],"--help")) { puts("fa18_native [--adf PATH] [--save-dir PATH] [--headless --frames N] [--replay E9K] [--input FA18_LOOP_INPUT_V1|FA18_GAME_INPUT_V1 --iterations N] [--input-anchors FILE --input-anchors-out PATH (diagnostics)] [--clock host|pal (window default host; headless default pal)] [--clock-report PATH] [--ppm PATH] [--data-out PATH] [--wav PATH] [--audio-rate 44100|48000 (startup; default 48000)] [--frame-times PATH] [--memory-report PATH] [--hidden (window diagnostics)] [--recorded-input-only (replay diagnostics)] [--frame-capture FIRST[+COUNT] PREFIX] [--frame-capture-entry-only] [--flight-trace PATH] [--frame-delta FIRST[+COUNT] PATH] [--audio-trace PATH] [--capture-budget-mib N (default 512)]"); free(game); return 0; }
        else if(i+1<argc && !strcmp(argv[i],"--adf")) adf=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--save-dir")) save_dir=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--frames")) { char *end; unsigned long n=strtoul(argv[++i],&end,10); if(*end || n>10000000) { fputs("Invalid frame count\n",stderr); goto done; } frames=(unsigned)n; }
        else if(i+1<argc && !strcmp(argv[i],"--ppm")) ppm=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--replay")) replay=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--input")) input=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--input-anchors")) input_anchors=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--input-anchors-out")) input_anchor_report=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--clock")) clock_mode=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--clock-report")) clock_report=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--iterations")) { char *end;unsigned long n=strtoul(argv[++i],&end,10);if(*end || !n || n>10000000) { fputs("Invalid iteration limit\n",stderr);goto done; } iterations=(unsigned)n; }
        else if(i+1<argc && !strcmp(argv[i],"--data-out")) data_out=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--wav")) wave=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--audio-rate")) {
            const char *rate=argv[++i];
            if(!strcmp(rate,"44100")) audio_rate=44100;
            else if(!strcmp(rate,"48000")) audio_rate=48000;
            else {fputs("Audio rate must be 44100 or 48000\n",stderr);goto done;}
        }
        else if(i+1<argc && !strcmp(argv[i],"--frame-times")) frame_times=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--memory-report")) memory_report=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--flight-trace")) flight_trace=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--audio-trace")) audio_trace_path=argv[++i];
        else if(i+2<argc && !strcmp(argv[i],"--frame-delta")) {
            char *end;unsigned long n=strtoul(argv[++i],&end,10),count=1;
            if(*end=='+') count=strtoul(end+1,&end,10);
            if(*end || !n || !count || n>10000000 || count>10000000-n+1) {
                fputs("Invalid frame delta range (FIRST[+COUNT])\n",stderr);goto done;
            }
            diagnostics.delta_first=(unsigned)n;diagnostics.delta_count=(unsigned)count;delta_path=argv[++i];
        }
        else if(!strcmp(argv[i],"--hidden")) hidden=1;
        else if(!strcmp(argv[i],"--recorded-input-only")) recorded_input_only=1;
        else if(!strcmp(argv[i],"--frame-capture-entry-only")) capture.entry_only=1;
        else if(i+1<argc && !strcmp(argv[i],"--capture-budget-mib")) {
            char *end;unsigned long n=strtoul(argv[++i],&end,10);
            if(*end || !n || n>10000000 || n>SIZE_MAX/1024/1024) {fputs("Invalid capture budget\n",stderr);goto done;}
            capture_budget_mib=(unsigned)n;
        }
        else if(i+2<argc && !strcmp(argv[i],"--frame-capture")) {
            char *end;unsigned long n=strtoul(argv[++i],&end,10);
            unsigned long count=1;
            if(*end=='+') count=strtoul(end+1,&end,10);
            if(*end || !n || !count || n>10000000 || count>10000000-n+1) {
                fputs("Invalid frame capture iteration/range (FIRST[+COUNT])\n",stderr);goto done;
            }
            capture.count=(unsigned)count;
            capture.iteration=(unsigned)n;capture.prefix=argv[++i];
        }
        else { fprintf(stderr,"Unknown/incomplete option: %s\n",argv[i]); goto done; }
    }
    if(headless && !frames) { fputs("Headless runs require --frames N\n",stderr); goto done; }
    if(!clock_mode) clock_mode=headless?"pal":"host";
    if(strcmp(clock_mode,"pal") && strcmp(clock_mode,"host")) {
        fputs("Clock must be host (microsecond acquisition) or pal (deterministic diagnostics)\n",stderr);goto done;
    }
    if(hidden && headless) {fputs("Hidden window diagnostics require window presentation\n",stderr);goto done;}
    if(recorded_input_only && !input && !replay) {fputs("Recorded-input-only diagnostics require --input or --replay\n",stderr);goto done;}
    if(iterations && !input) { fputs("Iteration limit requires --input\n",stderr);goto done; }
    if((input_anchors && (!input || !input_anchor_report)) || (input_anchor_report && !input_anchors)) {
        fputs("Input anchors require --input, --input-anchors and --input-anchors-out together\n",stderr);goto done;
    }
    if(capture.prefix && !input) {fputs("Frame capture requires recorded --input\n",stderr);goto done;}
    if(delta_path && (!input || flight_trace || capture.prefix)) {
        fputs("Frame delta requires --input and a separate capture budget from flight/RAM traces\n",stderr);goto done;
    }
    if(audio_trace_path && (flight_trace || capture.prefix || delta_path)) {
        fputs("Audio trace uses its own capture budget; run separately from flight/RAM captures\n",stderr);goto done;
    }
    if(capture.entry_only && !capture.prefix) {fputs("Entry-only capture requires --frame-capture\n",stderr);goto done;}
    if(capture.prefix && capture.count>capture_budget_mib/(capture.entry_only?1u:3u)) {
        fputs("Frame capture exceeds --capture-budget-mib (default 512); use a smaller range or an explicit budget\n",stderr);goto done;
    }
    if(input && !native_replay_load(&loop,input,error,sizeof error)) { fputs(error,stderr);goto done; }
    if(input && !iterations) iterations=loop.end;
    if(input && iterations>loop.end) { fputs("Iteration limit exceeds recorded end\n",stderr);goto done; }
    if(input_anchors) {
        if(iterations!=loop.end) {fputs("Anchored replay requires the complete recorded source end\n",stderr);goto done;}
        if(!native_replay_load_anchors(&loop,input_anchors,error,sizeof error)) {fputs(error,stderr);goto done;}
    }
    if(delta_path && (diagnostics.delta_first>iterations || diagnostics.delta_count>iterations-diagnostics.delta_first+1)) {
        fputs("Frame delta range exceeds the recorded iteration limit\n",stderr);goto done;
    }
    if(replay) {
        FILE *file=fopen(replay,"r"); char line[128];
        if(!file) { fprintf(stderr,"Cannot open replay: %s\n",replay); goto done; }
        if(!fgets(line,sizeof line,file) || strncmp(line,"E9K_INPUT_V1",12)) { fclose(file); fputs("Invalid replay header\n",stderr); goto done; }
        while(fgets(line,sizeof line,file)) {
            HostEvent event;char extra;
            if(sscanf(line,"F %u %c %d %d %d %d %c",&event.frame,&event.kind,
                &event.a,&event.b,&event.c,&event.d,&extra)!=6 ||
                (events && event.frame<host_events[events-1].frame) ||
                (event.kind!='K' && event.kind!='m' && event.kind!='b') ||
                (event.kind=='K' && event.d!=0 && event.d!=1) ||
                (event.kind!='K' && event.a!=0 && event.a!=4) ||
                (event.kind=='b' && ((event.b!=0 && event.b!=1) || (event.c!=0 && event.c!=1)))) {
                fclose(file);fputs("Invalid/unsupported replay row\n",stderr);goto done;
            }
            if(events==event_capacity) {
                if(event_capacity>SIZE_MAX/2/sizeof *host_events) {
                    fclose(file);fputs("Replay event storage exceeds host size limit\n",stderr);goto done;
                }
                const size_t capacity=event_capacity?event_capacity*2:1024;
                HostEvent *grown=realloc(host_events,capacity*sizeof *host_events);
                if(!grown) {
                    fclose(file);fputs("Cannot allocate replay event storage\n",stderr);goto done;
                }
                host_events=grown;event_capacity=capacity;
            }
            host_events[events++]=event;
        }
        const int read_error=ferror(file),close_error=fclose(file);
        if(read_error || close_error) {fputs("Cannot finish reading replay\n",stderr);goto done;}
    }
    if(!game || !native_frontend_open(game,adf,save_dir,error,sizeof error)) { fprintf(stderr,"%s\n",game?error:"Allocation failed"); goto done; }
    if(!native_pcm_filter_begin(&game->audio.output_filter,audio_rate)) {
        fputs("Cannot configure native A500 PCM output filter\n",stderr);goto done;
    }
    if(input) { game->begin_update=native_replay_update;game->update_context=&loop; }
    if(audio_trace_path) {
        if(!native_audio_trace_open(&audio_trace,audio_trace_path,(size_t)capture_budget_mib*1024*1024)) goto done;
        game->audio.observe=native_audio_trace_event;game->audio.observe_context=&audio_trace;
    }
    const size_t reserved_capture=capture.prefix?(size_t)capture.count*(capture.entry_only?1u:3u)*1024*1024:0;
    if(delta_path) {
        delta=calloc(1,sizeof *delta);
        if(!delta || !fa18_frame_delta_open(delta,delta_path,(size_t)capture_budget_mib*1024*1024)) goto done;
        diagnostics.delta=delta;
    }
    if(flight_trace && !fa18_flight_trace_open(&trace,flight_trace,
        (size_t)capture_budget_mib*1024*1024-reserved_capture)) goto done;
    if(capture.prefix || flight_trace || delta_path) {
        diagnostics.capture=capture.prefix?&capture:NULL;diagnostics.replay=input?&loop:NULL;
        diagnostics.trace=flight_trace?&trace:NULL;
        game->observe_frame=observe_frame;game->frame_context=&diagnostics;
    }
    if(!headless) {
        SDL_SetMainReady(); if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER)) goto sdl_error;
        window=SDL_CreateWindow("F/A-18 Interceptor - native intro/menu",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,768,
            SDL_WINDOW_RESIZABLE|(hidden?SDL_WINDOW_HIDDEN:0));
        if(!window) goto sdl_error;
        renderer=SDL_CreateRenderer(window,-1,0); if(!renderer) goto sdl_error;
        if(SDL_GetRendererInfo(renderer,&renderer_info)) goto sdl_error;
        SDL_RenderSetLogicalSize(renderer,320,256); texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,320,256); if(!texture) goto sdl_error;
    }
    const uint64_t frequency=SDL_GetPerformanceFrequency();
    if(!frequency || frequency>UINT64_MAX/1000000u) {fputs("Unsupported host counter frequency\n",stderr);goto done;}
    const double microseconds_per_tick=1000000.0/(double)frequency;
    FA18FramePacer pacer;
    if(!amiga_pcm_open(&audio_output,audio_rate,!headless,wave,error,sizeof error)) {
        fputs(error,stderr);goto done;
    }
    if(frame_times) {
        timing=fopen(frame_times,"w");
        if(!timing) {fprintf(stderr,"Cannot create native frame timing report: %s\n",frame_times);goto done;}
        if(setvbuf(timing,timing_buffer,_IOFBF,sizeof timing_buffer)) goto timing_error;
        if(fputs("frame,iteration,mode,stage,view,scene_updated,presented,input_us,game_us,audio_us,convert_us,present_us,wait_us,work_us,total_us,start_interval_us,renderer\n",timing)==EOF) goto timing_error;
    }
    startup_memory=amiga_sdl_memory_stats();
    if(!headless) {
        /* Populate SDL's reusable render-command/vertex and initial event
         * caches before gameplay. Pending real events remain queued. Flush
         * executes the normal pipeline without presenting an extra frame. */
        /* SDL_events.c: polling also creates its sentinel queue entry.
         * NULL warms that path while preserving every queued real event. */
        SDL_PollEvent(NULL);
        for(unsigned i=0;i<320*256;++i) {uint16_t c=game->palette[game->indices[i]];pixels[i]=0xff000000u|(((c>>8)&15)*17u<<16)|(((c>>4)&15)*17u<<8)|((c&15)*17u);}
        if(SDL_UpdateTexture(texture,NULL,pixels,320*sizeof *pixels) || SDL_RenderClear(renderer) ||
           SDL_RenderCopy(renderer,texture,NULL,NULL) || SDL_RenderFlush(renderer)) goto sdl_error;
        startup_memory=amiga_sdl_memory_stats();
    }
    host_clock=(HostClock){SDL_GetPerformanceCounter(),frequency};
    /* Device callbacks and renderer/event cache initialization are startup.
     * Begin the presentation deadlines with the gameplay clock, so that work
     * cannot create pacing debt before the first game frame. */
    fa18_frame_pacer_init(&pacer,host_clock.origin,frequency);
    native_clock_set_source(!strcmp(clock_mode,"host")?host_microseconds:NULL,&host_clock);
    amiga_runtime_memory_lock(1);
    while(running && (!frames || game->ticks<frames) &&
          (loop.anchor_count?(!loop.anchor_complete && !loop.anchor_failed):(!iterations || loop.iteration<iterations))) {
        uint64_t times[7]={0};int presented=0;
        const unsigned previous_scene=game->scene_frames;
        if(timing) times[0]=SDL_GetPerformanceCounter();
        while(next<events && host_events[next].frame<=game->ticks) { deliver_event(game,&host_events[next]); ++next; }
        if(!headless) {
            SDL_Event event;
            while(SDL_PollEvent(&event))
                if(!native_host_event(game,&event,recorded_input_only)) running=0;
        }
        if(timing) times[1]=SDL_GetPerformanceCounter();
        native_frontend_tick(game);
        if(amiga_runtime_memory_violations()) goto memory_error;
        if(trace.failed) goto done;
        if(delta && delta->failed) goto done;
        if(timing) times[2]=SDL_GetPerformanceCounter();
        /* Both validated rates have an integral 20 ms block. Configure the
         * device/filter once before gameplay; retain the fixed maximum span. */
        const unsigned audio_frames=audio_rate/50;
        native_audio_render(&game->audio,samples,audio_frames,audio_rate);
        if(audio_trace.failed || (audio_trace_path && !native_audio_trace_boundary(&audio_trace,game,loop.iteration))) goto done;
        if(!amiga_pcm_write(&audio_output,samples,audio_frames)) {
            fprintf(stderr,"Cannot publish native PCM audio: %s\n",audio_output.error?audio_output.error:SDL_GetError());goto done;
        }
        if(timing) times[3]=times[4]=times[5]=SDL_GetPerformanceCounter();
        if(!headless) {
            for(unsigned i=0;i<320*256;++i) { uint16_t c=game->palette[game->indices[i]]; pixels[i]=0xff000000u|(((c>>8)&15)*17u<<16)|(((c>>4)&15)*17u<<8)|((c&15)*17u); }
            if(timing) times[4]=SDL_GetPerformanceCounter();
            if(SDL_UpdateTexture(texture,NULL,pixels,320*sizeof *pixels) || SDL_RenderClear(renderer) || SDL_RenderCopy(renderer,texture,NULL,NULL)) goto sdl_error;
            SDL_RenderPresent(renderer);presented=1;
            if(timing) times[5]=SDL_GetPerformanceCounter();
            uint64_t now=SDL_GetPerformanceCounter(),deadline=fa18_frame_pacer_next(&pacer,now);
            if(deadline>now) SDL_Delay((uint32_t)((deadline-now)*1000/frequency));
        }
        if(timing) {
            times[6]=SDL_GetPerformanceCounter();
            if(fprintf(timing,"%u,%u,%u,%06X,%u,%u,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%s\n",
                game->ticks,game->update_iterations,native_menu_selected_mode(game),rd_u32(STAGE_CALLBACK),rd_u8(VIEW_MODE),
                game->scene_frames!=previous_scene,presented,
                (times[1]-times[0])*microseconds_per_tick,(times[2]-times[1])*microseconds_per_tick,
                (times[3]-times[2])*microseconds_per_tick,(times[4]-times[3])*microseconds_per_tick,
                (times[5]-times[4])*microseconds_per_tick,(times[6]-times[5])*microseconds_per_tick,
                (times[5]-times[0])*microseconds_per_tick,(times[6]-times[0])*microseconds_per_tick,
                previous_frame_start?(times[0]-previous_frame_start)*microseconds_per_tick:0.0,
                renderer_info.name?renderer_info.name:"headless")<0) goto timing_error;
            previous_frame_start=times[0];
        }
        if(amiga_runtime_memory_violations()) goto memory_error;
    }
    amiga_runtime_memory_lock(0);gameplay_memory=amiga_sdl_memory_stats();
    if(clock_report) {
        const NativeClockStats stats=native_clock_stats();
        FILE *file=fopen(clock_report,"w");
        if(!file) {fprintf(stderr,"Cannot create clock report: %s\n",clock_report);goto done;}
        const int written=fprintf(file,"{\"clock\":\"%s\",\"counter_frequency\":%llu,\"requests\":%u,\"low_bits_seen\":%u,\"last_microseconds\":%llu}\n",
            clock_mode,(unsigned long long)frequency,stats.requests,stats.low_bits_seen,
            (unsigned long long)stats.last_microseconds)>=0;
        if(fclose(file) || !written) {fprintf(stderr,"Cannot finish clock report: %s\n",clock_report);goto done;}
    }
    if(input_anchor_report && !native_replay_write_anchors(&loop,input_anchor_report)) {
        fprintf(stderr,"Cannot write native input anchor report: %s\n",input_anchor_report);goto done;
    }
    if(ppm && !write_ppm(ppm,game)) { fprintf(stderr,"Cannot write PPM: %s\n",ppm); goto done; }
    if(data_out) {
        FILE *file=fopen(data_out,"wb");
        if(!file) { perror(data_out); goto done; }
        int written=fwrite(game->storage.buffers,1,sizeof game->storage.buffers,file)==sizeof game->storage.buffers
            && fwrite(game->storage.source,1,sizeof game->storage.source,file)==sizeof game->storage.source;
        if(fclose(file) || !written) { fprintf(stderr,"Cannot write native data: %s\n",data_out); goto done; }
    }
    printf("{\"frame_owner_exit\":%s,\"frames\":%u,\"screen\":\"%s\",\"mode\":%u,\"glyphs\":%u,\"record_updates\":%u,\"scene_frames\":%u,\"terrain_polygons\":%u,\"model_calls\":%u,\"hud_frames\":%u,\"control_frames\":%u,\"scene_selected\":%s,\"stage\":\"%06X\",\"game_tick\":%u,\"timer_pending\":%s,\"timer_yields\":%u,\"display_publications\":%u,\"display_yields\":%u,\"display_pending\":%s,\"displayed_page\":%u,\"postflight_callbacks\":%u,\"postflight_resets\":%u,\"input_passes\":%u,\"input_events\":%u,\"input_queued\":%u,\"update_iterations\":%u,\"replay_iterations\":%u,\"replay_events\":%zu,\"host_replay_events\":%zu,\"host_replay_pending\":%zu,\"replay_started\":%s,\"voice_ticks\":%u,\"voice_publications\":%u,\"voice_levels\":[[%d,%d],[%d,%d],[%d,%d],[%d,%d]],\"sample_requests\":%u,\"sample_frames\":%u,\"nonzero_sample_frames\":%u,\"audio_device\":%s,\"frame_capture_complete\":%s,\"frame_before_tick\":%u,\"frame_after_tick\":%u,\"frame_saved_tick\":%u,\"cpu_emulation\":false,\"chipset_emulation\":false}\n",capture.owner_exit?"true":"false",game->ticks,native_frontend_screen(game),native_menu_selected_mode(game),game->glyphs,game->record_updates,game->scene_frames,game->terrain_polygons,game->model_calls,game->hud_frames,game->control_frames,game->scene_selected?"true":"false",rd_u32(STAGE_CALLBACK),rd_u16(UPDATE_TICK),game->flight_timer_pending?"true":"false",game->timer_yields,game->display_publications,game->display_yields,game->display_pending?"true":"false",game->displayed_page,game->postflight_callbacks,game->postflight_resets,game->input_passes,game->input_events,game->input_count,game->update_iterations,loop.iteration,loop.next,next,events-next,loop.started?"true":"false",game->audio.ticks,game->audio.publications,game->audio.channels[0].period,game->audio.channels[0].volume,game->audio.channels[1].period,game->audio.channels[1].volume,game->audio.channels[2].period,game->audio.channels[2].volume,game->audio.channels[3].period,game->audio.channels[3].volume,game->audio.sample_requests,game->audio.sample_frames,game->audio.nonzero_frames,audio_output.device?"true":"false",capture.complete?"true":"false",capture.before_tick,capture.after_tick,capture.saved_tick);
    if(running && loop.anchor_count && !loop.anchor_complete) {
        fprintf(stderr,"Native anchored input incomplete at native update %u, source position %u: %s\n",
            loop.iteration,loop.source_iteration,loop.anchor_failed?"declared event skipped a key or invalid source position":"--frames limit reached before all declared events");goto done;
    }
    if(running && !loop.anchor_count && iterations && loop.iteration<iterations) {
        fprintf(stderr,"Native input stopped at iteration %u of %u: --frames limit reached%s\n",
                loop.iteration,iterations,loop.started?"":" before main-menu anchor");goto done;
    }
    if(capture.prefix && !capture.complete) {
        fprintf(stderr,"Native frame capture %u did not complete (%u/%u captured)%s\n",capture.iteration,
                capture.captured,capture.count,
                capture.begun?" before the run ended":" on a connected flight frame");goto done;
    }
    if(delta_path && (diagnostics.delta_completed!=diagnostics.delta_count || diagnostics.delta_phase)) {
        fprintf(stderr,"Frame delta did not capture every requested body (%u/%u)\n",
                diagnostics.delta_completed,diagnostics.delta_count);goto done;
    }
    result=0; goto done;
timing_error:
    fprintf(stderr,"Cannot write native frame timing report: %s\n",frame_times);
    goto done;
memory_error:
    fputs("Project heap allocation/free rejected during gameplay\n",stderr);
    goto done;
sdl_error:
    fprintf(stderr,"SDL: %s\n",SDL_GetError());
done:
    amiga_runtime_memory_lock(0);
    native_clock_set_source(NULL,NULL);
    if(amiga_runtime_memory_violations()) result=1;
    free(host_events);
    if(!fa18_flight_trace_close(&trace)) result=1;
    if(!fa18_frame_delta_close(delta)) result=1;
    free(delta);
    if(!native_audio_trace_close(&audio_trace)) result=1;
    if(timing && fclose(timing)) {fprintf(stderr,"Cannot finish native frame timing report: %s\n",frame_times);result=1;}
    if(!amiga_pcm_close(&audio_output)) {fputs("Cannot finish native WAV capture\n",stderr);result=1;}
    native_replay_close(&loop);
    SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    if(amiga_sdl_memory_stats().failures) {
        fputs("Fixed SDL memory arena exhausted; no host heap fallback is allowed\n",stderr);result=1;
    }
    if(memory_report) {
        const AmigaSdlMemoryStats final_memory=amiga_sdl_memory_stats();
        FILE *file=fopen(memory_report,"w");
        if(!file) {fprintf(stderr,"Cannot write memory report: %s\n",memory_report);result=1;}
        else {
            int failed=fprintf(file,"{\"sdl_arena_bytes\":%u,\"sdl_startup_bytes\":%zu,\"sdl_peak_bytes\":%zu,\"sdl_gameplay_pool_requests\":%zu,\"sdl_failures\":%zu,\"project_gameplay_heap_violations\":%zu,\"pcm_ring_frames\":%u,\"os_driver_allocations_observed\":false}\n",
                AMIGA_SDL_MEMORY_BYTES,startup_memory.used,final_memory.peak,
                gameplay_memory.requests>=startup_memory.requests?gameplay_memory.requests-startup_memory.requests:0,
                final_memory.failures,amiga_runtime_memory_violations(),AMIGA_PCM_RING_FRAMES)<0;
            if(fclose(file) || failed) result=1;
        }
    }
    if(game) { native_frontend_close(game); free(game); } return result;
}
