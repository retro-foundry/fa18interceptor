#ifndef FA18_GAME_GLOBALS_H
#define FA18_GAME_GLOBALS_H

/* Game variables at their original addresses (see memory.h). Names follow
 * the observed use; the routine or capture that established each one is noted
 * beside it. */

/* ---- polygon renderer ------------------------------------------------------
 * A filled polygon is drawn once as a one-bit mask in a scratch buffer, then
 * composited into each bitplane of the draw page according to its colour
 * (run075 frame 393 blit sequence; $C30466, $C304B2). */
#define POLY_MASK_PLANE    0xC456E2u /* long: row 0 of the one-plane mask buffer ($C305AA) */
#define PAGE_PLANE_TABLE   0xC456B6u /* long: address of the draw page's plane-pointer table */
#define PAGE_POINTER_TABLE 0xC456BAu /* long: address of the draw page's second table ($C2F558) */
#define DRAW_PAGE          0xC4566Cu /* word: nonzero when page 1 is the draw page */
#define PAGE0_PLANE_TABLE  0xC4566Eu /* long[4] per page, page 1 follows */
#define PAGE0_POINTER_TABLE 0xC4568Eu /* long[5] per page, page 1 follows */
#define POLY_PLANE_BITS    0xC45956u /* word: colour bits still to composite, one per plane */
#define POLY_MASK_END      0xC45960u /* long: last word of the mask buffer (descending blits) */
#define POLY_MASK_SOURCE   0xC45964u /* long: mask word the compositing blit starts from */
#define POLY_PLANE_OFFSET  0xC45968u /* long: byte offset of the same word within a plane */
#define POLY_BLIT_SIZE     0xC4596Eu /* word: BLTSIZE covering the polygon's bounding box */

/* ---- lines ($C2FA7E) -------------------------------------------------------- */
#define CURRENT_COLOUR     0xC45954u /* word: colour bits of the object being drawn */
#define LINE_LAST_ROW      0xC45984u /* word: last row lines may reach */
#define LINE_PLANES        0xC456E7u /* byte: planes a line is drawn into (bit n: table entry 3-n) */
#define LINE_COLOUR        0xC456E8u /* word: line colour in the low byte; negative: CURRENT_COLOUR */

/* ---- trigonometry ---------------------------------------------------------- */
#define SINE_TABLE         0xC3E5E8u /* word[901]: sin(i/10 degree), 2.14 ($C2E6DA) */

/* ---- sound ---------------------------------------------------------------- */
#define MASTER_VOLUME        0xC4FF26u /* long: 0-63, 16.16; limits every voice ($C501E0) */
#define MASTER_VOLUME_TARGET 0xC45668u /* long: level the master volume fades toward ($C24FE8) */
#define VOLUME_FADING        0xC457D7u /* byte: nonzero while the master volume fades */

/* ---- numerals ------------------------------------------------------------ */
#define DISPLAY_VALUE      0xC45B1Eu /* long: value to show ($C25A08) */
#define DISPLAY_VALUE_BCD  0xC45B22u /* long: the same as eight packed BCD digits */

/* ---- input ---------------------------------------------------------------- */
#define CIAA_PORT_COPY     0xC1839Au /* byte: last CIA-A port A; bit 6 = /FIR0 ($C1715C) */

/* ---- control records ------------------------------------------------------
 * Sixteen 512-byte records at $C46184, selected by index << 9 (memory map). */
#define CONTROL_RECORDS    0xC46184u
#define CONTROL_RECORD_BYTES 512
#define SELECTED_RECORD    0xC459C0u /* word: byte offset of the selected record, or -1 ($C230B0) */
#define SELECTION_ACTIVE   0xC45868u /* byte: cleared with the selection */
#define SELECTION_MARKER   0xC4593Au /* word: set to -1 with the selection */

/* ---- notifications ($C11B44) ----------------------------------------------- */
#define NOTIFY_COUNTDOWN   0xC45890u /* byte: 8..1 cadence counter */
#define NOTIFY_CODE        0xC4588Eu /* byte: code for this step ($86, $06, $04 or 0) */

/* ---- renderer spans ($C310E2) ----------------------------------------------- */
#define SPAN_ORIGIN        0xC45986u /* word: added to a span position (default $32) */

/* ---- screen frame lists ($C0DAA0) ------------------------------------------ */
#define FRAME_POINTS       0xC4B990u /* (x, y) word pairs, 8 bytes apart */

/* ---- view zoom (memory map) --------------------------------------------- */
#define ZOOM_SCALE         0xC45A42u /* word: $20 (wide) .. $80 */
#define ZOOM_FLAGS         0xC457DDu /* byte: bit 7 = zoom at $80 */
#define DISPLAY_UPDATE     0xC4583Du /* byte: display update request */

/* ---- misc ------------------------------------------------------------------ */
#define RECORD_RATE        0xC458BCu /* byte: 5, 3 or 1 from classify_record_rate ($C1C7F6) */
#define LIST_COUNT         0xC46182u /* word: entries in the list at LIST_BUFFER ($C25864) */
#define LIST_WRITE         0xC459CAu /* long: next free entry */
#define LIST_BUFFER        0xC4E2BCu
#define VOICE_TABLE        0xC4FE28u /* long[4]: voice record per channel ($C4FFB4) */

#define CURRENT_RECORD     0xC18210u /* long: address of the current control record (memory map) */

/* ---- post-input sequence (earlier port: post_input_followup) --------------- */
#define POST_INPUT_COUNTDOWN 0xC45AD6u /* word: negative once expired */
#define POST_INPUT_AUX       0xC45795u /* byte */
#define POST_INPUT_EVENT     0xC457AEu /* byte: event flag cleared on completion */
#define VIEWPORT_MODE        0xC458A0u /* byte: current viewport mode */
#define VIEWPORT_TARGET      0xC458A1u /* byte: mode being changed to */
#define STAGE_CALLBACK       0xC1820Cu /* long: routine run by the next update */

/* Routine addresses stored in STAGE_CALLBACK (function pointers once the
 * callers are C). */
#define ROUTINE_COMPLETE_POST_INPUT 0xC0FA80u
#define ROUTINE_AFTER_POST_INPUT    0xC10C08u

/* ---- rounded division ($C25980) ------------------------------------------ */
#define DIVIDE_NUMERATOR   0xC45ACCu /* long */
#define DIVIDE_DENOMINATOR 0xC45AD0u /* word */
#define DIVIDE_QUOTIENT    0xC45AD2u /* word: rounded to nearest, halves away from zero */

/* ---- cockpit display redraw requests (memory map) ------------------------- */
/* One byte per cockpit display (radar range, ECM, weapons, zoom, ...): set to
 * 3 when it must be redrawn. */
#define REDRAW_FIRST       0xC45836u
#define REDRAW_KEEP_STATE  0xC457C0u /* byte: nonzero keeps the two values below */
#define REDRAW_STATE_WORD  0xC458D8u /* word */
#define REDRAW_STATE_LONG  0xC45918u /* long */

/* ---- workspace records (earlier port: context_workspace_flags) ------------ */
#define WORKSPACE_RECORDS  0xC48184u /* sixteen 32-byte records */
#define WORKSPACE_RECORD_BYTES 32

/* ---- player view ---------------------------------------------------------- */
#define VIEW_RECORD        0xC458DEu /* word: offset of the viewed control record */
#define VIEW_MATRIX        0xC45C0Eu /* 3x3 2.14 heading matrix of the viewed record */
#define COMPASS_DEGREES    0xC459A0u /* word: heading in whole degrees */
#define COMPASS_TAPE       0xC458C4u /* word: compass tape position, 0-95 */

/* ---- depth sort ($C1E4A6) -------------------------------------------------- */
#define DEPTH_KEYS_SOURCE  0xC4E778u /* 22 words: keys of this frame, negative = unused */
#define DEPTH_KEYS         0xC4E7A4u /* working copy; values follow at +$2C */
#define DEPTH_VALUES       0xC4E7D0u
#define DEPTH_ORDER        0xC4E828u /* output: values, far to near */

/* ---- post-input sequence, continued --------------------------------------- */
#define CONTEXT_SELECT     0xC45785u /* byte: dispatcher context (memory map) */
#define CONTEXT_STARTED    0xC457B4u /* byte */
#define CONTEXT_STATE      0xC458AEu /* byte */
#define CONTEXT_GATE       0xC458ADu /* byte */
#define CONTEXT_AUX        0xC45788u /* byte */
#define ROUTINE_CONTEXT_STAGE      0xC10C68u
#define ROUTINE_VIEWPORT_CHANGE    0xC11A26u

/* ---- misc ------------------------------------------------------------------ */
#define TABLE_CLEAR_MODE   0xC458A4u /* byte: set to 2 by clear_long_table */
#define LONG_TABLE         0xC45660u /* long: address of a 16-long table */

/* ---- random numbers ($C50AB4) --------------------------------------------- */
#define RANDOM_SEED        0xC07288u /* long: 31-bit shift register */

/* ---- voices, continued ---------------------------------------------------- */
#define VOICE_SLOTS        0xC4FE38u /* long[4]: the voice playing on each channel */

/* ---- readouts ($C2548A) ---------------------------------------------------- */
#define READOUT_SOURCE_VALID 0xC45AFAu /* long: negative until a first sample */
#define READOUT_DIVISOR    0xC45B0Au /* long */
#define READOUT_VALUE      0xC45AE6u /* word: 800000 / divisor, 9999 above $7FFF */
#define READOUT_MINIMUM    0xC45AE8u /* word */
#define READOUT_MAXIMUM    0xC458E0u /* word */
#define READOUT_SAMPLE     0xC45AF2u /* two longs, copied to READOUT_SOURCE_VALID.. */

/* ---- view panning ($C258C8) ---------------------------------------------- */
#define VIEW_PAN           0xC45A94u /* word: 1/80 degree, limited to 270..90 through 0 */
#define VIEW_ROTATE        0xC45A96u /* word: 1/80 degree, full circle */
#define STICK_Y            0xC4582Eu /* byte: joystick up $10, down $20 (pans the view) */
#define STICK_X            0xC45830u /* byte: joystick left $08, right $04 (rotates the view) */
#define PAUSE_A            0xC457ADu /* byte: nonzero blocks panning */

/* ---- mission objects ($C0840E) ------------------------------------------- */
#define MISSION_FLAGS_A    0xC4588Bu
#define MISSION_FLAGS_B    0xC4588Cu
#define MISSION_FLAGS_C    0xC4588Du
#define MISSION_COUNTER    0xC458C2u /* word */
#define MISSION_LEVEL_A    0xC4584Cu /* byte: reset to $10 */
#define MISSION_LEVEL_B    0xC4584Du /* byte: reset to $10 */

/* ---- message sequence (earlier port: message_sequence) -------------------- */
#define MESSAGE_QUEUE      0xC4574Au /* words: queued message codes */
#define MESSAGE_TIMER      0xC4573Eu /* long */
#define MESSAGE_STATE_A    0xC457C6u /* byte */
#define MESSAGE_STATE_B    0xC457C3u /* byte */
#define MESSAGE_STATE_C    0xC457E0u /* byte */
#define MESSAGE_STATE_D    0xC45871u /* byte */

/* ---- misc lookups ---------------------------------------------------------- */
#define MODE_SELECT        0xC458A6u /* byte: $7E/$7F have no table entry */
#define MODE_TABLE         0xC1AB74u /* long: address of a table; bytes from +$12 */
#define STREAM_SKIP        0xC458DAu /* word: low 4 bits = 52-byte records to skip */
#define GRID_ORIGIN_X      0xC4594Cu /* word: low byte = grid column */
#define GRID_ORIGIN_Z      0xC4594Eu /* word: low byte = grid row */
#define DISPLAY_FORCE      0xC458DBu /* byte: bit 0 forces display updates */

/* ---- cockpit display script ------------------------------------------------ */
#define SCRIPT_RECORD      0xC459B6u /* word: offset of the record the script shows */
#define SCRIPT_COUNT       0xC45847u /* byte: bit 7 is a flag; bits 0-6 a repeat count */

/* ---- player record setup ($C09620) ------------------------------------------ */
#define PLAYER_FLAGS_A     0xC45899u
#define PLAYER_FLAGS_B     0xC4589Au
#define PLAYER_FLAGS_C     0xC458B3u
#define PLAYER_FLAGS_D     0xC458B1u
#define PLAYER_FLAGS_E     0xC45889u
#define PLAYER_FLAGS_F     0xC4586Fu
#define PLAYER_FLAGS_G     0xC458B5u
#define PLAYER_LIMIT       0xC45B42u /* word: reset to $7FFF */
#define PLAYER_READY       0xC457D8u /* byte: set to 1 */
#define PLAYER_PHASE       0xC45798u /* byte: set to 4 when nonzero */

/* ---- view octant ($C254E8; earlier port: angle_octant) ---------------------- */
#define VIEW_OCTANT        0xC45854u /* byte: 0-7, 45-degree sector of the view angle */
#define HEADING_ANGLE      0xC45A8Cu /* long: low word is the heading used outside contexts */

/* ---- paired records ($C231A2) ---------------------------------------------- */
#define PAIR_OVERRIDE      0xC457BCu /* byte: one-shot "ready" override, consumed */

/* ---- attitude term ($C148E2) ----------------------------------------------- */
#define REFERENCE_18       0xC456FAu /* long: compared with record field +$18 */

/* ---- cockpit slide animation ($C2559A) ------------------------------------ */
#define SLIDE_ANIMATION    0xC457C0u /* byte: 1-based animation number, 0 = none (also REDRAW_KEEP_STATE) */
#define SLIDE_STEP         0xC4588Fu /* byte: current step */
#define SLIDE_FLAGS        0xC458CCu /* word: bit 10 = animation started */
#define SLIDE_TABLE        0xC25572u /* per animation: long -> step count byte, long -> offsets */

/* ---- position history ($C2651E) ------------------------------------------ */
#define HISTORY_RECORD     0xC4FDD2u /* word: record offset the history follows */
#define HISTORY_COUNT      0xC4FDD0u /* byte: 0-6 entries filled */
#define HISTORY_NEXT       0xC4FDD1u /* byte: 0-5 next slot */
#define HISTORY_SLOTS      0xC4FDD4u /* 12-byte slots: record +$14, +$18, +$1C */

/* ---- square root ($C2564E) ------------------------------------------------ */
#define SQRT_INPUT         0xC45B64u /* long */
#define SQRT_RESULT        0xC45B68u /* word */

/* ---- cell occupancy ($C1D520) --------------------------------------------- */
#define CELL_CHECKS        0xC45864u /* byte: nonzero enables the cell check */
#define CELL_TIMER         0xC45AD4u /* word: set to $50 by the check */

/* ---- record position accumulation ($C1D0B6) ------------------------------- */
#define POSITION_BIAS      0xC45A66u /* long: added to +$18 before scaling */
#define POSITION_LEVEL     0xC45B3Cu /* long: (+$18 + POSITION_BIAS) >> shift */
#define POSITION_VALID     0xC458BBu /* byte: set to 1 */

/* ---- record update tables ($C26428) --------------------------------------- */
#define RECORD_UPDATES_ON  0xC45784u /* byte */
#define TABLE_78_LIMIT     0xC263DCu /* word[31]: by |+$6C| >> 7 */
#define TABLE_76_TARGET    0xC2639Cu /* word[31]: by |+$6E| >> 7 */

/* ---- small text ($C32794) -------------------------------------------------- */
#define SMALL_GLYPHS       0xC3D790u /* word offsets from here, per character from ' ' */
#define ERROR_CODE         0xC4599Eu /* word: set to $46 on a misaligned glyph */
#define TEXT_ALWAYS        0xC45793u /* byte: draw text even while a context runs */

/* ---- sounds ($C17B2C, $C1803C) ------------------------------------------- */
#define SOUND_VOICES       0xC0A438u /* long[]: voice record of each sound */
#define SOUND_FLAGS        0xC45B5Bu /* byte: enables: bit 1 engine, 2 alert tone, 4 noise, 6 scripted */
#define SOUND_ALERT        5         /* the alert tone's sound number */

/* ---- joystick ($C16F1C) --------------------------------------------------- */
#define STICK_RAW          0xC45950u /* word: last JOY1DAT */
#define STICK_Y_HELD       0xC45831u /* byte: an up/down direction is latched */
#define STICK_X_HELD       0xC45832u /* byte: a left/right direction is latched */
#define PLAYER_STICK       0xC461E9u /* byte: player record +$65; bits 4-5 Y, 2-3 X, while paused */

/* ---- matrices ($C2E5AC) ------------------------------------------------- */
#define MATRIX_ROW_SCALES  0xC45A3Eu /* word[3]: per-row scale, 8 fraction bits */

/* ---- attitude ($C122A2) ---------------------------------------------------- */
#define ATTITUDE_A         0xC45A88u /* long: angle, 1/80 degree (>> 3 = tenths) */
#define ATTITUDE_B         0xC45A90u /* long: angle, 1/80 degree */
#define ATTITUDE_LATCH     0xC45786u /* byte: set while angle A is in (60, 315) degrees */
#define ATTITUDE_NEAR      0xC4586Cu /* byte: 1 while angle A is in (45, 320) degrees */
#define ATTITUDE_BAND      0xC4586Bu /* byte: 0-3 by how far angle A is from level */
#define UPDATE_MASK        0xC45858u /* byte: bits $0B set when the latch changes */
#define STATUS_CA          0xC458CAu /* word: bit 1 = exactly one angle past 90 degrees */

/* ---- interrupt server ($C06132) ------------------------------------------ */
#define SERVER_COUNT       0x20      /* word in the server's data: calls so far */

/* ---- clipping and view transform ($C2EA5A, $C1F2EE) --------------------- */
#define CLIP_POINT         0xC45AC6u /* word[3]: where a segment met a side plane */
#define CAMERA_MATRIX      0xC45C20u /* word[9]: camera rotation, 8 fraction bits ($C1F2EE) */

/* ---- magnitude ($C1D974) ------------------------------------------------- */
#define MAGNITUDE_TABLE    0xC1D9D8u /* word[]: sqrt(1 + (i/256)^2), 2.14 */
#define MAGNITUDE          0xC45B40u /* word: the last magnitude3 result */

/* ---- sound programs ($C17EF2, $C18096) ---------------------------------- */
#define PROGRAM_4_VALUES       0xC50B7Cu /* long[]: sound 4 program values, 8 bytes apart */
#define SCRIPTED_SOUND_PROGRAM 0xC50C00u /* sound 11's voice program */

/* ---- world transforms and slots ($C091E0, $C265E8, $C25754) ------------- */
#define RECORD_POSITION    0x14      /* long[3] in a control record: world x, y, z */
#define RECORD_INVERSE     0x92      /* word[9] in a control record: inverse orientation */
#define OBSERVER           0xC45C32u /* observer; long[3] position at +$0C */
#define SLOT_TABLE         0xC45C72u /* 20 slots of 64 bytes */
#define SLOT_COUNT         20
#define SLOT_SIZE          64
#define WORKSPACES         0xC48390u /* renderer workspaces, offset by stream words */
#define NORMALIZED         0xC45A4Cu /* word[3]: normalize_vector's result */

/* ---- renderer buffers, sound mute, point list ($C2FD22, $C3316A, $C25876) */
#define RENDER_BUFFERS_A   0xC456BEu /* long[5]: work buffers, the fifth optional */
#define RENDER_BUFFERS_B   0xC456D2u /* long[5]: work buffers */
#define RENDER_BUFFER_LONGS 2000     /* longs in each */
#define FIFTH_BUFFER_USED  0xC457D6u /* byte: clear the fifth A buffer too */
#define TONE_MUTE          0xC4588Au /* byte: > 0 suppresses tones ($C3316A) */
#define LIST_MATRIX        0xC45BFCu /* word[9]: rotation for list points, 8 fraction bits */

/* ---- target point, record range, lane blit ($C1C2C8, $C24568, $C304FA) -- */
#define TARGET_ENABLED     0xC457ACu /* byte: compute TARGET_POINT */
#define TARGET_POINT       0xC45A62u /* long[3]: $C1C2C8's point */
#define POLY_MAX_Y         0xC45982u /* word: polygon bounds, bottom row ($C301F6) */
#define POLY_MIN_X         0xC4597Cu /* word: left column less one */
#define LANE_PATTERN       0x12ADCu  /* Chip RAM pattern the lane blit reads through C */

/* ---- projection seed, post-input expiry, condition tables ---------------- */
#define POST_INPUT_EXPIRED 0xC457A1u /* byte: set once POST_INPUT_COUNTDOWN runs out */
#define STAGE_AFTER_EXPIRY 0xC10DAEu /* the stage callback installed then */
#define PROJECTION_ORIGIN  0xC45A7Cu /* long[3]: where the projection is seen from */
#define PROJECTION_WORDS   0xC45A72u /* word[3]: TARGET_POINT >> 8 */
#define PROJECTION_Y       0xC45A78u /* long: its y */
#define CONDITION_KEY_A    0xC45948u /* word: first key of the condition tables */
#define CONDITION_KEY_B    0xC4594Au /* word: second key */
#define CONDITION_VALUE    0xC45A78u /* long: compared (negated) with the thresholds */
#define CONDITION_BYTE_A   0xC45850u /* byte: matched against the first byte list */
#define CONDITION_BYTE_B   0xC45854u /* byte: matched against the second */

/* ---- key queue, component bound ($C1BA86, $C1FC42) ----------------------- */
#define VIEW_MODE          0xC457A7u /* byte: selects LINE_LAST_ROW */
#define KEY_TAKEN          0xC457A3u /* byte: a key was queued this update */
#define KEY_RAW            0xC457E1u /* byte[10]: raw key codes */
#define KEY_TRANSLATED     0xC457EBu /* byte[]: translated keys */
#define KEY_WRITE          0xC457F7u /* byte: next KEY_RAW slot */
#define KEY_TRANSLATED_WRITE 0xC457F6u /* byte: KEY_TRANSLATED slot */
#define KEY_COUNT          0xC457F9u /* byte: keys queued */
#define KEY_TABLE          0xC331CEu /* byte[128]: raw key -> translated */
#define KEY_STATE          0xC45878u /* byte[3]: cleared after a key */
#define BOUND_RECORD       0xC45A32u /* long: record whose component is tested */
#define BOUND_SHIFT        0xC45AB8u /* word: shift for the bound offsets */
#define BOUND_OFFSET_X     0xC45B2Au /* word */
#define BOUND_OFFSET_Z     0xC45B2Eu /* word */

/* ---- date line ($C24E2C) --------------------------------------------------- */
#define DATE_LINE          0xC3FD96u /* text: the month name (reversed) and day */
#define MONTH_NAMES        0xC24E08u /* char[4][9] */

/* ---- conditions and selection ($C09A78, $C09A98, $C12242) --------------- */
#define CONDITIONS_A       0xC09B48u /* condition table for CONDITION_MET_A */
#define CONDITIONS_B       0xC09D78u /* condition table for CONDITION_MET_B */
#define CONDITION_MET_A    0xC4589Bu /* byte: 1 when CONDITIONS_A matches */
#define CONDITION_MET_B    0xC4589Cu /* byte */
#define TARGET_RECORD      0xC458DCu /* word: index of the targeted record, 0 = none */
#define SPAN_ORIGIN_Y      0xC45988u /* word */

/* ---- level lists ($C1D5D8) ------------------------------------------------ */
#define LIST_END           0xC45A2Eu /* long: end of the level list being scanned */

/* ---- pixel plots ($C2F5F4, $C2F60A, $C2F66E) ------------------------------ */
#define POINT_XOR_PLANES   0xC456EBu /* byte: planes toggled instead of drawn (LINE_COLOUR >= 0) */
#define PIXEL_MASKS        0xC2F766u /* word[16]: one pixel at x & 15 */
#define PAIR_MASKS         0xC2F7C6u /* word[16]: two pixels ending at x & 15 */
#define PLOT_ROWS_1        0xC2F786u /* long[16]: per-colour one-row writers */
#define PLOT_ROWS_2        0xC2F7E6u /* long[16]: per-colour two-row writers */

/* ---- polygon preparation ($C301F6) --------------------------------------- */
#define POLY_MAX_X         0xC4597Eu /* word: polygon bounds, right column */
#define POLY_MIN_Y         0xC45980u /* word: polygon bounds, top row */
#define POLY_VERTICES      0xC4B390u /* word count, then (x, y) word pairs */
#define POLY_EDGES_LEFT    0xC45970u /* word: edge counter */
#define LINE_STYLE         0xC456E6u /* long: $C456E6-$C456E9 (planes, colour) */
#define KEEP_LINE_STYLE    0xC457A2u /* byte: thin polygons keep the line style */

/* ---- polygon submission ($C2FF48) ----------------------------------------- */
#define POLY_COMPLEMENT    0xC456EAu /* word: bit n complements plane entry 3 - n (with LINE_COLOUR) */
#define POLY_MASK_BLIT     0xC456ECu /* word: nonzero: blit the mask between planes instead */

#endif
