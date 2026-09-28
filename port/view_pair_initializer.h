#ifndef FA18_VIEW_PAIR_INITIALIZER_H
#define FA18_VIEW_PAIR_INITIALIZER_H

#include <stdint.h>

/* Native View and display-instruction identities. They replace only the
 * source pointer representation; their construction remains caller-owned. */
typedef struct {
    uint32_t view_pointer;
    uint32_t display_instruction_pointer;
} FA18NativeViewPair;

typedef int (*FA18ViewPairBuildDisplayInstructions)(void *context,
                                                     uint32_t raster_source,
                                                     uint32_t *result);
typedef int (*FA18ViewPairBuildView)(void *context, uint32_t raster_source,
                                     uint32_t display_instruction,
                                     uint32_t *result);

typedef struct {
    FA18ViewPairBuildDisplayInstructions build_display_instructions;
    FA18ViewPairBuildView build_view;
    void *context;
} FA18ViewPairInitializerOps;

typedef struct {
    /* `$C1822E` / `$C1825A`: source-selected raster/view configuration. */
    uint32_t active_raster_source;
    /* `$C1821C` / `$C18232`: live results before table publication. */
    FA18NativeViewPair live_pair;
    /* `$C182BA` / `$C182C2`: the two published View/DspIns pairs. */
    FA18NativeViewPair pair[2];
} FA18ViewPairInitializerState;

/* `$C16084-$C16126`: select the slot's raster source, construct its display
 * instruction first and View second, then copy those live fields into the
 * corresponding pointer-table slot. The two construction callbacks are
 * required because the original graphics-library calls own their identities.
 * This routine never imports the observed Amiga pointers as native values. */
int fa18_initialize_view_pair_slot(FA18ViewPairInitializerState *state,
                                   uint16_t slot, uint32_t raster_source,
                                   const FA18ViewPairInitializerOps *ops);

#endif
