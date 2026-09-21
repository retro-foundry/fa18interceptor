# `process_pending_key_events` at `$C0F3C4` (Hunk 0 +`$1514`)

Classification: **behavioural**, with a deliberately narrow contract. This is
the immediate bounded caller of `keyboard_poll_and_dispatch` in the observed
attract input phase. It is not yet identified as the main loop or a complete
input subsystem.

## Evidence packet

- Restore: `captures/baseline_menu/state.bin`.
- Playback: `local/attract_hud_toggle.e9k`; the `H` press and release were
  delivered during ordinary replay before the breakpoint.
- Breakpoint: `$C0F3C4`, armed at capture frame 450, hit at frame 455.
- Exit: observed return to `$C0EFE4` after 863 stepped instructions.
- Guard: there is no recorded input event after the breakpoint.
- Raw Ghidra P-code: `pcode/raw/h_key_input_phase/` (270 observed RAM starts,
  1,239 P-code operations). Segment annotation maps 262 starts; the eight
  unmapped starts are ROM/OS activity, not silently attributed to game code.

The function first calls `$C16EAE` and `$C1715C`, then enters the observed
poll-and-dispatch sequence at `$C0F43A`. It returns to its caller at `$C0EFE4`.
The helper meanings are unknown.

## Observed queue drain

| Poll call | Raw value at `$C0F440` | Dispatcher | Result |
| ---: | ---: | --- | --- |
| 1 | `$30000025` (low byte `$25`) | `$C1AD74` entered with `$25` | recorded `H` press |
| 2 | `$0000FFA5` (low byte `$A5`) | `$C1AD74` entered with `$A5` | recorded `H` release |
| 3 | `$000000FF` | not entered | termination of this observed drain |

The numeric relationship `$25`/`$A5` is observed for this frontend event pair.
Do not generalize it into a complete raw-key encoding table without more
single-key recordings.

## No-input control

`local/start_demo.e9k` has no event after its menu selection. From the same
restore, breakpoint and return boundary, `$C0F3C4` ran for 145 instructions,
called `$C0F43A` once, received `$FF`, and returned to `$C0EFE4` without
entering `$C1AD74`. Its separately exported P-code is
`pcode/raw/no_key_input_phase/` (118 observed RAM starts, 561 P-code
operations; 117 starts map to a resolved Hunk).

The H-event case therefore adds two dispatcher entries and 718 instructions
within the same bounded phase. This compares two recorded runs; it is not a
claim that every key has the same handler cost.

The key loop's full implementation lies across this caller and
`keyboard_poll_and_dispatch` at `$C0F43A`; the latter’s byte-exact named slice
and dispatcher-state effects are documented in
[`c0f43a_keyboard_poll.md`](c0f43a_keyboard_poll.md).

## Byte-exact source

The complete static input-phase range is now byte-exact source at
`source_amiga/observed/process_pending_key_events.asm`
(`$C0F3C4-$C0F4A5`, 226 bytes). Helper and field names remain bounded by the
two observed packets above.

## Training-parent boundary

From `build/training_focus_600/state.bin`, the same empty-input route returns
from `$C0F3C4` directly to its next parent call at `$C0F5F8` after 145
instructions. Its independently exported P-code is
`pcode/raw/training_600_c0f3c4/` (118 RAM instruction starts, 560 operations,
seven observed call targets). This joins the false-guard parent route without
assuming that it is the global main loop.
