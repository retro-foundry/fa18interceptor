/* Shared headless/window runner. The reference target restores a UAE state;
 * FA18_ROMFREE_MAIN loads the original ADF with an explicit process handoff. */
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "m68k.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "input.h"
#include "recomp_ports.h"
#include "loop_input.h"
#include "../os/rom_audit_adapter.h"
#include "../os/exec_task_services_adapter.h"
#include "../os/exec_supervisor.h"
#include "../os/exec_memory_adapter.h"
#include "../os/exec_scheduler_adapter.h"
#include "../os/exec_interrupt_adapter.h"
#ifdef FA18_ROMFREE_MAIN
#include "../romfree/profile.h"
#include "../romfree/media.h"
#include "../os/host_compat_adapter.h"
#endif

#ifndef FA18_ROMFREE_MAIN
static uint8_t *read_file(const char *path, size_t *size) {
    FILE *f = fopen(path, "rb");
    uint8_t *data;
    long n;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = malloc((size_t)n);
    if (data && fread(data, 1, (size_t)n, f) != (size_t)n) { free(data); data = NULL; }
    fclose(f);
    *size = (size_t)n;
    return data;
}
#endif

static int write_ppm(const char *path, const uint16_t *pixels) {
    FILE *f = fopen(path, "wb");
    int i;
    if (!f) return 0;
    fprintf(f, "P6\n%d %d\n255\n", FA18_SCREEN_W, FA18_SCREEN_H);
    for (i = 0; i < FA18_SCREEN_W * FA18_SCREEN_H; i++) {
        uint16_t v = pixels[i];
        unsigned char rgb[3] = {(unsigned char)(((v >> 8) & 15) * 17), (unsigned char)(((v >> 4) & 15) * 17),
                                (unsigned char)((v & 15) * 17)};
        fwrite(rgb, 1, 3, f);
    }
    fclose(f);
    return 1;
}

static void usage(void) {
    fprintf(stderr,
#ifdef FA18_ROMFREE_MAIN
            "usage: fa18_romfree [--adf PATH] [--save-dir PATH] [--window [--scale N] [--vsync on|off]]\n"
            "                     (without --adf: discover a supported ADF beside the executable)\n"
            "                     [--identify-adf PATH] (print disk/executable SHA-256 and compatibility)\n"
            "                     [--frames N] [--ppm OUT.ppm] [--ppm-every DIR] [--rgb444 OUT.bin]\n"
            "                     [--record OUT.fa18in] [--input IN.fa18in [--to-end]]\n"
            "                     [--replay RUN.e9k --start-frame N] [--no-recomp] [--ram-out OUT.bin]\n"
            "                     [--ports off|on|shadow|sandbox] [--ports-only LIST] [--ports-report OUT.json]\n"
            "                     [--profile OUT.json] [--edges OUT.json] [--poison] [--frame-times OUT.csv]\n"
            "                     [--fast-forward N] (window: run initial frames without real-time waits)\n"
            "                     [--rom-audit OUT.json] [--fallback-log OUT.json]\n"
            "ADF-only menu/demo and flight-log save/reload verified; full mission/teardown acceptance pending.\n"
#else
            "usage: fa18_recomp --state STATE.bin --rom KICK13.rom [--frames N] [--ppm OUT.ppm]\n"
            "                   [--ppm-every DIR] [--rgb444 OUT.bin] [--no-recomp] [--fallback-log OUT.json]\n"
            "                   [--ram-out OUT.bin] [--replay RUN.e9k --start-frame N]\n"
            "                   [--window [--scale N] [--vsync on|off]]   (window: --frames 0 runs until closed)\n"
            "                   [--ports off|on|shadow|sandbox] [--ports-only LIST] [--ports-report OUT.json]\n"
            "                   [--profile OUT.json] [--edges OUT.json] [--poison] [--frame-times OUT.csv]\n"
            "                   [--fast-forward N] (window: run initial frames without real-time waits)\n"
            "                   [--rom-transitions OUT.json] (RAM-to-ROM entry inventory)\n"
            "                   [--rom-audit OUT.json] (ROM instructions, nested edges, data and vectors)\n"
            "                   [--no-os-vbeam] (use the ROM VBeamPos)\n"
            "                   [--no-os-waitblit] (use ROM graphics.library WaitBlit)\n"
            "                   [--no-os-waitbovp] (use ROM graphics.library WaitBOVP)\n"
            "                   [--no-os-blitter-owner] (use ROM OwnBlitter/DisownBlitter)\n"
            "                   [--no-os-exec-interrupts] (use ROM Exec Disable/Enable)\n"
            "                   [--no-os-getmsg] (use ROM Exec GetMsg)\n"
            "                   [--no-os-potgo] (use ROM potgo.resource WritePotgo)\n"
            "                   [--no-os-task-lookup] (use ROM Exec FindTask/FindName)\n"
            "                   [--no-os-lists] (use ROM Exec list services)\n"
            "                   [--no-os-task-services] (use ROM Exec messages/signals/task protection)\n"
            "                   [--no-os-supervisor] (use ROM Exec Supervisor/privilege callback)\n"
            "                   [--no-os-scheduler] (use ROM Exec task switching and callbacks)\n"
            "                   [--no-os-irq-services] (use ROM Exec IRQ roots, vectors, servers and Cause)\n"
            "                   [--no-os-memory] (use ROM Exec memory-list services)\n"
            "                   [--record OUT.fa18in] (with --window)  [--input IN.fa18in [--to-end]]\n"
#endif
            );
}

/* Engine9000 recordings start from a UAE restore, and UAE's first frame
 * after a restore runs two frames of machine time (two vertical blanks)
 * before the second frame's input is read. Replays reproduce that; a state
 * written mid-session by the bridge (no restore in the reference) does not
 * need it: pass --no-restore-lead. */
static int restore_lead =
#ifdef FA18_ROMFREE_MAIN
    0;
#else
    -1;
#endif

#ifdef FA18_WITH_SDL
#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include "frame_pacer.h"

/* Live 50 Hz window. Keys go to the Amiga keyboard; clicking the window
 * captures the mouse, F12 releases it. Recorded replay events still apply. */
static int run_window(FA18Machine *m, FA18Replay *replay, int start_frame, int frames, int scale, int vsync,
                      int *completed_frames,const char *frame_times_path,int fast_forward) {
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    static uint32_t argb[FA18_SCREEN_W * FA18_SCREEN_H];
    FA18FramePacer pacer;
    uint64_t frequency;
    int running = 1, frame = 0, grabbed = 0, result = 0;
    FILE *timing=NULL;
    double microseconds_per_tick=0;
    static uint16_t previous_screen[FA18_SCREEN_W * FA18_SCREEN_H];
    *completed_frames=0;
    /* Explicit host presentation option. Execute every original frame and
     * replay event; only omit window presentation and host pacing here. */
    for (;frame<fast_forward
#ifdef FA18_ROMFREE_MAIN
           && !fa18_os_host_exited()
#endif
         ;++frame) {
        fa18_replay_apply(replay,m,start_frame+frame+1);
        if (frame==0 && restore_lead) { fa18_machine_run_frame(m); fa18_loop_frame(); }
        fa18_machine_run_frame(m); fa18_loop_frame();
    }
    *completed_frames=frame;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    win = SDL_CreateWindow("F/A-18 Interceptor (translated)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           FA18_SCREEN_W * scale, FA18_SCREEN_H * scale, SDL_WINDOW_RESIZABLE);
    ren = win ? SDL_CreateRenderer(win, -1, vsync ? SDL_RENDERER_PRESENTVSYNC : 0) : NULL;
    tex = ren ? SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FA18_SCREEN_W,
                                  FA18_SCREEN_H) : NULL;
    if (!tex) {
        fprintf(stderr, "SDL display creation failed: %s\n", SDL_GetError());
        if (ren) SDL_DestroyRenderer(ren);
        if (win) SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetLogicalSize(ren, FA18_SCREEN_W, FA18_SCREEN_H);
    if (frame_times_path) {
        timing=fopen(frame_times_path,"w");
        if (!timing) { fprintf(stderr,"cannot write %s\n",frame_times_path); result=1; goto window_cleanup; }
        microseconds_per_tick=1000000.0/(double)SDL_GetPerformanceFrequency();
        memcpy(previous_screen,m->last_screen,sizeof previous_screen);
        fprintf(timing,"frame,input_us,simulation_us,convert_us,present_us,wait_us,total_us,screen_changed\n");
    }
    frequency = SDL_GetPerformanceFrequency();
    fa18_frame_pacer_init(&pacer, SDL_GetPerformanceCounter(), frequency);
    while (running && (frames <= 0 || frame < frames)
#ifdef FA18_ROMFREE_MAIN
           && !fa18_os_host_exited()
#endif
    ) {
        SDL_Event e;
        int p;
        uint64_t ticks[6]; int screen_changed=0;
        if (timing) ticks[0]=SDL_GetPerformanceCounter();
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT: running = 0; break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (e.key.keysym.sym == SDLK_F12) {
                    if (e.type == SDL_KEYDOWN) { grabbed = 0; SDL_SetRelativeMouseMode(SDL_FALSE); }
                } else if (!e.key.repeat) {
                    int raw = fa18_amiga_rawkey(e.key.keysym.sym);
                    if (raw >= 0 && fa18_loop_recording()) fa18_loop_host_key(raw, e.type == SDL_KEYDOWN);
                    else if (raw >= 0) fa18_machine_key(m, raw, e.type == SDL_KEYDOWN);
                }
                break;
            case SDL_MOUSEMOTION:
                if (grabbed && fa18_loop_recording()) fa18_loop_host_mouse(e.motion.xrel, e.motion.yrel);
                else if (grabbed) fa18_machine_mouse(m, e.motion.xrel, e.motion.yrel);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (!grabbed && e.type == SDL_MOUSEBUTTONDOWN) {
                    grabbed = 1;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                } else if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
                    int button = e.button.button == SDL_BUTTON_LEFT ? 0 : 1;
                    if (fa18_loop_recording()) fa18_loop_host_button(button, e.type == SDL_MOUSEBUTTONDOWN);
                    else fa18_machine_button(m, button, e.type == SDL_MOUSEBUTTONDOWN);
                }
                break;
            default: break;
            }
        }
        if (!running) break;
        fa18_replay_apply(replay, m, start_frame + frame + 1);
        if (timing) ticks[1]=SDL_GetPerformanceCounter();
        if (frame == 0 && restore_lead) { fa18_machine_run_frame(m); fa18_loop_frame(); }
        fa18_machine_run_frame(m);
        fa18_loop_frame();
        frame++;
        if (timing) ticks[2]=SDL_GetPerformanceCounter();
        for (p = 0; p < FA18_SCREEN_W * FA18_SCREEN_H; p++) {
            uint16_t v = m->last_screen[p];
            argb[p] = 0xFF000000u | (uint32_t)((v >> 8) & 15) * 0x110000u | (uint32_t)((v >> 4) & 15) * 0x1100u |
                      (uint32_t)(v & 15) * 0x11u;
        }
        if (timing) {
            ticks[3]=SDL_GetPerformanceCounter();
            screen_changed=memcmp(previous_screen,m->last_screen,sizeof previous_screen)!=0;
            memcpy(previous_screen,m->last_screen,sizeof previous_screen);
        }
        SDL_UpdateTexture(tex, NULL, argb, FA18_SCREEN_W * (int)sizeof argb[0]);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        if (timing) ticks[4]=SDL_GetPerformanceCounter();
        {
            uint64_t now = SDL_GetPerformanceCounter();
            uint64_t deadline = fa18_frame_pacer_next(&pacer, now);
            while (now < deadline) {
                /* Sleep through the bulk of the wait, then use the precise
                 * clock for the final millisecond rather than oversleeping. */
                if (deadline - now > frequency / 1000) SDL_Delay(1);
                now = SDL_GetPerformanceCounter();
            }
        }
        if (timing) {
            ticks[5]=SDL_GetPerformanceCounter();
            if (fprintf(timing,"%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d\n",frame,
                (ticks[1]-ticks[0])*microseconds_per_tick,(ticks[2]-ticks[1])*microseconds_per_tick,
                (ticks[3]-ticks[2])*microseconds_per_tick,(ticks[4]-ticks[3])*microseconds_per_tick,
                (ticks[5]-ticks[4])*microseconds_per_tick,(ticks[5]-ticks[0])*microseconds_per_tick,
                screen_changed)<0) { fprintf(stderr,"cannot write %s\n",frame_times_path); result=1; break; }
        }
    }
window_cleanup:
    if (timing && fclose(timing)) { fprintf(stderr,"cannot close %s\n",frame_times_path); result=1; }
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    *completed_frames=frame;
    return result;
}
#endif

int main(int argc, char **argv) {
#ifdef FA18_ROMFREE_MAIN
    const char *adf_path=NULL,*identify_adf=NULL,*save_directory="local/saves";
    char discovered_adf[4096];
    FA18RomFreeProfile clean_profile={0};
#else
    const char *state_path = NULL, *rom_path = NULL;
#endif
    const char *ppm = NULL, *ppm_dir = NULL, *rgb_path = NULL,
               *fallback = NULL, *ram_out = NULL;
    const char *record_path = NULL, *input_path = NULL;
    int to_end = 0;
    const char *replay_path = NULL, *ports_only = NULL, *ports_report = NULL, *profile_path = NULL, *edges_path = NULL;
    const char *rom_transitions_path = NULL;
    const char *rom_audit_path = NULL;
#ifndef FA18_ROMFREE_MAIN
    int os_vbeam = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_waitblit = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_waitbovp = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_blitter_owner = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_exec_interrupts = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_getmsg = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_potgo = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_task_lookup = -1;
    int os_lists = -1;
    int os_task_services = -1;
    int os_supervisor = -1;
    int os_memory = -1;
    int os_scheduler = -1;
    int os_irq_services = -1;
#endif
    FA18PortMode ports_mode = FA18_PORTS_OFF;
    int frames = 10, use_recomp = 1, i, start_frame = 0, window = 0, scale = 3;
    int run_result=0;
    int fast_forward=0;
    const char *frame_times_path=NULL;
    /* Host-paced presentation avoids coupling PAL frames to a second clock.
     * Keep the reference runner's presentation default for existing users. */
    int vsync =
#ifdef FA18_ROMFREE_MAIN
        0;
#else
        1;
#endif
    FA18Replay replay = {0};
#ifndef FA18_ROMFREE_MAIN
    size_t state_size, rom_size;
    uint8_t *state, *rom;
#endif
    char error[256];
    FA18Machine *m;
    FILE *rgb = NULL;
    uint64_t rgb_bytes = 0, rgb_limit = 4ull << 30;
    const char *rgb_limit_text = getenv("FA18_RGB444_MAX_MIB");
    char *rgb_limit_end;

    if (rgb_limit_text) {
        unsigned long long mib = strtoull(rgb_limit_text, &rgb_limit_end, 10);
        if (!*rgb_limit_text || *rgb_limit_end || mib > (0xFFFFFFFFFFFFFFFFull >> 20)) {
            fprintf(stderr, "FA18_RGB444_MAX_MIB must be a nonnegative integer\n");
            return 2;
        }
        rgb_limit = (uint64_t)mib << 20;
    }

    for (i = 1; i < argc; i++) {
#ifdef FA18_ROMFREE_MAIN
        if (!strcmp(argv[i], "--adf") && i + 1 < argc) adf_path = argv[++i];
        else if (!strcmp(argv[i], "--identify-adf") && i + 1 < argc) identify_adf = argv[++i];
        else if (!strcmp(argv[i], "--save-dir") && i + 1 < argc) save_directory = argv[++i];
#else
        if (!strcmp(argv[i], "--state") && i + 1 < argc) state_path = argv[++i];
        else if (!strcmp(argv[i], "--rom") && i + 1 < argc) rom_path = argv[++i];
#endif
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--ppm") && i + 1 < argc) ppm = argv[++i];
        else if (!strcmp(argv[i], "--ppm-every") && i + 1 < argc) ppm_dir = argv[++i];
        else if (!strcmp(argv[i], "--rgb444") && i + 1 < argc) rgb_path = argv[++i];
        else if (!strcmp(argv[i], "--fallback-log") && i + 1 < argc) fallback = argv[++i];
        else if (!strcmp(argv[i], "--ram-out") && i + 1 < argc) ram_out = argv[++i];
        else if (!strcmp(argv[i], "--no-recomp")) use_recomp = 0;
        else if (!strcmp(argv[i], "--replay") && i + 1 < argc) replay_path = argv[++i];
        else if (!strcmp(argv[i], "--start-frame") && i + 1 < argc) start_frame = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--no-restore-lead")) restore_lead = 0;
        else if (!strcmp(argv[i], "--window")) window = 1;
        else if (!strcmp(argv[i], "--vsync") && i + 1 < argc) {
            const char *value = argv[++i];
            if (strcmp(value, "on") && strcmp(value, "off")) {
                fprintf(stderr, "--vsync requires on or off\n");
                return 2;
            }
            vsync = !strcmp(value, "on");
        }
        else if (!strcmp(argv[i], "--ports") && i + 1 < argc) {
            const char *v = argv[++i];
            ports_mode = !strcmp(v, "on") ? FA18_PORTS_ON : !strcmp(v, "shadow") ? FA18_PORTS_SHADOW
                       : !strcmp(v, "sandbox") ? FA18_PORTS_SANDBOX : FA18_PORTS_OFF;
        }
        else if (!strcmp(argv[i], "--ports-only") && i + 1 < argc) ports_only = argv[++i];
        else if (!strcmp(argv[i], "--ports-report") && i + 1 < argc) ports_report = argv[++i];
        else if (!strcmp(argv[i], "--profile") && i + 1 < argc) profile_path = argv[++i];
        else if (!strcmp(argv[i], "--frame-times") && i + 1 < argc) frame_times_path = argv[++i];
        else if (!strcmp(argv[i], "--fast-forward") && i + 1 < argc) {
            char *end; const char *value=argv[++i]; long count=strtol(value,&end,10);
            if (!*value || *end || count<0 || count>INT_MAX) {
                fprintf(stderr,"--fast-forward requires a nonnegative frame count\n"); return 2;
            }
            fast_forward=(int)count;
        }
        else if (!strcmp(argv[i], "--edges") && i + 1 < argc) edges_path = argv[++i];
        else if (!strcmp(argv[i], "--rom-transitions") && i + 1 < argc) rom_transitions_path = argv[++i];
        else if (!strcmp(argv[i], "--rom-audit") && i + 1 < argc) rom_audit_path = argv[++i];
#ifndef FA18_ROMFREE_MAIN
        else if (!strcmp(argv[i], "--os-vbeam")) os_vbeam = 1;
        else if (!strcmp(argv[i], "--no-os-vbeam")) os_vbeam = 0;
        else if (!strcmp(argv[i], "--os-waitblit")) os_waitblit = 1;
        else if (!strcmp(argv[i], "--no-os-waitblit")) os_waitblit = 0;
        else if (!strcmp(argv[i], "--os-waitbovp")) os_waitbovp = 1;
        else if (!strcmp(argv[i], "--no-os-waitbovp")) os_waitbovp = 0;
        else if (!strcmp(argv[i], "--os-blitter-owner")) os_blitter_owner = 1;
        else if (!strcmp(argv[i], "--no-os-blitter-owner")) os_blitter_owner = 0;
        else if (!strcmp(argv[i], "--os-exec-interrupts")) os_exec_interrupts = 1;
        else if (!strcmp(argv[i], "--no-os-exec-interrupts")) os_exec_interrupts = 0;
        else if (!strcmp(argv[i], "--os-getmsg")) os_getmsg = 1;
        else if (!strcmp(argv[i], "--no-os-getmsg")) os_getmsg = 0;
        else if (!strcmp(argv[i], "--os-potgo")) os_potgo = 1;
        else if (!strcmp(argv[i], "--no-os-potgo")) os_potgo = 0;
        else if (!strcmp(argv[i], "--os-task-lookup")) os_task_lookup = 1;
        else if (!strcmp(argv[i], "--no-os-task-lookup")) os_task_lookup = 0;
        else if (!strcmp(argv[i], "--os-lists")) os_lists = 1;
        else if (!strcmp(argv[i], "--no-os-lists")) os_lists = 0;
        else if (!strcmp(argv[i], "--os-task-services")) os_task_services = 1;
        else if (!strcmp(argv[i], "--no-os-task-services")) os_task_services = 0;
        else if (!strcmp(argv[i], "--os-supervisor")) os_supervisor = 1;
        else if (!strcmp(argv[i], "--no-os-supervisor")) os_supervisor = 0;
        else if (!strcmp(argv[i], "--os-scheduler")) os_scheduler = 1;
        else if (!strcmp(argv[i], "--no-os-scheduler")) os_scheduler = 0;
        else if (!strcmp(argv[i], "--os-irq-services")) os_irq_services = 1;
        else if (!strcmp(argv[i], "--no-os-irq-services")) os_irq_services = 0;
        else if (!strcmp(argv[i], "--os-memory")) os_memory = 1;
        else if (!strcmp(argv[i], "--no-os-memory")) os_memory = 0;
#endif
        else if (!strcmp(argv[i], "--poison")) fa18_ports_set_poison(1);
        else if (!strcmp(argv[i], "--scale") && i + 1 < argc) scale = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--record") && i + 1 < argc) record_path = argv[++i];
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) input_path = argv[++i];
        else if (!strcmp(argv[i], "--to-end")) to_end = 1;
        else if (!strcmp(argv[i], "--help")) { usage(); return 0; }
        else { usage(); return 2; }
    }
    if (frame_times_path && !window) {
        fprintf(stderr,"--frame-times requires --window\n"); return 2;
    }
    if (fast_forward && (!window || (frames>0 && fast_forward>=frames))) {
        fprintf(stderr,"--fast-forward requires --window and must be less than a finite --frames limit\n"); return 2;
    }
#ifdef FA18_ROMFREE_MAIN
    if (identify_adf) {
        FA18MediaInfo info;
        if (!fa18_media_probe(identify_adf,&info)) { fprintf(stderr,"cannot read ADF %s\n",identify_adf); return 1; }
        printf("{\"disk_sha256\":\"%s\",\"executable_sha256\":\"%s\",\"version\":\"%s\",\"ofs\":%s,\"supported\":%s}\n",
               info.disk_sha256,info.executable_sha256,info.version,info.ofs?"true":"false",info.supported?"true":"false");
        return 0;
    }
    if (!*save_directory) { usage(); return 2; }
    if (!adf_path) {
        if (!fa18_media_discover(argv[0],discovered_adf,sizeof discovered_adf,error,sizeof error)) {
            fprintf(stderr,"%s\n",error); return 1;
        }
        adf_path=discovered_adf;
    }
    m=calloc(1,sizeof *m);
    if (!m || !fa18_romfree_load(&clean_profile,m,adf_path,save_directory,use_recomp,error,sizeof error)) {
        fprintf(stderr,"%s\n",m?error:"cannot allocate machine"); free(m); return 1;
    }
#else
    if (!state_path || !rom_path) { usage(); return 2; }
    state = read_file(state_path, &state_size);
    rom = read_file(rom_path, &rom_size);
    if (!state || !rom) { fprintf(stderr, "cannot read state or ROM\n"); return 1; }
    m = calloc(1, sizeof *m);
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    if (os_vbeam < 0) os_vbeam = use_recomp;
    if (os_waitblit < 0) os_waitblit = use_recomp;
    if (os_waitbovp < 0) os_waitbovp = use_recomp;
    if (os_blitter_owner < 0) os_blitter_owner = use_recomp;
    if (os_exec_interrupts < 0) os_exec_interrupts = use_recomp;
    if (os_getmsg < 0) os_getmsg = use_recomp;
    if (os_potgo < 0) os_potgo = use_recomp;
    if (os_task_lookup < 0) os_task_lookup = use_recomp;
    if (os_lists < 0) os_lists = use_recomp;
    if (os_task_services < 0) os_task_services = use_recomp;
    if (os_supervisor < 0) os_supervisor = use_recomp;
    if (os_memory < 0) os_memory = use_recomp;
    if (os_scheduler < 0) os_scheduler = use_recomp;
    if (os_irq_services < 0) os_irq_services = use_recomp;
    fa18_recomp_init(use_recomp);
    if (os_vbeam) fa18_recomp_enable_vbeam_shim();
    if (os_waitblit) fa18_recomp_enable_wait_blit_shim();
    if (os_waitbovp) fa18_recomp_enable_wait_bovp_shim();
    if (os_blitter_owner) fa18_recomp_enable_blitter_ownership_shim();
    if (os_exec_interrupts) fa18_recomp_enable_exec_interrupt_shim();
    if (os_getmsg) fa18_recomp_enable_exec_get_msg_shim();
    if (os_potgo) fa18_recomp_enable_potgo_shim();
    if (os_task_lookup) fa18_recomp_enable_exec_task_lookup_shim();
    if (os_lists) fa18_recomp_enable_exec_lists_shim();
    if (os_task_services) fa18_os_exec_task_services_enable_reference();
    if (os_supervisor) fa18_os_exec_supervisor_enable_reference();
    if (os_memory) fa18_os_exec_memory_enable_reference();
    if (os_scheduler) fa18_os_exec_scheduler_enable_reference();
    if (os_irq_services) fa18_os_exec_interrupt_services_enable_reference();
#endif
    if (rom_transitions_path && !fa18_recomp_track_rom_transitions()) {
        fprintf(stderr, "cannot allocate ROM transition inventory\n");
        return 1;
    }
    fa18_ports_init(ports_mode, ports_only);
    if (rom_audit_path && (ports_mode != FA18_PORTS_OFF || !fa18_rom_audit_open())) {
        fprintf(stderr,"--rom-audit requires --ports off and an allocated inventory\n");
        return 2;
    }
    if (restore_lead < 0) restore_lead = replay_path != NULL && start_frame == 0;
    if (replay_path && !fa18_replay_load(&replay, replay_path)) {
        fprintf(stderr, "cannot read E9K_INPUT_V1 replay %s\n", replay_path);
        return 1;
    }
    if ((record_path || input_path) && !use_recomp) {
        fprintf(stderr, "--record and --input count main-loop iterations in translated code; drop --no-recomp\n");
        return 2;
    }
    if (record_path && (input_path || replay_path || !window)) {
        fprintf(stderr, "--record needs --window and no --input or --replay\n");
        return 2;
    }
    if (record_path) {
        FILE *out = fopen(record_path, "w");
        if (!out) { fprintf(stderr, "cannot write %s\n", record_path); return 1; }
        fa18_loop_record(out);
    }
    if (input_path && !fa18_loop_replay(input_path)) {
        fprintf(stderr, "cannot read FA18_LOOP_INPUT_V1 input %s\n", input_path);
        return 1;
    }
    fa18_meter_start(profile_path != NULL);
    if (window) {
#ifdef FA18_WITH_SDL
        run_result = run_window(m, &replay, start_frame, frames, scale, vsync, &i,frame_times_path,fast_forward);
#else
        fprintf(stderr, "--window needs the CMake build (SDL2)\n");
        return 2;
#endif
    } else {
    if (rgb_path && !(rgb = fopen(rgb_path, "wb"))) { fprintf(stderr, "cannot write %s\n", rgb_path); return 1; }
    for (i = 0; (to_end ? fa18_loop_iterations() < fa18_loop_replay_end() : i < frames)
#ifdef FA18_ROMFREE_MAIN
         && !fa18_os_host_exited()
#endif
         ; i++) {
        /* Events recorded for a frame are delivered before that frame runs. */
        fa18_replay_apply(&replay, m, start_frame + i + 1);
        if (i == 0 && restore_lead) { fa18_machine_run_frame(m); fa18_loop_frame(); }
        fa18_machine_run_frame(m);
        fa18_loop_frame();
        if (rgb) {
            size_t pixels = FA18_SCREEN_W * FA18_SCREEN_H;
            size_t written = fwrite(m->last_screen, sizeof m->last_screen[0], pixels, rgb);
            rgb_bytes += written * sizeof m->last_screen[0];
            if (written != pixels || (rgb_limit && rgb_bytes > rgb_limit)) {
                if (written == pixels)
                    fprintf(stderr, "RGB444 output exceeded %llu MiB; set FA18_RGB444_MAX_MIB=0 for unlimited output\n",
                            (unsigned long long)(rgb_limit >> 20));
                else fprintf(stderr, "cannot write RGB444 output %s\n", rgb_path);
                fclose(rgb);
                rgb = NULL;
                remove(rgb_path);
                fa18_bus_trace_close();
                return 1;
            }
        }
        if (ppm_dir) {
            char path[512];
            snprintf(path, sizeof path, "%s/frame_%03d.ppm", ppm_dir, i + 1);
            write_ppm(path, m->last_screen);
        }
    }
    if (rgb) fclose(rgb);
    }
    if (!fa18_bus_trace_close()) return 1;
    if (ppm && !write_ppm(ppm, m->last_screen)) { fprintf(stderr, "cannot write %s\n", ppm); return 1; }
    if (fallback) fa18_recomp_write_fallback_log(fallback);
    if (rom_audit_path && !fa18_rom_audit_finish(rom_audit_path)) {
        fprintf(stderr,"cannot finish ROM dependency inventory %s\n",rom_audit_path);
        return 1;
    }
    if (profile_path && !fa18_recomp_write_profile(profile_path)) {
        fprintf(stderr, "cannot write profile %s\n", profile_path); return 1;
    }
    if (edges_path) fa18_recomp_write_edges(edges_path);
    if (rom_transitions_path && !fa18_recomp_write_rom_transitions(rom_transitions_path)) {
        fprintf(stderr, "cannot write ROM transition inventory %s\n", rom_transitions_path);
        return 1;
    }
    if (ports_mode == FA18_PORTS_SHADOW || ports_mode == FA18_PORTS_SANDBOX || ports_report) {
        long bad = fa18_ports_report(ports_report);
        fprintf(stderr, "ports: %ld mismatching calls\n", bad);
        if (bad) return 3;
    }
    if (ram_out) {
        /* Chip RAM, Slow RAM, then D0-D7/A0-A7/SR/PC as big-endian longs. */
        FILE *f = fopen(ram_out, "wb");
        int r;
        if (!f) { fprintf(stderr, "cannot write %s\n", ram_out); return 1; }
        fwrite(m->chip, 1, FA18_CHIP_SIZE, f);
        fwrite(m->slow, 1, FA18_SLOW_SIZE, f);
        for (r = 0; r < 18; r++) {
            unsigned v = m68k_get_reg(NULL, (m68k_register_t)(r < 16 ? M68K_REG_D0 + r : r == 16 ? M68K_REG_SR : M68K_REG_PC));
            unsigned char b[4] = {(unsigned char)(v >> 24), (unsigned char)(v >> 16), (unsigned char)(v >> 8), (unsigned char)v};
            fwrite(b, 1, 4, f);
        }
        fclose(f);
    }
    {
        uint64_t total = m->cycle;
        uint64_t gen = fa18_recomp_stats.generated_cycles;
        int nonblack = 0, p;
        for (p = 0; p < FA18_SCREEN_W * FA18_SCREEN_H; p++) nonblack += m->last_screen[p] != 0;
        printf("{\"frames\": %d, \"recomp\": %d, \"cpu_cycles\": %llu, \"generated_cycles\": %llu, "
               "\"generated_share\": %.4f, \"dispatches\": %llu, \"interpreted_game_instructions\": %llu, "
               "\"code_writes\": %llu, \"disabled_functions\": %d, \"blits\": %llu, \"line_blits\": %llu, "
               "\"nonblack_pixels\": %d, \"pc\": \"%06X\", \"iterations\": %ld}\n",
               i, use_recomp, (unsigned long long)total, (unsigned long long)gen,
               total ? (double)gen / (double)total : 0.0, (unsigned long long)fa18_recomp_stats.dispatches,
               (unsigned long long)fa18_recomp_stats.interpreted_game,
               (unsigned long long)fa18_recomp_stats.code_writes, fa18_recomp_stats.disabled_functions,
               (unsigned long long)m->blits, (unsigned long long)m->line_blits, nonblack,
               m68k_get_reg(NULL, M68K_REG_PC), fa18_loop_iterations());
    }
#ifdef FA18_ROMFREE_MAIN
    printf("{\"rom_reads\": %llu, \"rom_instruction_fetches\": %llu, \"unsupported_services\": %llu}\n",
        (unsigned long long)m->runtime_guard.rom_reads,
        (unsigned long long)m->runtime_guard.rom_instruction_fetches,
        (unsigned long long)m->runtime_guard.unsupported_services);
    if (!run_result && clean_profile.compat && clean_profile.compat->exited)
        run_result=clean_profile.compat->exit_code;
    if (!fa18_romfree_close(&clean_profile)) {
        fprintf(stderr,"cannot flush or close a save file during shutdown\n");
        if (!run_result) run_result=1;
    }
#endif
    fa18_loop_finish();
    fa18_replay_free(&replay);
    free(m);
    return run_result;
}
