#ifndef FA18_NATIVE_VECTOR_MATH_H
#define FA18_NATIVE_VECTOR_MATH_H
#include "scene_component_magnitude.h"
typedef struct {
    const PortFieldWindow *table;
    uint16_t *magnitude;
    int16_t *normalized; /* same three shared words consumed by game callers */
} FA18NativeVectorMath;

/* Complete C2574A with actual C1D974. Scale and component low words retain
 * original arithmetic; the result words and numeric carried axis are live.
 * Returns zero for missing data or a source path that cannot complete. */
int fa18_normalize_native_vector(const FA18NativeVectorMath *state,int16_t scale,
    const int32_t components[3],uint32_t *axis);
/* Shared body of C25754, whose argument's high word supplies direction and
 * low word supplies scale. These are explicit numeric inputs, not a stack. */
int fa18_normalize_native_vector_with_direction(const FA18NativeVectorMath *state,
    int16_t scale,int16_t direction,const int32_t components[3],uint32_t *axis);
#endif
