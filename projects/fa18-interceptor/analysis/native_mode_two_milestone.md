# Native mode 2 connection

Digit 3 now passes source mode 2's banner, viewport and two prompts, reaches
aircraft-record-4 playback, then a flight failure and menu return in a normal
disk/input run. Further streams, aircraft resets and complete mode outcomes
are unverified. The full port goal remains active.

The runtime caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE. The native adapter connects C29490's preset,
C10272/C1029E's viewport wait and C102D8/C10302/C10362's return-message path.
The record update parent now connects C233AA for slot 4, C23578 for next-stream
selection and C25704 for stream messages. Existing game C owns their behavior.
This removes the native mode gate and the reached missing-child failures;
no emulator, translated code or CPU bridge is added to the playable link.

Two source comparisons caught defects that a successful run alone missed:

- C0FECE's small frame puts C1E48C's sort-choice byte inside the sort's saved
  A4. C29490 leaves the preset end `$C29872`, making that byte nonzero even
  when the request mask is zero. The mode-2 transition sorts all lists.
- C2F1C0 draws from the last plane pointer plus fixed `$1F40` offsets. The
  former native branch used individual plane pointers. Restoring the source
  addresses resolves six differing drawing bytes in the reached frames.
  Native pages now use the original descending 320x200 plane layout; the
  256-row host surface clears the rows below the source bitmap. Retained
  original demo/carrier RAM independently confirms the order and spacing.
  The entry oracle checks these relative offsets against its original state
  before replacing RAM with a native export.

`native_mode_two_test.c` shares all runtime objects with `fa18_native`. It
starts from the ADF and normal keys: Space at 1800, digit 3 at 3000, Return at
5000 and 6500, with releases two ticks later. No captured state is loaded into
native gameplay. It reaches the menu by tick 10000, with 89 scene/HUD updates.

`check_mode_two.py` passes 32 actual C0F3C4/C0F5F8 intervals, including the
expiring banner and key-driven prompts, and 21 C0EFEA/C0F3C0 bodies sampled at
stage/stream changes. Original instructions execute in a separate reference
process. All compared RAM and drawing bytes match; only original stack,
documented native record locals, and the frame oracle's existing scratch,
asynchronous voice and blitter-poll exclusions apply. No HUD masking or new
rendering exclusions are added. The input oracle selects descriptor contracts
by the original call site, supporting the native runner's null OS handles.

Artifacts are retained in `build/native-flight/mode-check/`: `captures.json`,
`mode_entry-check.log`, `frame_body-check.log` and before/after/source RAM
exports. This demonstrates component correctness and integration for the
sampled route. It does not establish complete sequence parity, audio fidelity,
other modes or whole-game completion. No full original replay was repeated.

Validation also passed the native frontend's original pixels, save/reload,
SDL presentation and link-omission checks; mission/log menu and cockpit-asset
checks; scene-exit and record-expiry integration; active/crash/map source-body
comparisons; 848 input cases and queued-input timing; and twelve reference
host/loader checks. Countermeasure regression also passes 256 full source
parents, four actual collision parents, two recorder-FD input parents and four
keyboard-driven source frame bodies with the corrected page layout.
The reference build still compiles. The delivered
`build/native/fa18_native.exe` runs this mode-2 route through menu return.
