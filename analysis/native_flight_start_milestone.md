# Native Free Flight startup continuation

The playable native executable now leaves the Free Flight banner and runs
source-owned scene-storage preparation, delayed scene selection, viewport
transition and mode-message publication. Its endpoint is `scene-setup`, with
callback C1072E. This is still before interactive location/aircraft selection
and active flight. Other modes retain their banner endpoint.

Runtime caller: `port/native/main.c` -> `native_frontend_tick` ->
`native_flight_tick` -> `run_post_input_tick` -> C0FECE
`advance_delayed_menu`. Children call the existing scene constructors,
placement-cache and condition owners directly. Subsequent native dispatch calls
C101FC, C10228 and C10678; C32CEE renders their original message queue.
The runner has no CPU, translation, glue, bus or chipset backend. It does
retain checked, address-indexed host data, so typed-state migration is open.

Startup first calls the C08F26 prefix `prepare_scene_storage`: original state
clears, player preparation, initial observer, root placement and template gates.
The original `bootstrap_scene` still calls that prefix followed by its existing
record-update and context-refresh children. Native startup has **not** connected
those two final bootstrap children. C0FECE's later context refresh does run,
but it is not a substitute for the missing record update.

Source table helpers moved unchanged from `stages.c` into `stage_tables.c`;
ordinary renderer workspace stores moved from `render_state.c` into
`renderer_workspace.c`. The input callback's existing palette/countdown tail
is shared as `advance_viewport_palette`; the native host supplies direct
palette presentation. It does not read a simulated mouse register or create
Copper lists. Palette fade remains excluded from acceptance.

Source free-voice state updates execute. Their interrupt acknowledgement is
unneeded in the silent native host; no Paula register is modelled. Native fatal
errors abort with the source error code instead of spinning indefinitely.
There is no native audio playback claim.

## Focused evidence

Original reference: actual ADF launch, `--ports off`, Space at frame 1800,
digit 2 at 3000, RAM output at 3035. Native input uses the same keys; its
source update cadence currently differs, so comparisons are stage/field based.
The native executable receives only the ADF and keyboard input, never reference
RAM or pixels. `--data-out` exports its own host data for diagnostics.

- Initial player coordinates (+14/+18/+1C):
  `10545920 00000708 10A404F0`, exact reference match.
- Initial player angles (+66/+68/+6A): `0000 08C0 0000`, exact match.
- Free Flight observer preset: `10800000 03000000 10C00000`, exact match.
- Scene pose/input bytes: `03 11`, exact match.
- All three 2048-byte template-gate banks match the original exactly;
  their hashes are checked in `tools/native/check_flight_start.py`.
- The placement cache is populated and the native viewport reaches its target
  15 before C10678 publishes its source messages. C1072E remains visible as
  the next unconnected stage.
- A narrow original C1E328 boundary trace confirms the source saved-stack
  alias enables sorting all pending lists in this Free Flight call. The
  native child passes that semantic decision directly, without CPU registers.

Full-state parity is **not** established. At the construction checkpoint,
11 of the first 164 player-record bytes differ: quadrant/classification,
ground height, marker/countdown and geometry-kind fields. Record-grid
selectors and placement-cache contents also differ. These are unresolved
record-update integration, not Copper fade. Root motion and flight pixels
have not been compared. The native UI remains paced at one source update per
host tick; original update frequency still needs its own source-based contract.

Checks: native and both MSVC reference targets build; GNU reference build
passes. Frontend/loader/host CTests, settled menu controls and the new native
flight-start check pass. Shared bootstrap/input owner checks are run at bounded
case counts after the source splits. No full sealed gameplay replay was run.

Flight-start estimate: roughly **50%**, scoped to storage preparation and the
transition/message chain versus the remaining record updates, interactive
choices and rendering. This is an implementation estimate, not a measured
whole-game completion percentage. The gameplay/recorded-run goal remains open.
