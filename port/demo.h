#ifndef FA18_DEMO_H
#define FA18_DEMO_H

#include "menu.h"
#include "renderer.h"

typedef enum {
    FA18_DEMO_IDLE,
    FA18_DEMO_WAITING_FOR_TRANSITION,
    FA18_DEMO_ENTRY_READY,
    FA18_DEMO_FOLLOWUP_MATCH
} FA18DemoPhase;

/* Native controller for the bounded run075 menu-to-demo route. It owns no
 * Amiga addresses or callback pointers; unproved callbacks are not invoked. */
typedef struct {
    FA18MenuState menu;
    FA18IndexedFrameBuffer work_buffer;
    FA18DemoPhase phase;
} FA18DemoController;

/* Begins after the recorded first menu command has been recognized. */
int fa18_demo_start_run075(FA18DemoController *controller);

/* Executes one proved post-input tick and its known delayed-transition route.
 * Returns 1 when the demo-entry continuation becomes ready, 0 while waiting,
 * or -1 for an invalid controller state.
 */
int fa18_demo_tick(FA18DemoController *controller);

#endif
