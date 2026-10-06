#ifndef FA18_PORTS_GLUE_H
#define FA18_PORTS_GLUE_H
#include <stdint.h>
#include "glue_command_dispatch.h"
#include "glue_postflight_scheduler.h"
#include "glue_context_publication.h"
#include "glue_menu_transition.h"
#include "glue_menu_setup.h"
#include "glue_menu_cold.h"
#include "glue_menu_followup.h"
#include "glue_menu_outcome.h"
#include "glue_menu_return.h"
#include "glue_menu_context_finish.h"
#include "glue_postflight_completion.h"
#include "glue_postflight_messages.h"
#include "glue_postflight_file_callers.h"
#include "glue_input_device_callbacks.h"
#include "glue_input_display_setup.h"
#include "glue_main_loop_timers.h"
#include "glue_main_loop_control_messages.h"
#include "glue_record_control_actions.h"
#include "glue_main_loop_flight_controls.h"
int glue_C16EAE(void);
int glue_C16BF2(void);
int glue_C16C56(void);
int glue_C13D34(void);
int glue_C16EAE_step(void);
int glue_C16BF2_step(void);
int glue_C16C56_step(void);
int glue_C13D34_step(void);
int glue_C0EFD4(void);
int glue_C0EFD4_step(void);
int glue_C0F3C4(void);
int glue_C0F3C4_step(void);
int glue_C0D730(void);
int glue_C0D730_step(void);
int glue_C29042(void);
int glue_C29042_step(void);
int glue_origin_control_record(void);
int glue_origin_candidate_preset(uint32_t preset);
int glue_C22C80(void);
int glue_C1C63E(void);
int glue_C22C80_step(void);
int glue_C1C63E_step(void);
int glue_C1C860(void);
int glue_C08F26(void);
int glue_C0F920(void);
int glue_C0F992(void);
int glue_C1C860_step(void);
int glue_C08F26_step(void);
int glue_C0F920_step(void);
int glue_C0F992_step(void);
int glue_C090C2_step(void);
int glue_C090F2_step(void);
int glue_C0910C_step(void);
int glue_C0915A_step(void);
int glue_C0F4A6_step(void);
int glue_C11ACC_step(void);
int glue_C0F5F8(void);
int glue_C0F5F8_step(void);
int glue_C1CCBC(void);
int glue_C1D0A4(void);
int glue_C1CCBC_step(void);
int glue_C1D0A4_step(void);
int glue_C1D0B6_step(void);
int glue_C25876_step(void);
int glue_C1CB14(void);
int glue_C1CB26(void);
int glue_C1CB14_step(void);
int glue_C1CB26_step(void);
int glue_C1D10C(void);
int glue_C1D10C_step(void);
int glue_C1D722_step(void);

/* Glue entry points, one per recreated routine, named by original address. */

/* Scene transition and complete root-setup timing chain. */
int glue_C0FAA4_step(void);
int glue_C0924A_step(void);
int glue_C09266_step(void);
int glue_C092A0_step(void);
int glue_C095C0_step(void);
int glue_C09620_step(void);
int glue_C0840E_step(void);

/* Workspace selector helpers, including their shared source tails. */
int glue_C1EBB0(void);
int glue_C1EBB0_step(void);
int glue_C1EC84(void);
int glue_C1EC84_step(void);

/* active_planes.c and source-timed audio bridges */
int glue_C2FD8C(void);
int glue_C2FD8C_step(void);
int glue_C50158_step(void);
int glue_C501E0_step(void);
int glue_C50212_step(void);
int glue_C4FFB4_step(void);
int glue_C4FFB0_step(void);
int glue_C24FE8_step(void);
int glue_C330FE_step(void);
int glue_C32806_step(void);
int glue_C1715C_step(void);
int glue_C2F558_step(void);
int glue_C11B44_step(void);
int glue_C31226_step(void);
int glue_C3129A_step(void);
int glue_C31312_step(void);

/* render_polygon.c */
int glue_C30466(void);
int glue_C30466_step(void);
int glue_C304B2(void);
int glue_C304B2_step(void);
int glue_C305AA(void);
int glue_C305AA_step(void);

/* render_line.c */
int glue_C2FA7E(void);
int glue_C2FA7E_step(void);

/* fixed_math.c, audio.c, text.c */
int glue_C2E6DA(void);
int glue_C2E6DA_step(void);
int glue_C501E0(void);
int glue_C24FE8(void);
int glue_C330FE(void);
int glue_C32806(void);
int glue_C25A08(void);
int glue_C25A08_step(void);
int glue_C1715C(void);
int glue_C2F558(void);

/* notify.c, control_records.c, screen_frame.c, render_span.c, fixed_math.c */
int glue_C11B44(void);
int glue_C15138(void);
int glue_C310E2(void);
int glue_C1EBC0(void);
int glue_C230B0(void);
int glue_C230B0_step(void);
int glue_C244E2_step(void);
int glue_C1C54E_step(void);
int glue_C254E8_step(void);
int glue_C122A2_step(void);
int glue_C1C2C8_step(void);
int glue_C2374C_step(void);
int glue_C2574A_step(void);
int glue_C25704_step(void);

int glue_C0DAA0(void);
int glue_C0DAA0_step(void);
int glue_C0DAD0(void);
int glue_C0DAD0_step(void);
int glue_C0DAD4(void);
int glue_C0DAD4_step(void);
int glue_C0DADC(void);
int glue_C0DADC_step(void);
int glue_C0DAE6(void);
int glue_C0DAE6_step(void);

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
int glue_C14876(void);
int glue_C308E2(void);
int glue_C30904(void);
int glue_C28F16(void);
int glue_C28F16_step(void);
int glue_C0FA4C(void);
int glue_C0FA80(void);
int glue_C1FE20(void);
int glue_C21960(void);
int glue_C25980(void);
int glue_C25980_step(void);
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
int glue_C11B0E_step(void);

/* batch 9: hex text, decay, nudge, random, voices, readout, mission, view pan */
int glue_C0F56A(void);
int glue_C0F56A_step(void);
int glue_C13396(void);
int glue_C50AB4(void);
int glue_C50AB4_step(void);
int glue_C17B08(void);
int glue_C17B08_step(void);
int glue_C2548A(void);
int glue_C0840E(void);
int glue_C258C8(void);

/* batch 10: decay, messages, lookups, cell steps, 2.8 matrix, cached display value */
int glue_C148A2(void);
int glue_C11312(void);
int glue_C11312_step(void);
int glue_C287DA(void);
int glue_C287DA_step(void);
int glue_C1FEF2(void);
int glue_C1ECFC(void);
int glue_C1ECD4(void);
int glue_C2E346(void);
int glue_C31C20(void);

/* batch 11: player setup, steering, random bits, channel stop, cockpit script */
int glue_C09620(void);
int glue_C50B02(void);
int glue_C50B02_step(void);
int glue_C180FC(void);
int glue_C180FC_step(void);
int glue_C1EC96(void);
int glue_C21916(void);
int glue_C21966(void);
int glue_C2198C(void);
int glue_C218C8(void);
int glue_C207FE(void);

/* batch 12: view octant, paired records, attitude term */
int glue_C254E8(void);
int glue_C231A2(void);
int glue_C231A2_step(void);
int glue_C2559A_step(void);
int glue_C25864_step(void);

int glue_C148E2(void);

/* batch 13: decimal format, cockpit slide, position history */
int glue_C3267A(void);
int glue_C2559A(void);
int glue_C2651E(void);

/* batch 14: paired sin_cos, print_number, square_root */
int glue_C2E5F6(void);
int glue_C2E5F6_step(void);
int glue_C24F76(void);
int glue_C24F76_step(void);
int glue_C2564E(void);
int glue_C2564E_step(void);

/* batch 15: cell occupancy, record position, record 76/78 */
int glue_C1D520(void);
int glue_C1D520_step(void);
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
int glue_C1D4E4_step(void);

/* batch 18: joystick */
int glue_C16F1C(void);
int glue_C16F1C_step(void);

/* batch 19: rotation matrices and row scaling */
int glue_C2E47A(void);
int glue_C2E47A_step(void);
int glue_C2E38E(void);
int glue_C2E5AC(void);

/* batch 19: three-angle matrix variants */
int glue_C2E3DE(void);
int glue_C2E514(void);
int glue_C2E514_step(void);

/* batch 20: inverse orientation, stream skip, attitude flags, vertex tail, divide, interrupt server */
int glue_C2D970(void);
int glue_C21940(void);
int glue_C122A2(void);
int glue_C0D384(void);
int glue_C52EC8(void);
int glue_C06132(void);

/* batch 20: play_sound */
int glue_C17B2C(void);
int glue_C17B2C_step(void);

/* batch 21: side-plane clips, view transform */
int glue_C2EA5A(void);
int glue_C2EA5A_step(void);
int glue_C2EAD0(void);
int glue_C2EAD0_step(void);
int glue_C2F0C6(void);
int glue_C2F0C6_step(void);
int glue_C2F0F4(void);
int glue_C2F0F4_step(void);
int glue_C1F2EE(void);

/* batch 22: magnitude */
int glue_C1D974(void);

/* batch 23: y-plane clips, sound routines, record orientation */
int glue_C2EB4C(void);
int glue_C2EB4C_step(void);
int glue_C2EBC2(void);
int glue_C2EBC2_step(void);
int glue_C2F156(void);
int glue_C2F156_step(void);
int glue_C17CF6(void);
int glue_C17CF6_step(void);
int glue_C17DAA(void);
int glue_C17DAA_step(void);
int glue_C17E4A(void);
int glue_C17E4A_step(void);
int glue_C17EF2(void);
int glue_C17EF2_step(void);
int glue_C18096(void);
int glue_C18096_step(void);
int glue_C2D954(void);
int glue_C2D954_step(void);

/* batch 24: local to world, shown vertices, normalize, slot scan */
int glue_C091E0(void);
int glue_C091E0_step(void);
int glue_C091CE_step(void);
int glue_C091A8_step(void);
int glue_C091CE(void);
int glue_C091A8(void);
int glue_C0D334(void);
int glue_C25754(void);
int glue_C265E8(void);

/* batch 25: main engine, tone, edge vertices, buffers, stage blit, grid position, list point */
int glue_C17C62(void);
int glue_C17C62_step(void);
int glue_C17D6E(void);
int glue_C17D6E_step(void);
int glue_C3316A(void);
int glue_C3316A_step(void);
int glue_C219AE(void);
int glue_C2FD22(void);
int glue_C2FD22_step(void);
int glue_C3040C(void);
int glue_C3040C_step(void);
int glue_C1EBE0(void);
int glue_C25876(void);

/* batch 26: tones, page plane tops */
int glue_C33180(void);
int glue_C33180_step(void);
int glue_C3318E_step(void);
int glue_C33186_step(void);
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
int glue_C24E2C_step(void);

/* batch 31: condition flags, lost selection */
int glue_C09A78(void);
int glue_C09A98(void);
int glue_C12242(void);

/* batch 32: fault hook, level lists */
int glue_C06C02(void);
int glue_C1D5D8(void);
int glue_C1D5D8_step(void);

/* batch 33: pixel plots */
int glue_C2F5F4(void);
int glue_C2F60A(void);

/* batch 34: polygon preparation */
int glue_C301F6(void);
int glue_C301F6_step(void);

/* batch 35: polygon submission */
int glue_C2FF48(void);
int glue_C2FF48_step(void);

/* batch 36: clip stages */
int glue_C247C0(void);
int glue_C247C0_step(void);
int glue_C248B2(void);
int glue_C248B2_step(void);
int glue_C24996(void);
int glue_C24996_step(void);

/* batch 37: outer polygon clipper */
int glue_C2469E(void);
int glue_C2469E_step(void);
int glue_C246A0(void);
int glue_C246A0_step(void);

/* batch 38: faces and view marks */
int glue_C09952(void);
int glue_C099F6(void);
int glue_C332FE(void);
int glue_C30918(void);
int glue_C30918_step(void);

/* batch 39: cockpit messages */
int glue_C11BFC(void);

/* not registered: no recording calls it yet */
int glue_C345A0(void);

/* batch 41: plane-side test */
int glue_C27456(void);

/* batch 42: direction tracking */
int glue_C123FA(void);
int glue_C123FA_step(void);
int glue_C1E328_step(void);
int glue_C1E4A6_step(void);
int glue_C1D91A_step(void);
int glue_C1D974_step(void);
int glue_C09A78_step(void);
int glue_C09A98_step(void);
int glue_C09AB8_step(void);
int glue_C1CA82_step(void);
int glue_C06132_step(void);
int glue_C12098_step(void);
int glue_C1B906_step(void);
int glue_C1BA86_step(void);
int glue_C08324_step(void);
int glue_C082B8_step(void);
int glue_C082B0_step(void);
int glue_C1C7F6_step(void);
int glue_C2D99C_step(void);
int glue_C2D9BA_step(void);
int glue_C2DB18_step(void);
int glue_C2DEE0_step(void);
int glue_C2DAF2_step(void);
int glue_C2E370_step(void);
int glue_C2E346_step(void);
int glue_C2E38E_step(void);
int glue_C2E3DE_step(void);
int glue_C2E5AC_step(void);
int glue_C2D970_step(void);
int glue_C258C8_step(void);


int glue_C2F5C0_step(void);
int glue_C2F5D4_step(void);
int glue_C2F5F4_step(void);
int glue_C2F60A_step(void);
int glue_C2F63A_step(void);
int glue_C2F64E_step(void);
int glue_C2F66E_step(void);
int glue_C2F826_step(void);
int glue_C2F83A_step(void);
int glue_C2F844_step(void);
int glue_C2F84E_step(void);
int glue_C2F858_step(void);
int glue_C2F862_step(void);
int glue_C2F86C_step(void);
int glue_C2F876_step(void);
int glue_C2F880_step(void);
int glue_C2F88A_step(void);
int glue_C2F894_step(void);
int glue_C2F89E_step(void);
int glue_C2F8A8_step(void);
int glue_C2F8B2_step(void);
int glue_C2F8EA_step(void);
int glue_C2F904_step(void);
int glue_C2F91E_step(void);
int glue_C2F96C_step(void);
int glue_C2F986_step(void);
int glue_C2F9A0_step(void);
int glue_C2F9BA_step(void);
int glue_C2F9D4_step(void);
int glue_C2F9EE_step(void);
int glue_C2FA08_step(void);
int glue_C2FA22_step(void);



/* batch 43: normalize register entry */
int glue_C2574A(void);

/* batch 44: view aiming */
int glue_C2D9BA(void);

/* batch 45: face toward eye */
int glue_C1FB8C(void);

/* batch 46: target distance */
int glue_C1D91A(void);

/* batch 47: post-input stages, stick and throttle, flight recorder */
int glue_C0F946(void);
int glue_C0F974(void);
int glue_C0FB70(void);
int glue_C0FBB6(void);
int glue_C101FC(void);
int glue_C10228(void);
int glue_C1072E(void);
int glue_C1075A(void);
int glue_C11872(void);
int glue_C118E6(void);
int glue_C11958(void);
int glue_C119D4(void);
int glue_C0A2F0(void);
int glue_C1B4D0(void);
int glue_C1B4D4(void);
int glue_C1B4D8(void);
int glue_C1B4DE(void);
int glue_C1B50C(void);
int glue_C1B510(void);
int glue_C1B514(void);
int glue_C1B558(void);
int glue_C1B55C(void);
int glue_C1B560(void);
int glue_C25A6A(void);
int glue_C33DA4(void);
int glue_C21C4C(void);
int glue_C1FED4(void);

/* batch 48: coloured face, stored-normal test, record steering, view rotation, edge split */
int glue_C099AA(void);
int glue_C1FB9C(void);
int glue_C2CAA0(void);
int glue_C2CA92(void);
int glue_C2CA26(void);
int glue_C2CB86(void);
int glue_C2CE82(void);
int glue_C21C2E(void);

/* batch 49: projected segment, top-plane crossing, in-sight flag, edge alignment */
int glue_C2ED70(void);
int glue_C2F128(void);
int glue_C2F128_step(void);
int glue_C2436A(void);
int glue_C2084A(void);
int glue_C2082A(void);

/* batch 50: symbol plot */
int glue_C348B2(void);

/* batch 51-52: clipped segment, ground points, voices, messages, observer, stages, long table, alert, start position, typed code */
int glue_C2EE4A(void);
int glue_C2EE4A_step(void);
int glue_C098C6(void);
int glue_C0F4A6(void);
int glue_C25704(void);
int glue_C0915A(void);
int glue_C11078(void);
int glue_C11ACC(void);
int glue_C1803C(void);
int glue_C1803C_step(void);
int glue_C10678(void);
int glue_C0910C(void);
int glue_C25246(void);

/* batch 53: draw-stream commands */
int glue_C212B0(void);
int glue_C212B0_step(void);
int glue_C2129C(void);
int glue_C211DC(void);
int glue_C2131C(void);
int glue_C20E4E(void);
int glue_C20E40(void);
int glue_C21490(void);
int glue_C2139E(void);
int glue_C21412(void);
int glue_C20F10(void);
int glue_C20EC4(void);

/* batch 54: segment grids, block generators */
int glue_C20D68(void);
int glue_C20904(void);
int glue_C21A20(void);
int glue_C217EA(void);

/* batch 55: corner edges */
int glue_C2E758(void);
int glue_C2E758_step(void);

/* batch 56: fixed-row line, text lines and digits */
int glue_C2FA78(void);
int glue_C2FA78_step(void);
int glue_C32726(void);
int glue_C32AB4(void);
int glue_C32AA6(void);
int glue_C32AA4(void);

/* batch 57-58: side face, quad list and strip, face grids and lattices */
int glue_C2159E(void);
int glue_C21060(void);
int glue_C210E6(void);
int glue_C20C38(void);
int glue_C20C22(void);
int glue_C20A52(void);
int glue_C20A40(void);

/* batch 59: cockpit readouts */
int glue_C2F5C0(void);
int glue_C2F5D4(void);
int glue_C31A64(void);
int glue_C31ACC(void);
int glue_C31F4C(void);
int glue_C3201A(void);
int glue_C3212A(void);
int glue_C32178(void);
int glue_C321D2(void);
int glue_C32260(void);
int glue_C31EB6(void);
int glue_C31C60(void);
int glue_C31D16(void);
int glue_C31E6C(void);
int glue_C31D64(void);
int glue_C33F54(void);

/* batch 59b: weapon status, threat lights */
int glue_C2F64E(void);
int glue_C2F63A(void);
int glue_C328A8(void);
int glue_C3112A(void);

/* batch 60: cockpit bars and panel image */
int glue_C30CC4(void);
int glue_C30B5C(void);
int glue_C30D34(void);
int glue_C30EAA(void);
int glue_C309B6(void);

/* batch 60b: compass tape */
int glue_C30F78(void);

/* batch 60c: panel frame */
int glue_C30764(void);

/* batch 61: HUD marks */
int glue_C34146(void);
int glue_C34066(void);

/* batch 61b: target box */
int glue_C342D0(void);

/* batch 61d: ring point, pixel block */
int glue_C2F66E(void);
int glue_C347F2(void);

/* batch 61e: missile cue */
int glue_C33DC8(void);

/* batch 62: message line, display list sort */
int glue_C322EE(void);
int glue_C1E328(void);

/* batch 63: scene startup and command reset leaves */
int glue_C08394(void);
int glue_C090C2(void);
int glue_C090F2(void);
int glue_C1B602(void);
int glue_C0833E(void);
int glue_C133B2(void);
int glue_C118A0(void);
int glue_C083E2(void);
int glue_C25A00(void);
int glue_C30AE2(void);
int glue_C30A00(void);
int glue_C30A00_step(void);
int glue_C30AE2_step(void);
int glue_C17B96(void);
int glue_C2F1C0(void);
int glue_C2EC90(void);
int glue_C2EC94(void);
int glue_C2EC9C(void);
int glue_C2ECA4(void);
int glue_C1FE24(void);
int glue_C1FE46(void);
int glue_C0DAEE(void);
int glue_C0DAEE_step(void);
int glue_C2EC90_step(void);
int glue_C2EC94_step(void);
int glue_C2EC9C_step(void);
int glue_C2ECA4_step(void);
int glue_C2F1C0_step(void);
int glue_C06C02_step(void);

int glue_C17F8C(void);
int glue_C18108(void);
int glue_C1B906(void);
int glue_C0CFFA(void);
int glue_C0CF98(void);
int glue_C13176(void);
int glue_C12098(void);
int glue_C1B27E(void);
int glue_C3316E(void);
int glue_C3316E_step(void);
int glue_C244E2(void);

/* batch 63: postflight HUD */
int glue_C33CD2(void);

/* batch 63b */
int glue_C33B38(void);

/* batch 63c */
int glue_C33370(void);

/* batch 63d: HUD stage */
int glue_C332BC(void);
int glue_C332BC_step(void);

/* polygon to row C7 */
int glue_C301F0(void);
int glue_C301F0_step(void);

/* zone exit */
int glue_C28E28(void);

/* shape */
int glue_C2D16C(void);

/* block face */
int glue_C21500(void);

/* offset run */
int glue_C2122A(void);

/* split square */
int glue_C20592(void);

/* side triangle */
int glue_C2168A(void);

/* square faces */
int glue_C203D0(void);

/* shadow */
int glue_C201A6(void);

/* mark polygon */
int glue_C3019C(void);

/* face test dispatch */
int glue_C1FB82(void);
int glue_C1FB82_step(void);
int glue_C2F490_step(void);
int glue_C1FB8C_step(void);
int glue_C1FB9C_step(void);
int glue_C1FC42_step(void);

/* bound points */
int glue_C1F99A(void);

/* draw_stream.c */
int glue_C1FF0A(void);

/* draw_stream.c */
int glue_C2005C(void);
int glue_C2005C_step(void);
int glue_C2B05A(void);
int glue_C2B05A_step(void);
int glue_C2AB34(void);
int glue_C2AB34_step(void);
int glue_C2AB5A(void);
int glue_C2AB5A_step(void);
int glue_C2AA9C(void);
int glue_C2AA9C_step(void);
int glue_C279D0(void);
int glue_C279D0_step(void);
int glue_C20100(void);

/* draw_stream.c */
int glue_C20002(void);

/* control_records.c */
int glue_C1D3F4(void);
int glue_C1D3F4_step(void);

/* hud_bars.c */
int glue_C3003A(void);

/* control_records.c */
int glue_C28800(void);
int glue_C28800_step(void);

/* scene_setup.c */
int glue_C0924A(void);
int glue_C09266(void);
int glue_C092A0(void);

/* postflight scene callbacks (/) */
int glue_C11788(void);
int glue_C11830(void);

/* post-input heading formatter () */
int glue_C25070(void);

/* scene record stream dispatch () */
int glue_C28B34(void);
int glue_C28B34_step(void);

/* scene record initialization and aim () */
int glue_C28AFE(void);
int glue_C28AFE_step(void);

/* post-input context command and heading marker () */
int glue_C10C68(void);

/* static template bit-gate builder () */
int glue_C1C40C(void);

/* selected-fire record initializer () */
int glue_C2374C(void);

/* scene stream selection and special scene record () */
int glue_C28722(void);
int glue_C28722_step(void);

/* scene initialization and ordered child calls () */
int glue_C0FAA4(void);

/* timer-gated post-input scene transition () */
int glue_C0FA04(void);
int glue_C0FA04_step(void);

/* display-record candidate and selector siblings (/) */
int glue_C0D74A(void);
int glue_C0D74A_step(void);
int glue_C0D752(void);
int glue_C0D752_step(void);

/* signed matrix transform and three returned angles */
int glue_C2DEE0(void);

/* active control-record matrix route */
int glue_C2DB18(void);

/* matrix route selector */
int glue_C2D99C(void);

/* current record matrix update */
int glue_C2D408(void);

/* indexed control-record update */
int glue_C13D84(void);
int glue_C26EBE(void);
int glue_C23CA6(void);
int glue_C23CA6_step(void);
int glue_C0D04C(void);
int glue_C3129A(void);
int glue_C31312(void);
int glue_C31226(void);

/* selected projected segment */
int glue_C1FF9C(void);

/* planar lane mask handlers */
int glue_C2F826(void);
int glue_C2F83A(void);
int glue_C2F844(void);
int glue_C2F84E(void);
int glue_C2F858(void);
int glue_C2F862(void);
int glue_C2F86C(void);
int glue_C2F876(void);
int glue_C2F880(void);
int glue_C2F88A(void);
int glue_C2F894(void);
int glue_C2F89E(void);
int glue_C2F8A8(void);
int glue_C2F8B2(void);
int glue_C2F8EA(void);
int glue_C2F904(void);
int glue_C2F91E(void);
int glue_C2F96C(void);
int glue_C2F986(void);
int glue_C2F9A0(void);
int glue_C2F9BA(void);
int glue_C2F9D4(void);
int glue_C2F9EE(void);
int glue_C2FA08(void);
int glue_C2FA22(void);

/* selected clipped segment sibling */
int glue_C1FFA4(void);

/* Complete placement ordering and packed-cell helper timing. */
int glue_C1E540(void);
int glue_C1E540_step(void);
int glue_C1EBC0_step(void);
int glue_C1EBE0_step(void);
int glue_C1EC3A_step(void);
int glue_C1EC96_step(void);
int glue_C1ECD4_step(void);
int glue_C1ECFC_step(void);

#endif

#include "glue_flight_record_actions.h"
#include "glue_flight_motion_helpers.h"
#include "glue_flight_dynamics.h"
#include "glue_flight_geometry.h"

#include "glue_flight_markers.h"
#include "glue_projection_readouts.h"
#include "glue_hud_stream.h"
#include "glue_hud_parents.h"

#include "glue_hud_readout_parents.h"

#include "glue_hud_text_helpers.h"

#include "glue_hud_projection_parents.h"

#include "glue_hud_history_stream.h"

#include "glue_render_parents.h"

#include "glue_face_stream_parents.h"

#include "glue_face_list_parents.h"

#include "glue_hud_render_parents.h"

#include "glue_render_leaf_helpers.h"
#include "glue_render_entry_helpers.h"
#include "glue_segment_projection.h"

#include "glue_corner_view.h"

#include "glue_control_readouts.h"

#include "glue_record_steering.h"

#include "glue_display_record_selection.h"
