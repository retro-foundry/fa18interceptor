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

/* batch 10: decay, messages, lookups, cell steps, 2.8 matrix, cached display value */
int glue_C148A2(void);
int glue_C11312(void);
int glue_C287DA(void);
int glue_C1FEF2(void);
int glue_C1ECFC(void);
int glue_C1ECD4(void);
int glue_C2E346(void);
int glue_C31C20(void);

/* batch 11: player setup, steering, random bits, channel stop, cockpit script */
int glue_C09620(void);
int glue_C13BA0(void);
int glue_C13C64(void);
int glue_C50B02(void);
int glue_C180FC(void);
int glue_C1EC96(void);
int glue_C21916(void);
int glue_C21966(void);
int glue_C2198C(void);
int glue_C218C8(void);
int glue_C207FE(void);

/* batch 12: view octant, paired records, attitude term */
int glue_C254E8(void);
int glue_C231A2(void);
int glue_C148E2(void);

/* batch 13: decimal format, cockpit slide, position history */
int glue_C3267A(void);
int glue_C2559A(void);
int glue_C2651E(void);

#endif
