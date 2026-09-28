#ifndef FA18_PORTS_GLUE_H
#define FA18_PORTS_GLUE_H

/* Glue entry points, one per recreated routine, named by original address. */

/* render_polygon.c */
int glue_C30466(void);
int glue_C304B2(void);
int glue_C305AA(void);

/* render_line.c */
int glue_C2FA7E(void);

/* fixed_math.c, audio.c, text.c */
int glue_C2E6DA(void);
int glue_C501E0(void);
int glue_C24FE8(void);
int glue_C330FE(void);
int glue_C32806(void);
int glue_C25A08(void);
int glue_C1715C(void);
int glue_C2F558(void);

/* notify.c, control_records.c, screen_frame.c, render_span.c, fixed_math.c */
int glue_C11B44(void);
int glue_C15138(void);
int glue_C310E2(void);
int glue_C1EBC0(void);
int glue_C230B0(void);
int glue_C2DE96(void);
int glue_C0DAA0(void);
int glue_C0DAD0(void);
int glue_C0DAD4(void);
int glue_C0DADC(void);
int glue_C0DAE6(void);

/* render_state.c, view.c, stages.c */
int glue_empty_stage(void);
int glue_C25864(void);
int glue_C2F490(void);
int glue_C4FFB4(void);
int glue_C2F596(void);
int glue_C08324(void);
int glue_C095C0(void);
int glue_C1D722(void);
int glue_C30F56(void);
int glue_C1C7F6(void);
int glue_C25482(void);

/* batch 7 */
int glue_C50212(void);
int glue_C4FFB0(void);
int glue_C13B5A(void);
int glue_C14876(void);
int glue_C308E2(void);
int glue_C30904(void);
int glue_C2DEA2(void);
int glue_C28F16(void);
int glue_C0FA4C(void);
int glue_C0FA80(void);
int glue_C1FE20(void);
int glue_C21960(void);
int glue_C25980(void);
int glue_C2E370(void);

/* batch 8: depth sort, records, compass, cockpit, context stage */
int glue_C1E4A6(void);
int glue_C1CA82(void);
int glue_C1EC3A(void);
int glue_C2DAF2(void);
int glue_C310AA(void);
int glue_C082B8(void);
int glue_C082B0(void);
int glue_C10C08(void);
int glue_C11B0E(void);

/* batch 9: hex text, decay, nudge, random, voices, readout, mission, view pan */
int glue_C0F56A(void);
int glue_C13A2A(void);
int glue_C13CDE(void);
int glue_C13396(void);
int glue_C50AB4(void);
int glue_C17B08(void);
int glue_C2548A(void);
int glue_C0840E(void);
int glue_C258C8(void);

#endif
