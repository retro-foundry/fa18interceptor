/* Remaining postflight helpers. Complete history/display/render owners are
 * in hud_history_stream.c, hud_projection_parents.c and render_parents.c. */
#include "glue.h"
#include "ports_glue.h"
#include "globals.h"
#include "memory.h"
#include "glue_text.h"

int glue_C28E28(void) {
    return glue_complete_zone_exit();
}
