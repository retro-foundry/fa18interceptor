/* fa18_recomp: run the translated game from a UAE savestate on the native
 * machine layer and write the resulting frames. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68k.h"
#include "machine.h"
#include "recomp_runtime.h"

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
            "                   [--ppm-every DIR] [--rgb444 OUT.bin] [--no-recomp] [--fallback-log OUT.json]\n");
}

int main(int argc, char **argv) {
    const char *state_path = NULL, *rom_path = NULL, *ppm = NULL, *ppm_dir = NULL, *rgb_path = NULL,
               *fallback = NULL, *ram_out = NULL;
    int frames = 10, use_recomp = 1, i;
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
    if (rgb_path && !(rgb = fopen(rgb_path, "wb"))) { fprintf(stderr, "cannot write %s\n", rgb_path); return 1; }
    for (i = 0; i < frames; i++) {
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
