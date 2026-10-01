/* fa18_recomp: run the translated game from a UAE savestate on the native
 * machine layer and write the resulting frames. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68k.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "input.h"
#include "recomp_ports.h"
#include "loop_input.h"

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
            "usage: fa18_recomp --state STATE.bin --rom KICK13.rom [--frames N] [--ppm OUT.ppm]\n"
            "                   [--ppm-every DIR] [--rgb444 OUT.bin] [--no-recomp] [--fallback-log OUT.json]\n"
            "                   [--ram-out OUT.bin] [--replay RUN.e9k --start-frame N]\n"
            "                   [--window [--scale N]]   (window: --frames 0 runs until closed)\n"
            "                   [--ports off|on|shadow|sandbox] [--ports-only LIST] [--ports-report OUT.json]\n"
            "                   [--profile OUT.json] [--edges OUT.json] [--poison]\n"
            "                   [--rom-transitions OUT.json] (RAM-to-ROM entry inventory)\n"
            "                   [--no-os-vbeam] (use the ROM VBeamPos)\n"
            "                   [--no-os-waitblit] (use ROM graphics.library WaitBlit)\n"
            "                   [--no-os-waitbovp] (use ROM graphics.library WaitBOVP)\n"
            "                   [--no-os-blitter-owner] (use ROM OwnBlitter/DisownBlitter)\n"
            "                   [--no-os-exec-interrupts] (use ROM Exec Disable/Enable)\n"
            "                   [--no-os-getmsg] (use ROM Exec GetMsg)\n"
            "                   [--no-os-potgo] (use ROM potgo.resource WritePotgo)\n"
            "                   [--record OUT.fa18in] (with --window)  [--input IN.fa18in [--to-end]]\n");
}

/* Engine9000 recordings start from a UAE restore, and UAE's first frame
 * after a restore runs two frames of machine time (two vertical blanks)
 * before the second frame's input is read. Replays reproduce that; a state
 * written mid-session by the bridge (no restore in the reference) does not
 * need it: pass --no-restore-lead. */
static int restore_lead = -1;

#ifdef FA18_WITH_SDL
#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>

/* Live 50 Hz window. Keys go to the Amiga keyboard; clicking the window
 * captures the mouse, F12 releases it. Recorded replay events still apply. */
static int run_window(FA18Machine *m, FA18Replay *replay, int start_frame, int frames, int scale) {
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    static uint32_t argb[FA18_SCREEN_W * FA18_SCREEN_H];
    uint64_t deadline;
    int running = 1, frame = 0, grabbed = 0;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    win = SDL_CreateWindow("F/A-18 Interceptor (translated)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           FA18_SCREEN_W * scale, FA18_SCREEN_H * scale, SDL_WINDOW_RESIZABLE);
    ren = win ? SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC) : NULL;
    if (!ren && win) ren = SDL_CreateRenderer(win, -1, 0);
    tex = ren ? SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FA18_SCREEN_W,
                                  FA18_SCREEN_H) : NULL;
    if (!tex) {
        fprintf(stderr, "SDL display creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetLogicalSize(ren, FA18_SCREEN_W, FA18_SCREEN_H);
    deadline = SDL_GetTicks64();
    while (running && (frames <= 0 || frame < frames)) {
        SDL_Event e;
        int p;
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
        fa18_replay_apply(replay, m, start_frame + frame + 1);
        if (frame == 0 && restore_lead) { fa18_machine_run_frame(m); fa18_loop_frame(); }
        fa18_machine_run_frame(m);
        fa18_loop_frame();
        frame++;
        for (p = 0; p < FA18_SCREEN_W * FA18_SCREEN_H; p++) {
            uint16_t v = m->last_screen[p];
            argb[p] = 0xFF000000u | (uint32_t)((v >> 8) & 15) * 0x110000u | (uint32_t)((v >> 4) & 15) * 0x1100u |
                      (uint32_t)(v & 15) * 0x11u;
        }
        SDL_UpdateTexture(tex, NULL, argb, FA18_SCREEN_W * (int)sizeof argb[0]);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        deadline += 20; /* 50 Hz PAL */
        while (SDL_GetTicks64() < deadline) SDL_Delay(1);
        if (SDL_GetTicks64() > deadline + 100) deadline = SDL_GetTicks64();
    }
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
#endif

int main(int argc, char **argv) {
    const char *state_path = NULL, *rom_path = NULL, *ppm = NULL, *ppm_dir = NULL, *rgb_path = NULL,
               *fallback = NULL, *ram_out = NULL;
    const char *record_path = NULL, *input_path = NULL;
    int to_end = 0;
    const char *replay_path = NULL, *ports_only = NULL, *ports_report = NULL, *profile_path = NULL, *edges_path = NULL;
    const char *rom_transitions_path = NULL;
    int os_vbeam = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_waitblit = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_waitbovp = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_blitter_owner = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_exec_interrupts = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_getmsg = -1; /* default C with recomp, ROM in interpreter-only mode */
    int os_potgo = -1; /* default C with recomp, ROM in interpreter-only mode */
    FA18PortMode ports_mode = FA18_PORTS_OFF;
    int frames = 10, use_recomp = 1, i, start_frame = 0, window = 0, scale = 3;
    FA18Replay replay = {0};
    size_t state_size, rom_size;
    uint8_t *state, *rom;
    char error[256];
    FA18Machine *m;
    FILE *rgb = NULL;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--state") && i + 1 < argc) state_path = argv[++i];
        else if (!strcmp(argv[i], "--rom") && i + 1 < argc) rom_path = argv[++i];
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
        else if (!strcmp(argv[i], "--ports") && i + 1 < argc) {
            const char *v = argv[++i];
            ports_mode = !strcmp(v, "on") ? FA18_PORTS_ON : !strcmp(v, "shadow") ? FA18_PORTS_SHADOW
                       : !strcmp(v, "sandbox") ? FA18_PORTS_SANDBOX : FA18_PORTS_OFF;
        }
        else if (!strcmp(argv[i], "--ports-only") && i + 1 < argc) ports_only = argv[++i];
        else if (!strcmp(argv[i], "--ports-report") && i + 1 < argc) ports_report = argv[++i];
        else if (!strcmp(argv[i], "--profile") && i + 1 < argc) profile_path = argv[++i];
        else if (!strcmp(argv[i], "--edges") && i + 1 < argc) edges_path = argv[++i];
        else if (!strcmp(argv[i], "--rom-transitions") && i + 1 < argc) rom_transitions_path = argv[++i];
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
        else if (!strcmp(argv[i], "--poison")) fa18_ports_set_poison(1);
        else if (!strcmp(argv[i], "--scale") && i + 1 < argc) scale = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--record") && i + 1 < argc) record_path = argv[++i];
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) input_path = argv[++i];
        else if (!strcmp(argv[i], "--to-end")) to_end = 1;
        else { usage(); return 2; }
    }
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
    fa18_recomp_init(use_recomp);
    if (os_vbeam) fa18_recomp_enable_vbeam_shim();
    if (os_waitblit) fa18_recomp_enable_wait_blit_shim();
    if (os_waitbovp) fa18_recomp_enable_wait_bovp_shim();
    if (os_blitter_owner) fa18_recomp_enable_blitter_ownership_shim();
    if (os_exec_interrupts) fa18_recomp_enable_exec_interrupt_shim();
    if (os_getmsg) fa18_recomp_enable_exec_get_msg_shim();
    if (os_potgo) fa18_recomp_enable_potgo_shim();
    if (rom_transitions_path && !fa18_recomp_track_rom_transitions()) {
        fprintf(stderr, "cannot allocate ROM transition inventory\n");
        return 1;
    }
    fa18_ports_init(ports_mode, ports_only);
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
    if (window) {
#ifdef FA18_WITH_SDL
        int result = run_window(m, &replay, start_frame, frames, scale);
        if (!fa18_bus_trace_close()) return 1;
        fa18_loop_finish();
        fa18_replay_free(&replay);
        return result;
#else
        fprintf(stderr, "--window needs the CMake build (SDL2)\n");
        return 2;
#endif
    }
    if (rgb_path && !(rgb = fopen(rgb_path, "wb"))) { fprintf(stderr, "cannot write %s\n", rgb_path); return 1; }
    for (i = 0; to_end ? fa18_loop_iterations() < fa18_loop_replay_end() : i < frames; i++) {
        /* Events recorded for a frame are delivered before that frame runs. */
        fa18_replay_apply(&replay, m, start_frame + i + 1);
        if (i == 0 && restore_lead) { fa18_machine_run_frame(m); fa18_loop_frame(); }
        fa18_machine_run_frame(m);
        fa18_loop_frame();
        if (rgb) fwrite(m->last_screen, sizeof m->last_screen[0], FA18_SCREEN_W * FA18_SCREEN_H, rgb);
        if (ppm_dir) {
            char path[512];
            snprintf(path, sizeof path, "%s/frame_%03d.ppm", ppm_dir, i + 1);
            write_ppm(path, m->last_screen);
        }
    }
    if (rgb) fclose(rgb);
    if (!fa18_bus_trace_close()) return 1;
    if (ppm && !write_ppm(ppm, m->last_screen)) { fprintf(stderr, "cannot write %s\n", ppm); return 1; }
    if (fallback) fa18_recomp_write_fallback_log(fallback);
    if (profile_path) fa18_recomp_write_profile(profile_path);
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
               to_end ? i : frames, use_recomp, (unsigned long long)total, (unsigned long long)gen,
               total ? (double)gen / (double)total : 0.0, (unsigned long long)fa18_recomp_stats.dispatches,
               (unsigned long long)fa18_recomp_stats.interpreted_game,
               (unsigned long long)fa18_recomp_stats.code_writes, fa18_recomp_stats.disabled_functions,
               (unsigned long long)m->blits, (unsigned long long)m->line_blits, nonblack,
               m68k_get_reg(NULL, M68K_REG_PC), fa18_loop_iterations());
    }
    return 0;
}
