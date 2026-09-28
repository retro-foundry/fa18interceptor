/* fa18_recomp: run the translated game from a UAE savestate on the native
 * machine layer and write the resulting frames. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68k.h"
#include "machine.h"
#include "recomp_runtime.h"
#include "input.h"

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
            "                   [--window [--scale N]]   (window: --frames 0 runs until closed)\n");
}

#ifdef FA18_WITH_SDL
#define SDL_MAIN_HANDLED
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
                    if (raw >= 0) fa18_machine_key(m, raw, e.type == SDL_KEYDOWN);
                }
                break;
            case SDL_MOUSEMOTION:
                if (grabbed) fa18_machine_mouse(m, e.motion.xrel, e.motion.yrel);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (!grabbed && e.type == SDL_MOUSEBUTTONDOWN) {
                    grabbed = 1;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                } else if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
                    fa18_machine_button(m, e.button.button == SDL_BUTTON_LEFT ? 0 : 1,
                                        e.type == SDL_MOUSEBUTTONDOWN);
                }
                break;
            default: break;
            }
        }
        fa18_replay_apply(replay, m, start_frame + frame + 1);
        fa18_machine_run_frame(m);
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
    const char *replay_path = NULL;
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
        else if (!strcmp(argv[i], "--window")) window = 1;
        else if (!strcmp(argv[i], "--scale") && i + 1 < argc) scale = atoi(argv[++i]);
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
    fa18_recomp_init(use_recomp);
    if (replay_path && !fa18_replay_load(&replay, replay_path)) {
        fprintf(stderr, "cannot read E9K_INPUT_V1 replay %s\n", replay_path);
        return 1;
    }
    if (window) {
#ifdef FA18_WITH_SDL
        int result = run_window(m, &replay, start_frame, frames, scale);
        fa18_replay_free(&replay);
        return result;
#else
        fprintf(stderr, "--window needs the CMake build (SDL2)\n");
        return 2;
#endif
    }
    if (rgb_path && !(rgb = fopen(rgb_path, "wb"))) { fprintf(stderr, "cannot write %s\n", rgb_path); return 1; }
    for (i = 0; i < frames; i++) {
        /* Events recorded for a frame are delivered before that frame runs. */
        fa18_replay_apply(&replay, m, start_frame + i + 1);
        fa18_machine_run_frame(m);
        if (rgb) fwrite(m->last_screen, sizeof m->last_screen[0], FA18_SCREEN_W * FA18_SCREEN_H, rgb);
        if (ppm_dir) {
            char path[512];
            snprintf(path, sizeof path, "%s/frame_%03d.ppm", ppm_dir, i + 1);
            write_ppm(path, m->last_screen);
        }
    }
    if (rgb) fclose(rgb);
    if (ppm && !write_ppm(ppm, m->last_screen)) { fprintf(stderr, "cannot write %s\n", ppm); return 1; }
    if (fallback) fa18_recomp_write_fallback_log(fallback);
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
               "\"nonblack_pixels\": %d, \"pc\": \"%06X\"}\n",
               frames, use_recomp, (unsigned long long)total, (unsigned long long)gen,
               total ? (double)gen / (double)total : 0.0, (unsigned long long)fa18_recomp_stats.dispatches,
               (unsigned long long)fa18_recomp_stats.interpreted_game,
               (unsigned long long)fa18_recomp_stats.code_writes, fa18_recomp_stats.disabled_functions,
               (unsigned long long)m->blits, (unsigned long long)m->line_blits, nonblack,
               m68k_get_reg(NULL, M68K_REG_PC));
    }
    return 0;
}
