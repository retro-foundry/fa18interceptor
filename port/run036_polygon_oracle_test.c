#include "blit_job.h"
#include "projection_grid.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CHIP_BYTES = 512 * 1024 };

typedef struct {
    FA18ProjectionPairBlitterWrites writes[4];
    unsigned count;
    FA18ProjectionPairFinalState final_state;
    uint32_t final_line_cpt;
} Run036Emitter;

static int emit_blitter(void *context, const FA18ProjectionPairBlitterWrites *writes) {
    Run036Emitter *emitter = context;
    if (!emitter || !writes || emitter->count >= 4) return -1;
    emitter->writes[emitter->count++] = *writes;
    return 0;
}

static int emit_final(void *context, const FA18ProjectionPairFinalState *state) {
    Run036Emitter *emitter = context;
    if (!emitter || !state) return -1;
    emitter->final_state = *state;
    return 0;
}

static int ignored_protected_line(void *context, int16_t x0, int16_t y0,
                                  int16_t x1, int16_t y1, uint32_t scratch) {
    (void)context; (void)x0; (void)y0; (void)x1; (void)y1; (void)scratch;
    return -1;
}

static int load_file(const char *path, uint8_t *bytes) {
    FILE *file = 0;
    if (fopen_s(&file, path, "rb") != 0 || !file) return -1;
    const int failed = fread(bytes, 1, CHIP_BYTES, file) != CHIP_BYTES || fclose(file) != 0;
    return failed ? -1 : 0;
}

static int save_file(const char *path, const uint8_t *bytes) {
    FILE *file = 0;
    if (fopen_s(&file, path, "wb") != 0 || !file) return -1;
    const int failed = fwrite(bytes, 1, CHIP_BYTES, file) != CHIP_BYTES || fclose(file) != 0;
    return failed ? -1 : 0;
}

static size_t report_difference_runs(const char *label, const uint8_t *left,
                                     const uint8_t *right) {
    size_t differences = 0;
    unsigned runs = 0;
    for (size_t index = 0; index < CHIP_BYTES;) {
        if (left[index] == right[index]) {
            ++index;
            continue;
        }
        const size_t first = index;
        do {
            ++differences;
            ++index;
        } while (index < CHIP_BYTES && left[index] != right[index]);
        if (runs++ < 24)
            printf("%s[%05zX..%05zX]\n", label, first, index - 1u);
    }
    if (runs > 24) printf("%s additional_runs=%u\n", label, runs - 24u);
    return differences;
}

static int apply_line_writes(uint8_t *chip, Run036Emitter *emitter) {
    FA18BlitOperation operation = {
        .bltafwm = 0xffff, .bltalwm = 0xffff,
        .bltbpt = 0x00728e, .bltcdat = 0
    };
    for (unsigned index = 0; index < emitter->count; ++index) {
        const FA18ProjectionPairBlitterWrites *writes = &emitter->writes[index];
        operation.bltcon0 = writes->bltcon0;
        operation.bltcon1 = writes->bltcon1;
        operation.bltadat = writes->bltadat;
        operation.bltbdat = writes->bltbdat;
        operation.bltamod = writes->bltamod;
        operation.bltbmod = writes->bltbmod;
        operation.bltcmod = writes->bltcmod;
        operation.bltdmod = writes->bltdmod;
        operation.bltapt = writes->bltapt_low;
        operation.bltcpt = writes->bltcpt;
        operation.bltdpt = writes->bltdpt;
        operation.bltsize = writes->bltsize;
        if (fa18_execute_ocs_line_blit(&operation, chip, CHIP_BYTES) != 0) {
            fprintf(stderr, "run036 line job %u failed\n", index);
            return -1;
        }
        if (index + 1u == emitter->count) emitter->final_line_cpt = operation.bltcpt;
    }
    return 0;
}

/* `$C303EC-$C30404` inherited fill register image.  The fill-only oracle
 * receives a checkpoint at the `$C30404` trigger, after line jobs settled. */
static int apply_descending_fill(uint8_t *chip, const Run036Emitter *emitter) {
    FA18BlitOperation operation = {
        .bltafwm = 0xffff, .bltalwm = 0xffff,
        /* `$C30466/$C304B2` do not rewrite these after the final fill. */
        .bltamod = 0x001d, .bltbmod = 0x001d,
        .bltcmod = 0x0028, .bltdmod = 0x001d,
        .bltcdat = emitter->final_state.bltcdat
    };
    /* `$C303EC-$C30404`: C is inherited from the last line submission. */
    operation.bltcon0 = emitter->final_state.bltcon0;
    operation.bltcon1 = emitter->final_state.bltcon1;
    operation.bltadat = emitter->final_state.bltadat;
    operation.bltbdat = emitter->final_state.bltbdat;
    operation.bltamod = 0x001d;
    operation.bltbmod = 0x001d;
    operation.bltdmod = 0x001d;
    operation.bltapt = emitter->final_state.bltapt;
    operation.bltbpt = 0x00728e;
    /* `$C302E6-$C30404` loads D0 with -1, then writes that longword to C.
     * It does not retain the C pointer advanced by the final line job. */
    operation.bltcpt = emitter->final_state.bltcpt;
    operation.bltdpt = emitter->final_state.bltdpt;
    operation.bltsize = emitter->final_state.blit_size;
    return fa18_execute_ocs_block_blit(&operation, chip, CHIP_BYTES);
}

static int apply_fill_and_lanes(uint8_t *chip, const Run036Emitter *emitter,
                                const char *after_fill_path) {
    FA18BlitOperation operation = {
        .bltafwm = 0xffff, .bltalwm = 0xffff,
        /* `$C30466/$C304B2` inherit the final fill's modulos. */
        .bltamod = 0x001d, .bltbmod = 0x001d,
        .bltcmod = 0x0028, .bltdmod = 0x001d,
        .bltcdat = emitter->final_state.bltcdat
    };
    if (apply_descending_fill(chip, emitter) != 0) return -1;
    if (after_fill_path && save_file(after_fill_path, chip) != 0) return -1;

    /* run036's `$C2FF58` state after `$C30404`: `$C456B6` points at
     * this source-order plane table and lanes 0/1 are enabled. */
    FA18RendererLaneStage lanes = {
        { 0x00018980u, 0x00016a40u, 0x00014b00u, 0x00012bc0u },
        0x03u, 3, 0, 0, 7,
        emitter->final_state.offset_long, emitter->final_state.lane_copy,
        emitter->final_state.lane_long, emitter->final_state.blit_size, 0
    };
    return fa18_execute_renderer_lane_stage(&lanes, &operation, chip, CHIP_BYTES);
}

int main(int argc, char **argv) {
    uint8_t *native = 0;
    uint8_t *initial = 0;
    uint8_t *original = 0;
    int result = 1;
    const int fill_only = (argc == 4 || argc == 5) &&
                          strcmp(argv[1], "--fill-only") == 0;
    const int post_lines = (argc == 4 || argc == 5) &&
                           strcmp(argv[1], "--post-lines") == 0;
    const int argument = (fill_only || post_lines) ? 2 : 1;
    if ((!fill_only && !post_lines && argc != 3 && argc != 4 && argc != 5) ||
        ((fill_only || post_lines) && argc != 4 && argc != 5)) {
        fprintf(stderr, "usage: %s PRE_CALL_CHIP POST_CALL_CHIP [AFTER_LINES_CHIP [AFTER_FILL_CHIP]]\n"
                        "       %s --fill-only PRE_TRIGGER_CHIP POST_FILL_CHIP [NATIVE_FILL_CHIP]\n",
                        argv[0], argv[0]);
        fprintf(stderr,
                "       %s --post-lines PRE_FILL_CHIP POST_CALL_CHIP [NATIVE_FILL_CHIP]\n",
                argv[0]);
        return 2;
    }
    native = malloc(CHIP_BYTES);
    initial = malloc(CHIP_BYTES);
    original = malloc(CHIP_BYTES);
    if (!native || !initial || !original || load_file(argv[argument], native) != 0 ||
        load_file(argv[argument + 1], original) != 0)
        goto done;
    memcpy(initial, native, CHIP_BYTES);

    const FA18ProjectionPairScreenPoint pairs[] = {
        {97, 127}, {130, 138}, {74, 145}, {57, 130}
    };
    Run036Emitter emitter = {0};
    FA18ProjectionPairBoundsRoute bounds_route;
    FA18ProjectionPairFinalizationRoute finalization_route;
    if (fa18_submit_projection_pair_far_list(
            pairs, 4, 144, 145, 127, 0x00006048u, 0, 0,
            ignored_protected_line, 0, emit_blitter, emit_final, &emitter,
            &bounds_route, &finalization_route) != 0 || emitter.count != 4 ||
        bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION ||
        finalization_route != FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED ||
        (fill_only ? (apply_descending_fill(native, &emitter) != 0 ||
                      (argc == 5 && save_file(argv[4], native) != 0)) :
         post_lines ? (apply_fill_and_lanes(native, &emitter,
                                            argc == 5 ? argv[4] : 0) != 0)
                   : (apply_line_writes(native, &emitter) != 0 ||
                      (argc == 4 && save_file(argv[3], native) != 0) ||
                      apply_fill_and_lanes(native, &emitter,
                                           argc == 5 ? argv[4] : 0))))
        goto done;

    printf("RUN036_FINAL blit=%04X con0=%04X con1=%04X a=%06X c=%06X line_c=%06X d=%06X lane=%06X\n",
           emitter.final_state.blit_size, emitter.final_state.bltcon0,
           emitter.final_state.bltcon1, emitter.final_state.bltapt,
           emitter.final_state.bltcpt, emitter.final_line_cpt, emitter.final_state.bltdpt,
           emitter.final_state.lane_long);

    const size_t differences = report_difference_runs("NATIVE_VS_ORIGINAL ", native, original);
    const size_t native_writes = report_difference_runs("NATIVE_CHANGED ", native, initial);
    const size_t original_writes = report_difference_runs("ORIGINAL_CHANGED ", original, initial);
    printf("RUN036_POLYGON_BYTE_DIFFERENCES=%zu NATIVE_CHANGED_BYTES=%zu ORIGINAL_CHANGED_BYTES=%zu\n",
           differences, native_writes, original_writes);
    result = differences ? 1 : 0;
done:
    free(native);
    free(initial);
    free(original);
    return result;
}
