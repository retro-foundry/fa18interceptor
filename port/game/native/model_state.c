/* The source's retained early-expiry model output, owned by the renderer.
 * Native startup resets this with the other newly allocated scratch state. */
#include "model.h"

static uint16_t retained_result;

uint16_t native_model_retained_result(void) { return retained_result; }
void native_model_retain_result(uint16_t value) { retained_result=value; }
