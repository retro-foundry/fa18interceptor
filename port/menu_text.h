#ifndef FA18_MENU_TEXT_H
#define FA18_MENU_TEXT_H

#include <stddef.h>
#include <stdint.h>

enum { FA18_MENU_TEXT_SELECTORS = 12 };

/* The bounded menu state written directly by `$C0FBE0-$C0FCB3`. Helper-call
 * effects and the later selection callback are separate port contracts. */
typedef struct {
    uint32_t display_mode;
    uint32_t video_latch;
    uint32_t display_delay;
    uint8_t video_flags;
    uint8_t auxiliary_latch;
    uint8_t mode_latch;
    uint16_t selectors[FA18_MENU_TEXT_SELECTORS];
    size_t selector_count;
} FA18MenuTextState;

/* Queue the title and top-level option records exactly as `$C0FBE0` does.
 * The zero terminator occupies the final selector slot. */
int fa18_menu_queue_top_level_text(FA18MenuTextState *state);

#endif
