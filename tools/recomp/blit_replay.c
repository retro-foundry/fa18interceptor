/* Blitter differential check: apply recorded custom-register writes (from an
 * Engine9000 custom_writes.jsonl, converted to "reg value" lines) to a Chip
 * RAM image through port/machine's blitter, then write the resulting image. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "recomp_runtime.h"

/* Link stubs: the blitter only needs the custom-register file. */
void fa18_raise_interrupt(FA18Machine *m, int bit) { (void)m; (void)bit; }
void fa18_blitter_zero_flag(int zero) { (void)zero; }
void fa18_blitter_busy(FA18Machine *m, int cycles) { (void)m; (void)cycles; }
void fa18_recomp_note_write(uint32_t a, int s) { (void)a; (void)s; }

int main(int argc, char **argv) {
    static FA18Machine m;
    FILE *f;
    unsigned reg, value;
    if (argc != 4) { fprintf(stderr, "usage: blit_replay CHIP.bin WRITES.txt OUT.bin\n"); return 2; }
    f = fopen(argv[1], "rb");
    if (!f || fread(m.chip, 1, FA18_CHIP_SIZE, f) != FA18_CHIP_SIZE) return 1;
    fclose(f);
    f = fopen(argv[2], "r");
    if (!f) return 1;
    while (fscanf(f, "%x %x", &reg, &value) == 2) {
        reg &= 0x1FE;
        m.custom[reg >> 1] = (uint16_t)value;
        if (reg == 0x058) fa18_blitter_start(&m);
    }
    fclose(f);
    f = fopen(argv[3], "wb");
    fwrite(m.chip, 1, FA18_CHIP_SIZE, f);
    fclose(f);
    return 0;
}
