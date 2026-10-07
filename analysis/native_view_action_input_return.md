# Native view-action input return - 2026-10-07

The existing view-command owner now exposes its actual detail, origin level,
record type, view mode and masked zoom flags to native command composition.
These results reach a subsequent depleted recorder flare/chaff command when
queue publication skips. Accepted publication still supplies its actual signed
translated index. Fire-request-only actions preserve their preceding output.

The connected caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command dispatch ->
execute_view_command_result -> existing view/redraw owners. Game behavior stays
in port/game/view_commands.c; composition stays in port/game/native/menu.c.
Legacy event-only APIs delegate to the same implementations. The change removes
the unresolved view-action output dependency at this native input boundary;
it does not establish whole-game acceptance.

## Original contract

- C1B77C-C1B7CC loads the chosen detail value before storing it. The result
  carries that actual detail rather than a later unrelated memory value.
- C1B80A/C1B890 loads the old origin level before its gates. Middle-coordinate
  arithmetic uses D1 and preserves this level; ordinary level changes publish
  their actual wrapped/clamped byte.
- C08324's maximum-zoom setup changes memory without replacing the output.
  C1B9DA-C1BA18 then loads the masked record type, retaining it on the type-$30
  early exit.
- C1BA1C-C1BA60 loads the view mode and retains it on matching-row exits.
  Span calculations at C1BA64-C1BA80 are superseded by the actual view-mode
  load after redraw at C1BA8C.
- C1BB30-C1BB36 loads and masks zoom flags to $7F. Subsequent scale tests and
  bit-seven updates preserve this actual masked result.

ViewActionOutput names these domain values separately from the event being
queued. The implementation uses the existing expressions and child order; it
does not add a CPU register shadow, read original oracle outputs into gameplay,
or reconstruct results from captured state. Unimplemented action families
continue to carry explicit unresolved contracts.

## Connected comparison

All previous control/cleanup cases remain. Twelve new ordinary Free Flight
bodies supply their actual cleanup outputs to intervening view commands and
then first depleted recorder input. These cover detail selection, outside-view
mode changes, origin/middle gates, zoom and a preserving fire request. Full
queues expose the existing publication skip; no native output or clock is
seeded. The original body independently supplies the original keyboard oracle,
whose independently checked output supplies only the original recorder oracle.

139 complete bodies, 156 recorder parents and 36 intervening keyboard parents
match original compared RAM/drawing and defined returns. Raw passing captures
remain temporary. No drawing/HUD comparison exclusions were added.

2,176 focused command parents match all compared non-stack RAM and defined
low-byte outputs: 446 preserve prior output, 144 publish selection, 28 publish
queue indices, 272 publish flight-action values and 1,286 publish view-action
values. No return is unresolved within this selected suite. The added cases
include 832 pending view parents across thirteen actions and 64 settings for
origin/detail gates, signed levels/modes, record types and row limits, plus 512
keyboard zoom parents covering every flag byte in both directions. These
bounded components are separate from complete-body integration and do not
cover every command family.

Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup. Native
Release/Debug and both reference MSVC runners build. Existing 2,512 input
parents, sixteen Delete parents and control/collision comparisons also pass.

The full-port goal remains active. Other action returns, earlier HUD outputs,
complete mission/combat and scenario acceptance, readable typed state, audio
fidelity and measured 20 ms frame performance remain open.
