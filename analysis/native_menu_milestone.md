# Native menu continuation

The real path is `port/native/main.c` -> `native_frontend_event` ->
`native_menu_key` -> `select_keyboard_command` (C1AD74) ->
`execute_indexed_command` (C1BC72/C1BD78). `native_frontend_tick` then calls
`native_menu_tick` -> `follow_top_level_menu` (C0FCB4). The original C32CEE
sequencer and C330FE glyph renderer draw the selected source text directly.
No CPU, generated translation, glue, bus or chipset object is linked.

C1017E builds the mission list from the actual pilot-log availability bytes.
The original F1-F4 selection paths accept modes 3-6 for the supplied disk;
a zero qualification word correctly rejects F1. Digit 7 uses the original
next-mission field, not an invented mission progression rule.
An independent source OFF checkpoint at frame 3010 and native selection both
choose mode 6 for digit 7 with the supplied disk record.

C24E8A and `print_number`/C24F76 format the pilot-log fields, including the
source DIVU overflow behavior. C0FE36 consumes log actions; C16406 clears all
39 words, SHIFT-2 requests that reset, and 1 writes the exact 78-byte record.
Saving a reset log returns through enlistment. Escape from the mission list
or log returns to the original main menu. SDL2 function and modifier keys map
at the host boundary to the same SDL1 codes used by reference recordings.

Shared source was separated by ownership: `message_reset.c`,
`menu_available.c`, and `menu_table.c` retain the existing functions; scene
placement remains in `menu_cold.c`. Reference runners compile those same files.
This removes their scene/hardware link dependency from the connected native UI.
Recorder host buffers use the C0EE5C allocation cap `$1E848 / 16`; none of their
contents comes from a captured frame or saved emulator state.

## Validation

Original-source OFF runs start from the read-only ADF, acknowledge credits at
frame 1800, and press the digit at frame 3000. Settled mode-banner frames at
3010 and mission/log frames at 3300 match native output with zero RGB pixel
differences. All seven digests are in `tools/native/check_menu.py`. Reference
images and RAM checkpoints stay in ignored `build/native-menu-reference/`;
they are never runtime inputs. Strict matching here does not claim frame timing;
Copper fade remains excluded from acceptance.

The focused connected check covers digits 1-5, mission/log navigation and return,
F1-F4, the qualification gate, exact reset/save/reload, and ADF immutability.
It also covers digit 7 and the first selection after a newly entered callsign.
Return release must reach C1AD74 to restore its signed event counter after
C32CEE finishes name entry; otherwise the first numbered selection is lost.
That connected first-tour -> Free Flight case matches the source banner.
The existing intro/frontend check also validates pixels, callsign editing,
save/reload, SDL presentation and link omission. Both MSVC and GNU reference
runners build after the shared-source split. No full sealed replay ran.
All fourteen registered CTests pass; the new first-tour selection edge was
also exercised directly after the final Return-release fix.

## Remaining dependency

`NATIVE_MODE_INTRO` stops at the original selection message. C0FECE's delayed
flight scene/root/view children have not been connected. Free Flight's
location/aircraft prompts, cockpit/world renderer and active flight loop remain
the next native work. Audio and original update frequency are still absent;
address-indexed checked host storage is still transitional.
