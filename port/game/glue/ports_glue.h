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

/* batch 14: paired sin_cos, print_number, square_root */
int glue_C2E5F6(void);
int glue_C24F76(void);
int glue_C2564E(void);

/* batch 15: cell occupancy, record position, record 76/78 */
int glue_C1D520(void);
int glue_C1D0B6(void);
int glue_C26428(void);

/* batch 16: small text */
int glue_C32794(void);
int glue_C32662(void);
int glue_C3271A(void);
int glue_C32736(void);

/* batch 17: BCD unpack, sorted search, record 56/66 with alert */
int glue_C259C2(void);
int glue_C1D4E4(void);
int glue_C13A8E(void);

/* batch 18: joystick */
int glue_C16F1C(void);

/* batch 19: rotation matrices and row scaling */
int glue_C2E47A(void);
int glue_C2E38E(void);
int glue_C2E5AC(void);

/* batch 19: three-angle matrix variants */
int glue_C2E3DE(void);
int glue_C2E514(void);

/* batch 20: inverse orientation, stream skip, attitude flags, vertex tail, divide, interrupt server */
int glue_C2D970(void);
int glue_C21940(void);
int glue_C122A2(void);
int glue_C0D384(void);
int glue_C52EC8(void);
int glue_C06132(void);

/* batch 20: play_sound */
int glue_C17B2C(void);

/* batch 21: side-plane clips, view transform */
int glue_C2EA5A(void);
int glue_C2EAD0(void);
int glue_C2F0C6(void);
int glue_C2F0F4(void);
int glue_C1F2EE(void);

/* batch 22: magnitude */
int glue_C1D974(void);

/* batch 23: y-plane clips, sound routines, record orientation */
int glue_C2EB4C(void);
int glue_C2EBC2(void);
int glue_C2F156(void);
int glue_C17CF6(void);
int glue_C17DAA(void);
int glue_C17E4A(void);
int glue_C17EF2(void);
int glue_C18096(void);
int glue_C2D954(void);

/* batch 24: local to world, shown vertices, normalize, slot scan */
int glue_C091E0(void);
int glue_C091CE(void);
int glue_C091A8(void);
int glue_C0D334(void);
int glue_C25754(void);
int glue_C265E8(void);

/* batch 25: main engine, tone, edge vertices, buffers, stage blit, grid position, list point */
int glue_C17C62(void);
int glue_C17D6E(void);
int glue_C3316A(void);
int glue_C219AE(void);
int glue_C2FD22(void);
int glue_C3040C(void);
int glue_C1EBE0(void);
int glue_C25876(void);

/* batch 26: tones, page plane tops */
int glue_C33180(void);
int glue_C3318E(void);
int glue_C33186(void);
int glue_C2F582(void);

/* batch 27: target point, record range, lane blit */
int glue_C1C2C8(void);
int glue_C24568(void);
int glue_C304FA(void);

/* batch 28: post-input expiry, projection seed, condition tables */
int glue_C10D8A(void);
int glue_C1C54E(void);
int glue_C09AB8(void);

/* batch 29: component bound, repeated sum, view key */
int glue_C1FC42(void);
int glue_C1BA86(void);

/* batch 30: audio interrupt, date line */
int glue_C50158(void);
int glue_C24E2C(void);

/* batch 31: condition flags, lost selection */
int glue_C09A78(void);
int glue_C09A98(void);
int glue_C12242(void);

/* batch 32: fault hook, level lists */
int glue_C06C02(void);
int glue_C1D5D8(void);

#endif
