# F/A-18 Interceptor reverse engineering

## Current milestone (2026-09-20)

The post-load menu state, deterministic replay host, bounded raw-P-code export,
runtime memory map and first scripted-demo capture are complete. The first
in-flight display packet and a bounded raw-key dispatcher path are imported
into Ghidra. Continue correlating display transactions and input-state paths
with real frames without assigning unsupported gameplay meaning.

Human run `run001` is now sealed and replayable from the menu state: Base 1 /
plane type 1 setup plus takeoff, afterburner, barrel-roll, and sea-crash
coverage. It has 256 events through frame 4,441, a verified ordinary replay
end-state, and single-pass keyframes. See `analysis/run001.md`.

Within the human-flight update path, a real executed indirect edge is now
complete: `$C1CC86 -> $C1EE14 -> $C1CC88`, with 7,866 instructions and 10,404
P-code operations. This corrects a prior stack-only association: `$C1CC86` is
not claimed as the JOY0DAT callback caller. See
`analysis/routines/c1ee14_indirect_stage.md`.

The same dispatcher now has a second complete display target from human flight:
`$C21060 -> $C1F944`, 7,254 instructions and two observed calls to the known
`$C2FA7E` blitter line emitter. It is a display-only contract with no assigned
record/object ownership; see `analysis/routines/c21060_display_target.md`.

Its direct `$C210CE -> $C246A0 -> $C210D4` child is now a complete 7,903-
instruction structural display-stage packet with fully mapped P-code. It
repeats nearby helpers, re-enters through `$C2AFE2`, and reaches the line
emitter twice, without proving an object or geometry type. See
`analysis/routines/c246a0_display_stage.md`.

The first frequent leaf below that stage, `$C2479E -> $C247C0 -> $C247A4`, is
now a complete 33-instruction Hunk-19 tuple packet. It processes three-word
caller tuples through `A2/A3/A4`, calls `$C248B2`, and returns status 1 in the
observed path; tuple meaning remains unassigned. See
`analysis/routines/c247c0_tuple_stage.md`.

Its direct `$C2488C -> $C248B2 -> $C24890` cache leaf is also complete: 13
instructions, fully mapped P-code, and an observed three-word copy through
`A3/A2` with status 1. See `analysis/routines/c248b2_tuple_leaf.md`.

The `$C248B2-$C24981` tuple-cache entry slice is now a 208-byte byte-exact
named 68k reconstruction in `source_amiga/observed/tuple_cache_stage.asm`.
It contains the static interpolation/rounding paths and documents its external
branches. Total symbolic reconstructed bytes: 2,044.

The adjacent `$C24996 -> $C2495A` tuple-cache stage is now bounded in run001:
514 instructions, 132 fully Hunk-mapped P-code starts, and a direct recursive
re-entry through `$C24970`. Its 254-byte `$C24996-$C24A93` observed-entry
slice is byte-exact as `source_amiga/observed/negated_tuple_cache_stage.asm`;
its tuple and output ownership remain structural. See
`analysis/routines/c24996_negated_tuple_stage.md`. Total symbolic reconstructed
bytes: 2,298.

The direct renderer edge `$C24D60 -> $C2FF48 -> $C24D66` is now complete:
969 instructions, 214 observed starts and 1,184 raw P-code operations. It
writes `$8400` to `DMACON`, walks state through `$C301F6`, and reaches the
known `$C2FA7E` blitter line emitter. See
`analysis/routines/c2ff48_display_submission_wrapper.md`; primitive and
list ownership remain unassigned.

The complete `$C2FF48-$C2FF57` wrapper is now a 16-byte byte-exact named
source slice in `source_amiga/observed/enable_dma_and_submit_tuple_list.asm`;
it preserves the real conditional branch to `$C2FF46`. Total symbolic
reconstructed bytes: 2,520.

The renderer child `$C2FF4E -> $C301F6 -> $C2FF56` is independently
complete: 964 instructions, 209 observed starts, and 1,170 raw P-code
operations. It reduces eight observed word-pairs from `$C4B390` to bounds
and takes the `$C302B6 -> $C2FA7E` line-emission path. Its byte-exact
206-byte observed-entry source is `source_amiga/observed/submit_bounded_tuple_list.asm`;
see `analysis/routines/c301f6_bounded_tuple_list.md`. Total symbolic
reconstructed bytes: 2,504.

The `$C21060-$C210E5` human-flight dispatcher target is now a 134-byte
byte-exact source reconstruction in
`source_amiga/observed/dispatch_offset_tuple_records.asm`. It walks a
sentinel-terminated offset list, expands four tuple records, and calls the
known `$C246A0` child while accumulating its result. See
`analysis/routines/c21060_display_target.md`. Total symbolic reconstructed
bytes: 2,654.

The table-driven dispatcher entry `$C1F910-$C1F94D` is now a 62-byte
byte-exact source slice in `source_amiga/observed/record_table_dispatch_entry.asm`.
It names the confirmed sentinel/error, selector-mask, indirect-call, and
status-accumulation protocol while retaining walker branches as external. See
`analysis/routines/c1f942_record_dispatch.md`. Total symbolic reconstructed
bytes: 2,716.

The observed control-record slice `$C1F7A0-$C1F837` is now a 152-byte
byte-exact source reconstruction in `source_amiga/observed/select_record_streams.asm`.
It selects `A1/A2` streams through confirmed control bits and feeds the
established record dispatcher; see `analysis/routines/c1f7a0_select_record_streams.md`.
Total symbolic reconstructed bytes: 2,868.

The enclosing `$C1F6F8` record-walk stage has a stack-proved caller return
`$C30001`, but its first 30,000-instruction no-future-input trace is capped
before that return. Its raw P-code is retained at
`pcode/raw/run001_c1f6f8_record_walk_stage/`; do not treat it as a completed
function. See `analysis/routines/c1f6f8_record_walk_stage.md`.

The frequent renderer edge `$C30324 -> $C305AA -> $C3032A` is bounded as
a four-instruction equal-word return path. Its P-code is in
`pcode/raw/run001_c305aa_renderer_child/`; the unequal continuation remains
unreconstructed. See `analysis/routines/c305aa_renderer_leaf.md`.

The `$C305AA-$C305D5` renderer entry is now a 44-byte byte-exact source
slice in `source_amiga/observed/prepare_unequal_pair_range.asm`, including
the shared equal return and named unequal continuations. Total symbolic
reconstructed bytes: 2,912.

The direct renderer edge `$C30026 -> $C30466 -> $C3002A` is now bounded:
28 instructions, no nested calls, and 141 raw P-code operations. Its
inputs and arithmetic meaning remain structural; see
`analysis/routines/c30466_renderer_child.md`.

The sibling renderer edge `$C3002A -> $C304B2 -> $C3002E` is now bounded:
14 instructions, no nested calls, and 78 raw P-code operations. See
`analysis/routines/c304b2_renderer_child.md`; its effects remain structural.

The `$C304B2-$C304F9` blitter setup leaf is now a 72-byte byte-exact source
reconstruction in `source_amiga/observed/setup_blitter_operation.asm`, with
named OCS blitter registers and busy-wait. Total symbolic reconstructed
bytes: 2,984.

The direct renderer edge `$C2F616 -> $C2F5F4 -> $C2F618` is bounded:
66 instructions, no nested calls, and 351 raw P-code operations. It remains
structural pending input/output correlation; see
`analysis/routines/c2f5f4_renderer_helper.md`.

The companion renderer edge `$C302D6 -> $C2F60A -> $C302DA` is bounded:
69 instructions, no nested calls, and 365 raw P-code operations. See
`analysis/routines/c2f60a_renderer_helper.md`; it remains structural.

A static reference scan identified `$C13E10-$C1414E` as a later candidate
consumer of joystick accumulator `$C45778` and companion field `$C4577C`.
The later isolated replay completes `$C13E10 -> $C25D84`; retain only the
unexecuted branches as static evidence. See
`analysis/routines/c13e10_control_state_stage.md`.

The `$C2F60A-$C2F621` paired-renderer wrapper is now a 24-byte byte-exact
source slice in `source_amiga/observed/submit_adjacent_renderer_values.asm`.
Total symbolic reconstructed bytes: 3,008.

The same stage's direct `$C1CC2E -> $C1D91A -> $C1CC34` child is now a complete
53-instruction fixed-point packet. It reads shifted global words/longword,
uses `$C1D9D8` for scaling, and stores a bounded word at `$C45B40`; its
semantics remain unassigned. See `analysis/routines/c1d91a_fixed_point_stage.md`.

Its `$C1D91A-$C1D9D7` observed-entry slice is now a 190-byte byte-exact named
68k reconstruction in `source_amiga/observed/fixed_point_stage_tail.asm`,
including static alternate paths and its branch to the earlier `$C1D90A` code.
Total symbolic reconstructed bytes: 1,836.

`python scripts/verify_reconstructions.py` is the fast byte-integrity gate for
observed source: it assembles every slice and compares it to the runtime bank
at its `ORG`. It currently verifies all 12 slices / 1,836 bytes in under a
second; use it after every source reconstruction change.

The first human-run input packet is complete: `$C1718E` reads `JOY0DAT`,
normalizes byte-counter deltas, updates and bounds two control accumulators,
then returns through the observed Kickstart callback boundary. Its 83 executed
starts / 490 P-code operations all map to Hunk 0; see
`analysis/routines/c1718e_joy0dat_callback.md`.

The whole `$C1718E-$C17455` callback is now a 712-byte byte-exact named 68k
assembly reconstruction in `source_amiga/observed/joystick_delta_callback.asm`.
Only the JOY0DAT path's contract is behavioural evidence; unobserved static
mode branches retain neutral names. Total symbolic reconstructed bytes: 1,592.

The adjacent `$C17456` registration helper is also byte-exact (54 bytes). It
stores `$C1718E` in the callback descriptor and registers it through `$C53B00`;
this is a static registration edge, while the target itself has independent
live JOY0DAT evidence. See `analysis/routines/c17456_joystick_registration.md`.
Total symbolic reconstructed bytes: 1,646.

Human-flight execution now joins the known long update sequence through the
proved edge `$C0F0A6 -> $C1CB14 -> $C0F0AC`. The `$C1CB14` packet is retained
as a 20,000-instruction cap, not a completed function: its 1,991 observed
starts / 13,883 P-code operations are exported and Hunk-annotated. See
`analysis/routines/c1cb14_flight_update_stage.md`.

## Authority and constraints

- Original disk: `FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf`,
  SHA-256 `e28e94698d68d0d2d3f009a67597a6bb70083e3ff2e4aea6c79369b76f93bfa2`.
  It is never modified.
- Runtime authority: Engine9000 v0.62-alpha, source commit `f9ca09b`, release
  `ami9000.dll` SHA-256 `5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
- Configuration: A500 PAL OCS, 68000 exact timing, 512 KiB Chip RAM plus
  512 KiB slow RAM, Kickstart 1.3, write-protected private ADF. Exact hashes
  and config live in `local/toolchain.json`.
- `GAME.md` separates documented and patch-derived claims from binary evidence.
  `AMIGA.md` provides hardware context. Neither assigns a meaning to a runtime
  address without trace evidence.
- Preserve original bytes and Hunk/segment questions. The final target is
  readable symbolic 68000 assembly; names and constants remain unknown until
  static and runtime evidence agree.
- Keep validation short: narrow replay windows, byte checks and one targeted
  breakpoint. Do not use full-session instruction stepping as a routine check.

## Runtime map

See `analysis/memory_map.md`. The Engine9000 descriptor table and snapshots
confirm exactly two exported RAM banks:

| Range | Bank | Size | Notes |
| --- | --- | ---: | --- |
| `$000000-$07FFFF` | Chip RAM | 512 KiB | DMA-addressable; exported as `chip.bin`. |
| `$C00000-$C7FFFF` | slow RAM | 512 KiB | exported as `slow.bin`; observed menu code executes here. |
| `$BF0000-$BFFFFF` | CIA decode | 64 KiB | I/O, not RAM. |
| `$DFF000-$DFFFFF` | custom registers | 4 KiB decode | captured separately with beam and CPU/Copper provenance. |
| `$FC0000-$FFFFFF` | Kickstart ROM | 256 KiB | pinned ROM; excluded from mutable-bank exports. |

## Established evidence

- `captures/baseline_menu/state.bin` is the post-load menu state. It represents
  original boot frame 11600 and becomes capture frame zero after restore.
- Two isolated 120-frame restores were byte-identical for Chip/slow RAM,
  registers, cycle count, video and audio; total time about one second.
- Native Engine9000 restore and bridge replay produce identical menu pixels.
  A recorded `2` selection was also pixel-identical at frame 300.
- A `1` selection at frames 60/64 reaches Demonstration Flight by frame 600;
  its screenshot shows the flight renderer and HUD. This is the sole setup
  input of the scripted attract capture. No flight-control input is supplied.
- `captures/menu_trace` is the canonical short menu trace: two hardware frames,
  22,068 executed instructions, 443 unique RAM instruction starts (1,756 bytes)
  and 36 observed RAM call targets. It has 181 custom-register writes.
- `pcode/raw/menu/` holds 2,170 raw Ghidra P-code operations with runtime
  address, bank offset, bytes, assembly, flows and structural functions.
  Ghidra's available 68000 family language is a 68040 superset; Capstone 68000
  decoded-length checks constrain the import but do not prove all semantics.
- `captures/attract_run001` is sealed at 9,000 PAL frames (three minutes),
  with only the two recorded mode-selection events and zero human flight-input
  events. Its ending video hash is
  `07049a77bb3f40e99dc3f900af907bc05ab89cf7b396997b6f808894f3906107`.
- Coarse full-frame samples at 600, 1800, 3600, 5400, 7200 and 9000 establish
  changing cockpit/forward-flight visual states from this one deterministic
  recording. See `analysis/attract_timeline.md`; only 600 and 1800 have been
  instruction-traced so far.
- `build/attract_focus_600` traces hardware frames 601-602 in flight: 12,902
  instructions, 1,527 RAM starts, 26 structural call targets and 9,877 raw
  P-code ops. It has 739 custom writes (651 CPU, 88 Copper), including repeated
  blitter control/pointer/data/start-register writes. Its stable P-code export
  is `pcode/raw/attract_600/`; see `analysis/attract_600.md`.
- `$C2FA7E-$C2FD20` is now the first behavioural routine:
  `blitter_draw_line_to_enabled_planes`. Its proven contract is a blitter
  line-mode emission to each enabled bitplane; coordinate semantics and owner
  remain unknown. See `analysis/routines/c2fa7e_blitter_line.md` and the
  byte-exact named-register slice in `source_amiga/observed/` (44 assembled
  bytes match the runtime snapshot).
- The observed upward call path is `$C21304 -> $C2EE4A -> $C2F0F4 ->
  $C2FA7E`; `$C21304` calls `$C2EE4A` 12 times in frame 602. It remains
  structural above the line emitter. See `analysis/routines/attract_display_call_path.md`.
- Later cockpit-frame packet `build/attract_focus_1800` has eight calls to
  `$C2EE4A` and two to `$C2FA7E`, the latter from `$C302B6`. The shared emitter
  is confirmed; caller meanings are still unknown. Its stable Ghidra export is
  `pcode/raw/attract_1800/` (2,810 observed instructions; 19,428 P-code ops).
- `scripts/check_breakpoint.py` pauses at observed candidate `$C0EFD4` within
  60 menu frames. It proves breakpoint plumbing, not routine purpose.
- `source_amiga/chip_ram.asm` and `slow_ram.asm` are byte-exact snapshot-bank
  scaffolds and rebuild with the Amiga assembler on PATH. They are not yet
  semantic reconstruction.

## Capture commands

Repeat or extend the first scripted run (three minutes, no human flight input):

```powershell
python scripts/capture_attract.py --name attract_run001 --frames 9000
```

It writes a sealed capture with `playback.e9k`, initial state/config/toolchain,
state/memory/screen exports and hashes. The two recorded events select mode 1;
`human_input_events` is explicitly zero.

Later interactive run:

```powershell
python scripts/record_run.py --name run001
# close Engine9000 when done
python scripts/finalize_run.py captures/run001
```

Avoid Restore, Rewind, Reset and Warp during a human recording. `F8` is pinned
to Restore only for the launcher setup; it is disabled as Restart.

## Focused analysis workflow

```powershell
python scripts/engine9000_bridge.py --restore captures/attract_run001/initial_state.bin `
  --config captures/attract_run001/config.uae `
  --playback captures/attract_run001/playback.e9k --frames 600 `
  --trace-frames 2 --output build/attract_focus_600
./scripts/import_ghidra.ps1 -Capture build/attract_focus_600 -Project Fa18Attract600
```

Trace packets must state the snapshot, hardware-frame interval, entry/exit PC,
callers, registers/state, custom writes and a narrow assertion. Start with
display-register writers and keyboard dispatch, then work toward the main loop.
Classify every inferred name as structural, behavioural or scenario evidence.

## Known limitations and next checks

- The menu trace includes operating-system and game RAM; ownership is not yet
  separated. No candidate has a gameplay name.
- Single-stepping calls the frontend repeatedly. Held-key/autorepeat parity is
  unverified, so stepped traces with held input are not definitive. Full-frame
  recordings/replays are unaffected. The bridge now rejects a trace window
  containing recorded input instead of silently skipping it.
- An explicit 11-frame `H` tap parity experiment confirmed that input stepping
  is presently invalid: its final Chip RAM and video matched full-frame replay,
  but Slow RAM, registers and cycles differed. Do not use stepped input traces
  until that frontend discrepancy is resolved.
- Full-frame `H` replay is nevertheless deterministic: two independent runs
  match RAM, registers, cycles, video and audio at frame 460. It changes
  game-visible Slow RAM in the demo without changing sampled pixels; see
  `analysis/input/h_key_fullframe.md`. The input consumer remains unknown.
- `$C457E4` changes to `$25` in the `H` full-frame endpoint, but an armed
  Engine9000 write watchpoint does not fire even without source filtering.
  Treat it as an endpoint difference only, not a CPU-writer or keyboard-handler
  address.
- Original executable extraction is now structurally mapped: 185 Hunk segments
  (126 CODE / 32 DATA / 27 BSS). CODE segment 36 maps byte-exactly after 223
  relocations to runtime payload `$C2F490`; it contains the known line emitter.
  Segment 71 maps to `$C45630`, making its documented `+357` state byte
  `$C45795`: `0` in menu snapshots and `1` in observed in-flight snapshots.
  See `analysis/hunk_runtime_mapping.md`.
- Relocation closure from segment 36 resolves 120 loaded original segments with
  no conflicting bases; 101 are byte-verified outside declared relocation
  fields, 13 are runtime-mutated/unverified and six BSS. The authoritative map
  is `analysis/hunk_runtime_resolved.json`.
- P-code is now annotated with original Hunk identities: 1,475/1,527 in-flight
  instructions in `pcode/raw/attract_600`, 2,757/2,810 in
  `pcode/raw/attract_1800`, and 387/443 menu instructions. The remaining rows
  stay explicitly unmapped in each `instructions.segmented.jsonl`.
- The observed Hunk-14 renderer entry is selected through a verified indirect
  dispatch in Hunk 10 (`$C1F942`: masked `D0` indexes `$C1FCE8`, then `JSR (A0)`).
  A live breakpoint at `$C212B0` confirms return PC `$C1F944`; the discriminator
  remains unknown.
- The Hunk-10 dispatcher is shared across attract packets and has five observed
  targets; only `$C212B0` is tied to line emission. See
  `analysis/routines/c1f942_dispatch.md`.
- Keyboard path is now bounded by full-frame control evidence: Hunk 0 +`$158A`
  polls `$C16C56`, treats raw `$FF` as no event, and dispatches non-`$FF` values
  through `$C1AD74`. An `H` tap produces raw `$25` and enters that dispatch;
  the no-input control does not. The 38-byte named source slice is byte-exact;
  see `analysis/routines/c0f43a_keyboard_poll.md`.
- The recorded `H` event has a 99-instruction, no-future-input trace from
  `$C1AD74` to its observed return `$C0F45C`; all starts map to verified Hunk
  5 and raw P-code is in `pcode/raw/h_key_dispatch/`. In the attract state,
  `$C4584B = 3` routes raw `$25` around the static `$25` handler into an
  observed bounded input-history fallback. The handler's control meaning
  remains unconfirmed; see `analysis/routines/c0f43a_keyboard_poll.md`.
- `$C0F3C4` is now a bounded behavioural input-phase caller: a 863-instruction
  post-event trace returns to `$C0EFE4` and drains raw `$25` (H press), `$A5`
  (H release), then `$FF` without dispatch. Its P-code packet has 270 observed
  RAM starts / 1,239 operations and explicit Hunk annotation. See
  `analysis/routines/c0f3c4_input_phase.md`.
- The same `$C0F3C4` boundary has a no-input control packet: 145 instructions,
  one `$FF` poll, no dispatcher entry, and P-code in
  `pcode/raw/no_key_input_phase/`. This isolates the recorded H pairâ€™s two
  dispatches without relying on held-input stepping.
- `$C0EFD4` is now an explicitly capped structural update-sequence packet. Its
  no-input prefix calls `$C0F3C4`, `$C0F5F8`, `$C11B44`, writes `$0008` and
  `$0010` to `$C45AD4`, and reaches costly Hunk-8 callee `$C1C63E`; it has not
  returned after 10,000 instructions. The partial trace is labeled as capped,
  not a completed function. See `analysis/routines/c0efd4_update_sequence.md`.
- The update sequenceâ€™s costly callee `$C1C63E` is now a complete 7,774-
  instruction no-input packet returning to `$C0F01C`. Its 3,095 observed RAM
  starts / 19,758 P-code operations, Hunk annotation, call fanout and 31 direct
  custom-register accesses are recorded without assigning a subsystem meaning;
  see `analysis/routines/c1c63e_update_stage.md`.
- Hunk 32 routine `$C2E6DA` is confirmed as native-angle sine/cosine lookup:
  an 11-instruction call returns `(0, $4000)` for zero input, while static
  quadrant reflection and the verified `$C3E5E8` word table establish the
  signed pair. Its 118-byte named assembly slice is byte-exact and its P-code
  packet is fully Hunk-mapped; see `analysis/routines/c2e6da_sine_cosine.md`.
- Its adjacent Hunk-32 sibling `$C2E5F6` now has a 228-byte byte-exact source
  reconstruction. It independently returns sine/cosine word pairs for `D0`
  and `D2`, and its 20-instruction live call / fully mapped P-code packet are
  documented in `analysis/routines/c2e5f6_two_sine_cosine.md`.
- `$C2E47A` now has a byte-exact 154-byte reconstruction as a three-angle,
  fixed-point 3Ã—3 rotation-matrix builder. Its 106-instruction live packet
  produces identity `$4000`/zero matrix words at `$C45B90` for zero input;
  P-code is fully Hunk-mapped. See `analysis/routines/c2e47a_rotation_matrix.md`.
- Matrix builder user `$C2DEE0` is bounded by a 288-instruction no-input trace
  from `$C2DEE0` to `$C2D704`. It builds `$C45B90`, transforms nine words from
  `$C46204`, and consults `$C3DD92`; all P-code starts map to Hunk 32. The
  vector and data ownership remain unknown; see
  `analysis/routines/c2dee0_matrix_transform_stage.md`.
- The enclosing Hunk-32 sequence reaches `$C2DEE0` through `$C2D700` after
  deriving `A4 = A1 + $80`; it updates `A1+$56-$5A` and uses adjacent flags.
  These are retained as object-layout facts only, without an object-type name.
- Hunk-32 `$C2E5AC` is now a byte-exact 74-byte `scale_matrix_rows` helper.
  It applies the `$C45A3E` three-word vector to the rows at `A1` with signed
  product `>> 8`; a live 28-instruction invocation and fully mapped P-code are
  documented in `analysis/routines/c2e5ac_scale_matrix_rows.md`.
- `$C2E38E` is now a byte-exact 80-byte two-angle matrix builder. A 59-
  instruction call from `$C2DAC2` writes `$C45BD8` and is immediately followed
  by the row scaler; its P-code is fully Hunk-mapped. See
  `analysis/routines/c2e38e_two_angle_matrix.md`.
- The adjacent one-angle builder `$C2E346` now has a 42-byte byte-exact source
  slice and a fully Hunk-mapped 30-instruction packet. It writes the distinct
  `$0100`-axis matrix at `$C45BFC`; see
  `analysis/routines/c2e346_single_angle_matrix.md`.
- The enclosing `$C2D9BA` matrix pipeline is now bounded: 433 instructions
  return to `$C2D9A8`, produce `$C45BD8` and `$C45BFC`, and copy three words
  from `$C461EA` to `$C45A88`. All observed P-code starts map to verified
  Hunks. Its subsystem ownership is deliberately unknown; see
  `analysis/routines/c2d9ba_matrix_pipeline.md`.
- A second bounded wrapper trace proves the upward edge `$C0F02A -> $C2D99C ->
  $C0F030` around the matrix pipeline: 437 instructions, 393 mapped P-code
  starts / 3,099 operations. This joins the known matrix path to the parent
  update sequence without naming the parent a main loop.
- Hunk-20 `$C254E8` is now a byte-exact 102-byte native-angle octant classifier:
  it selects a source angle and stores `0..7` at `$C45854`. A 14-instruction
  live call yields octant 1; this field is consumed at the start of `$C1C63E`.
  See `analysis/routines/c254e8_angle_octant.md`.
- Indirect-dispatch target `$C212B0` is now a complete 2,552-instruction
  display submission packet: ten `$C21304 -> $C2EE4A -> $C2F088 -> $C2FA7E`
  line submissions return to `$C1F944`, with fully Hunk-mapped P-code. Record
  ownership remains unknown; see `analysis/routines/c212b0_display_submission.md`.
- The immediately surrounding `$C1F942` dispatcher is now bounded as a
  table-driven record walk: selectors `$000C`, `$0084`, and `$0034` reached
  `$C2005C`, `$C207FE`, and `$C212B0` in the no-input attract packet. Its
  `$C1FCE8` table and return-status handling are documented without assigning
  a record type; see `analysis/routines/c1f942_record_dispatch.md`.
- Its frequent Hunk-13 target `$C2005C` now has a complete 76-instruction
  packet and raw P-code export: it copies three offset-selected table groups
  into `$C4BF90`-relative storage, calls `$C1FB82`, and returns `$FFFF` in the
  observed attract path. Data and record ownership remain unknown; see
  `analysis/routines/c2005c_dispatch_target.md`.
- The final attract-observed dispatcher target `$C207FE` is a complete
  four-instruction zero return on the nonzero `$C45785` branch; its 14 raw
  P-code operations are fully Hunk-mapped and recorded with the dispatcher.
- `$C1FB82`, the real helper called by the frequent `$C2005C` target, is now a
  complete 41-instruction packet with raw P-code. Its observed path forms a
  fixed-point three-vector cross product, dots it with a third stored triplet,
  and returns a signed orientation result; coordinate and record ownership are
  intentionally not inferred. See `analysis/routines/c1fb82_orientation_predicate.md`.
- The record walk's direct helper `$C1FC42` is now a complete 18-instruction,
  fully mapped P-code packet. It selects one signed shifted record component
  and compares it with a negated global longword, returning condition flags to
  `$C1F780`; alternate selector paths remain unexecuted. See
  `analysis/routines/c1fc42_record_constraint.md`.
- The poll routine's direct caller is `$C0F3C4` in verified Hunk 0; `$C0EFD4`
  is its parent, calling it at `$C0EFE0` before later update calls. `$C0EFD4`
  did not return to call site `$C15DA2` within a 10,000-instruction constrained
  attempt, so retain it as a long-lived structural caller and choose shorter
  edges for the next upward frame-path packet.
- `build/training_focus_600` / `pcode/raw/training_600` capture the result of
  the menu-3 selection (21,853 instructions; 460 observed RAM starts; 2,232
  P-code ops). Its instruction overlap is mostly with the menu trace and the
  screen retains a crack overlay, so this is only a mode-selection probeâ€”not
  proof that the documented training-demo state machine is active.
- The custom-write result â€œno DSKLEN writeâ€ applies only to the two-frame menu
  trace. Keep the disk attached and measure later scenarios before claiming the
  game stops loading.
- Current baseline restores after core initialization because earlier startup
  statefile restore was invalid. The recording epoch begins at restored frame 0.
- After the attract capture, take inexpensive coarse snapshots along the demo,
  then choose short display/keyboard/frame windows based on observed changes.
  Do not trace all 9,000 frames instruction-by-instruction.

The isolated `run001` joystick replay now has a complete bounded control-state packet: `$C13E10 -> $C25D84`, reached at frame 17 after the sole rebased joystick press. It executes 301 instructions, directly consumes `$C45778`, and reaches the observed `$C14044` update of `$C4577C`. The two fields remain axis-neutral; see `analysis/routines/c13e10_control_state_stage.md` and `analysis/memory_map.md`. Raw P-code is retained at `pcode/raw/run001_c13e10_control_state_stage/` (301 starts, 1,854 ops).

The bounded `$C13E10` control-state packet now yields a further byte-exact 44-byte helper at `$C14876-$C148A1`: `source_amiga/observed/apply_parent_delta_to_shared_word.asm`. It applies a caller-frame word delta to word `$26` of the record pointed to by `$C18210`; record ownership remains unknown. Total symbolic reconstructed bytes: 3,052.

The sole non-callback direct `$C45776` reference, `$C31BB6`, was tested against both an isolated post-press interval and the complete original frame-3,195-to-crash continuation (54 rebased events). Neither reaches it; it remains a dead-or-mode-specific static reference, not a control contract.

The second complete child of the bounded `$C13E10` packet is now a byte-exact 254-byte source slice, `$C2641E-$C2651B`, in `source_amiga/observed/update_indexed_shared_record_fields.asm`. The observed entry `$C26428` derives and smooths two indexed record words from table values; record and subsystem ownership remain unassigned. Total symbolic reconstructed bytes: 3,306.

The control-stage return edge is now statically and dynamically joined: `$C25D7E` saves `A1`, calls `$C13D84`, and resumes at `$C25D84`. `$C13D84` selects `$C46184 + ($C459B4 << 9)` and stores the pointer at `$C18210` before the measured `$C13E10` input-state path. See `analysis/routines/c13e10_control_state_stage.md`.

A normal full-frame replay now samples the early held `J 0 5` interval without instruction stepping: `$C45778` and `$C4577C` rise in `$20` word steps to `$03C0`, while `$C45776` remains `$FEF0`. This is one scenario contract, deliberately not an axis name; artifact `build/run001_takeoff_control_samples.json`, documentation `analysis/run001_takeoff_control_samples.md`.

An isolated frame-3,518 `J 0 7` replay reaches the `$C1718E` callback and returns in the same 83 instructions, but its JOY0DAT samples equal the prior bytes. Both deltas are zero and `$C45776/$C45778` remain unchanged. P-code: `pcode/raw/run001_right_joystick_callback/`; this disproves assigning `J 0 7` to either accumulator from this run.

The verified `$C13D84` function-entry prologue is now a byte-exact 140-byte source slice, `$C13D84-$C13E0F`, in `source_amiga/observed/prepare_indexed_control_record_context.asm`. It selects the 512-byte indexed record and prepares all observed local pointers before the input-state branch. Total symbolic reconstructed bytes: 3,446.

The larger indexed-update edge is now bounded: `$C22D88 -> $C25B66 -> $C22D8E` completes in 2,688 instructions and contains the verified `$C25D7E -> $C13D84` joystick-state call plus the observed matrix path. P-code: `pcode/raw/run001_c25b66_update_stage/` (1,803 starts / 11,570 ops); see `analysis/routines/c25b66_indexed_update_stage.md`.

The next complete upward stage is `$C1C6B6 -> $C22C80 -> $C1C6BC`: 3,155 instructions containing the full `$C25B66` packet and therefore the bounded `$C13D84` joystick-state branch. P-code: `pcode/raw/run001_c22c80_record_update_stage/` (2,093 starts / 12,940 ops); see `analysis/routines/c22c80_record_update_stage.md`.

Sealed human `run002` covers Base 4/carrier flight, gear, throttle, weapon fire, chaff, flare, and eject. Two complete replays through frame 7,447 match CPU/RAM/cycles/video/audio. Its first bounded action packet is `K 103` (`G`) -> raw `$24` -> `$C1BC12`, which sets `$C4599A` bit 0 and toggles `$C46200` bit 7; P-code `pcode/raw/run002_gear_key_dispatch/`. See `analysis/run002.md` and `analysis/routines/c1ad74_gear_dispatch.md`.

The sealed run002 Shift+E eject sequence now has a complete raw-key `$12` dispatch packet: it writes `$08` to `$C45842`, sets `$C457AB`, sets `$C4599A` bit 5 and ORs `$0A` into `$C46200`. Its byte-exact 56-byte command slice is `source_amiga/observed/dispatch_eject_command.asm` (`$C1B126-$C1B15D`); P-code `pcode/raw/run002_eject_key_dispatch/`. Total symbolic reconstructed bytes: 3,502.

The sealed run002 `C`/chaff action maps frontend key 99 to raw `$33` and a bounded `$C1C172` dispatch packet. The observed non-exhausted branch sets `$C4599B` bit 2, decrements `$C4584C`, writes `$1E` at `$C4584E` and `$01` at `$C4588B`, then calls `$C25704` with action `$4028`. Byte-exact source: `source_amiga/observed/dispatch_chaff_command.asm` (`$C1C172-$C1C1AF`); P-code: `pcode/raw/run002_chaff_key_dispatch/`. Total symbolic reconstructed bytes: 3,564.

The first sealed run002 Space/weapon-fire press maps key 32 to raw `$40`; its 67-instruction bounded packet calls `$C0833E`. The observed helper route sets `$C4599A` bit 2 and writes `$01` at `$C457BA`; its complete byte-exact 86-byte helper source is `source_amiga/observed/dispatch_space_command_effect.asm` (`$C0833E-$C08393`). P-code: `pcode/raw/run002_space_key_dispatch/`. Total symbolic reconstructed bytes: 3,650.

The preceding sealed run002 Return press maps to raw `$44` and `$C1BB7A`, reaching `$C33186`; its instruction trace reaches the 1,000-instruction cap without returning to `$C0F45C`. Retained P-code `pcode/raw/run002_return_key_dispatch/` is a capped prefix only, documented at `analysis/routines/c1ad74_return_dispatch.md`; it must not be used to assign a completed weapon-selection effect.

The sealed run002 `F`/flare press maps key 102 to raw `$23` and returns through `$C0F45C` in 105 instructions. Its observed `$C1C0E0` route sets `$C4599B` bit 1, decrements `$C4584D`, writes `$1E` at `$C4584F`, latches `$02` at `$C4588B`, and calls `$C25704` with `$4026`. P-code `pcode/raw/run002_flare_key_dispatch/`, documentation `analysis/routines/c1ad74_flare_dispatch.md`; its contiguous handler has not yet been reconstructed as source.

The flare handler's D6=0 command slice is now byte-exact source: `source_amiga/observed/dispatch_flare_command.asm` (`$C1C0E4-$C1C121`, 62 bytes). It preserves the observed action `$4026` and static exhausted action `$4027`. Total symbolic reconstructed bytes: 3,712.

The first sealed run002 throttle-minus press maps frontend key 45 to raw `$0B` and a complete 78-instruction packet. It selects `$C1B5BC`, replaces `$C461E9` low bits with `$02`, calls `$C1B602` to clear `$C45870/$C45778/$C4577C`, then returns through `$C0F45C`. P-code `pcode/raw/run002_throttle_minus_key_dispatch/`; documentation `analysis/routines/c1ad74_throttle_minus_dispatch.md`.

The throttle route's shared byte-exact sources are `source_amiga/observed/select_throttle_mode.asm` (`$C1B5B8-$C1B5DB`, 36 bytes) and `source_amiga/observed/reset_throttle_input_state.asm` (`$C1B602-$C1B615`, 20 bytes). Total symbolic reconstructed bytes: 3,768.

The first sealed run002 `=` throttle press maps frontend key 61 to raw `$0C`, selects `$C1B5B8` and mode `$01`, then takes the same reset route in 76 instructions. P-code `pcode/raw/run002_throttle_plus_key_dispatch/`; documentation `analysis/routines/c1ad74_throttle_plus_dispatch.md`.

The sealed run002 gear command now has byte-exact source `source_amiga/observed/dispatch_gear_command.asm` (`$C1BC12-$C1BC4F`, 62 bytes). It includes the observed request-bit set/toggle route and its static `$C33186` side-effect alternative. Total symbolic reconstructed bytes: 3,830.

The raw `$44` Return route's first callee is now bounded despite the earlier top-level cap: `$C33186 -> $C1BBC0` completes in 1,924 instructions with no future stepped input. Its P-code is `pcode/raw/run002_return_c33186/` (147 observed RAM starts / 1,025 ops), documented at `analysis/routines/c33186_return_inner_packet.md`. It is a complete inner packet only; subsystem role and outer Return effect remain unassigned.

The bounded keyboard parent phase is now full byte-exact source: `source_amiga/observed/process_pending_key_events.asm` (`$C0F3C4-$C0F4A5`, 226 bytes). It preserves the measured empty-poll and recorded key-drain paths, including calls to `$C16C56` poll and `$C1AD74` dispatch. This is the current concrete input-to-parent-loop source boundary. Total symbolic reconstructed bytes: 4,056.

The complete `$C1C63E` update-stage packet now has a byte-exact 126-byte setup prefix: `source_amiga/observed/prepare_c1c63e_update_stage.asm` (`$C1C63E-$C1C6BB`). It synchronizes observed stage fields and calls the already-bounded `$C22C80` indexed-record stage; field meanings remain neutral. Total symbolic reconstructed bytes: 4,182.

The upstream parent-loop prefix is now byte-exact source: `source_amiga/observed/run_parent_update_prefix.asm` (`$C0EFD4-$C0F01B`, 72 bytes). It records the direct input-phase call followed by pre-update helpers, measured `$C45AD4` stage markers, and the complete `$C1C63E` update-stage call. Total symbolic reconstructed bytes: 4,254.

The next parent-loop slice is now byte-exact source: `source_amiga/observed/run_parent_update_middle.asm` (`$C0F01C-$C0F08F`, 116 bytes). It establishes the observed update order after `$C1C63E`, including `$C2D99C` matrix pipeline and `$C254E8` angle-octant calls, before the later conditional stage. Total symbolic reconstructed bytes: 4,370.

The next conditional flight-update range is now byte-exact source: `source_amiga/observed/run_parent_flight_update.asm` (`$C0F090-$C0F123`, 148 bytes). It preserves the `$C45A66` guard, `$C1CB14` flight-stage call, and distinct measured `$A4/$A8/$AC/$B0` branch markers before joining the next parent stage. Total symbolic reconstructed bytes: 4,518.

The following parent-loop post-flight setup range is now byte-exact source: `source_amiga/observed/run_parent_postflight_setup.asm` (`$C0F124-$C0F1E1`, 190 bytes). It selects `$C46184 + ($C458DC << 9)`, examines offset `$62`, and executes activity-gated helper calls while preserving neutral field names. Total symbolic reconstructed bytes: 4,708.

The subsequent activity-gated parent block is now byte-exact source: `source_amiga/observed/run_parent_activity_stages.asm` (`$C0F1E2-$C0F2A7`, 198 bytes). It preserves three local activity/counter gates, thirteen helper calls, and the signed `$C45837` decrement before joining `$C0F2DC`. Total symbolic reconstructed bytes: 4,906.

The parent-loop tail is now byte-exact source: `source_amiga/observed/run_parent_update_tail.asm` (`$C0F2A8-$C0F3C3`, 284 bytes). The six adjacent parent source slices now cover the full static `$C0EFD4-$C0F3C3` routine (1,008 bytes), including the input-phase call and tail return. Its dynamic packet remains capped; no complete runtime-return claim is added. Total symbolic reconstructed bytes: 5,190.

The static caller above that parent update now has a byte-exact outer-loop slice: `source_amiga/observed/run_outer_update_loop.asm` (`$C15D80-$C15DB3`, 52 bytes). `$C15DA2` calls `$C0EFD4`; after `$C53FC0/$C1612C`, `$C15DB2` loops to `$C15D96`, excluding the initial `$C0E78A` delay call. See `analysis/routines/c15d80_outer_update_loop.md`. Total symbolic reconstructed bytes: 5,242.

The outer loop's final `$C1612C` child is now a complete no-input packet: it hits at frame 607 and returns to `$C15DB2` in 1,022 instructions. Canonical P-code is `pcode/raw/no_key_c1612c_outer_child/` (45 starts / 207 ops); see `analysis/routines/c1612c_outer_loop_child.md`. This bounds the pre-back-edge child without assigning its subsystem or timing role.

The `$C1612C` outer-child setup is now byte-exact source: `source_amiga/observed/prepare_outer_loop_child.asm` (`$C1612C-$C1617D`, 82 bytes). It uses `$C4566C` to select pointer-table entries at `$C182BA/$C182C2` before the later bounded activity path. Total symbolic reconstructed bytes: 5,324.

The `$C1612C` outer-child tail is now byte-exact source: `source_amiga/observed/run_outer_loop_child_tail.asm` (`$C1617E-$C16283`, 262 bytes). Together the two adjacent slices cover the complete static 344-byte `$C1612C` child, which is independently bounded to `$C15DB2` in the no-input packet. Total symbolic reconstructed bytes: 5,586.

`analysis/run003_plan.md` now specifies the next focused human recording: HUD, map, radar, targeting, hook, ECM J/K comparison, airbrake, rudder, pitch/roll, zoom, keypad views, and optional pause. It uses two-second gaps to keep later breakpoint packets bounded and avoids already-covered run002 controls.

Human `run003` is sealed with 401 post-setup events through frame 8,455. Its two ordinary full replays match registers, cycles, Chip/Slow RAM, video, and audio; end-frame video hash is `b410609316e32d265decf677a4982af5768e4d61400bf818ff75fd9d7dfb241d`. Actual event inventory is `analysis/run003.md`: H/M/R/T/A/J/K, comma/period, brackets, pause, repeated Space/Return, plus unresolved frontend code-291/code-0 windows and joystick/mouse coverage. Keyframes are `build/run003_keyframes/`.

The outer loop's per-iteration `$C2F558` child is now complete in the no-input demo: it returns to `$C15D9C` in nine instructions. Its byte-exact source is `source_amiga/observed/select_outer_loop_pointer_pair.asm` (`$C2F558-$C2F581`, 42 bytes), selecting/publishing a pointer pair from `$C4566E/$C4568E` based on `$C4566C`. P-code `pcode/raw/no_key_c2f558_outer_child/`; documentation `analysis/routines/c2f558_outer_pointer_selector.md`. Total symbolic reconstructed bytes: 5,628.

The first isolated run003 input trace confirms `H`: frontend `K 104 104 16 1`
becomes raw Amiga `$25`, branches to `$C1B264`, and toggles the two-state HUD
field `$C457A1`. The byte-exact source is
`source_amiga/observed/toggle_hud_mode.asm`; P-code is
`pcode/raw/run003_h_key_dispatch/`; detailed evidence is
`analysis/routines/c1b264_toggle_hud_mode.md`. Total symbolic reconstructed
bytes: 5,654.

The run003 `M` press maps to raw `$37`, reaches `$C1BF8C`, and sets
`$C4599C` bit 0 before a capped OS/display path. `R` maps to raw `$13`, reaches
`$C1B1C8`, and calls `$C33186`; its isolated child returns to `$C1B1CE` in
1,924 instructions and is preserved in `pcode/raw/run003_r_command_child/`.
The post-call code sets `$C4599A` bit 6. These raw mappings and effects are in
`analysis/run003.md`; map/radar labels still rest on GAME.md until their
visual effects are independently compared.

The run003 `T` trace is now bounded through its shared child: raw `$14` enters
`$C1B1A4`, `$C33186` returns to `$C1B1AA` in 1,942 instructions, then the
handler sets `$C4599A` bit 7 and `$C457B9 = 1`. Byte-exact source:
`source_amiga/observed/request_next_target.asm`; P-code:
`pcode/raw/run003_t_command_child/`. `A` maps to raw `$20` and directly sets
`$C4599A` bit 1 at `$C1B616`; its longer child remains to be isolated.

The run003 ECM ambiguity is resolved: `J` maps to raw `$26` and executes the
bounded `$C1C1F2` mode toggle, setting `$C4599B` bit 3, writing `$C45840 = 3`,
and toggling `$C458B5`; source is `source_amiga/observed/toggle_ecm_mode.asm`.
`K` maps to raw `$27`, has no command branch in this state, and only reaches
the generic queue. P-code is `pcode/raw/run003_j_command_child/` and
`pcode/raw/run003_k_key_dispatch/`. Total symbolic reconstructed bytes: 5,730.

The run003 held rudder inputs are bounded and reconstructed. Comma is raw
`$38` and selects `$80`; period is raw `$39` and selects `$40`. Both update the
high input bits of `$C461E9` and publish `$C4582F`; the adjacent zero-mask
entry is the static release/reset form. Source:
`source_amiga/observed/set_rudder_input_mask.asm`; P-code:
`pcode/raw/run003_comma_key_dispatch/` and
`pcode/raw/run003_period_key_dispatch/`. Total symbolic reconstructed bytes:
5,772.

The run003 bracket controls are bounded and reconstructed in
`source_amiga/observed/adjust_zoom_scale.asm`. `]` is raw `$1B` and reduces
`$C45A42` toward `$20`; `[` is raw `$1A` and increases it toward `$80`.
Both request display-update mode 3 and set/clear `$C457DD` bit 7 at the `$80`
endpoint. P-code is in the corresponding `run003_left_bracket` and
`run003_right_bracket` packets. Total symbolic reconstructed bytes: 5,904.

Run003 `P` maps to raw `$19` and enters `$C1C06E`, calling `$C0F4A6` under the
observed flight-state guards. Both the 1,000-instruction dispatcher and a
separate 3,000-instruction `$C0F4A6` child packet remain capped, so no
complete pause/resume implementation claim is made. P-code is retained at
`pcode/raw/run003_p_key_dispatch/` and `pcode/raw/run003_p_pause_helper/`;
details are `analysis/routines/c1c06e_pause_command.md`.

The exact restored-state run003 tuple `K 291 0 16 1` is native F10. It becomes
raw Amiga `$59`, reaches the function-key throttle range, and takes the bounded
`$C1BD04` level-application path. Byte-exact source:
`source_amiga/observed/apply_function_key_throttle_level.asm`; canonical
P-code: `pcode/raw/run003_f10_exact_keyboard_poll/`. F10 derives level 10,
stores `$79` at `$C45870`, and can publish scaled values to `$C45778/$C4577C`
under its observed active gate. The earlier F2/raw-`$50` label was incorrect.
Total symbolic reconstructed bytes: 6,020.

`build/run003_post_f10_first/state.bin` is now the approved scripted-control
base state. It restores the sealed run003 frame-3,887 state, replays only the
first F10 tuple, and reaches frame 3,899 with `$C45870 = $79` and
`$C45778/$C4577C = $0050`. Its derivation is documented in
`analysis/run003_post_f10_first.md`.

The post-F10 state now supports a full native F1--F10 differential probe.
It establishes `$C45870` values `$0C,$18,$24,$30,$3C,$48,$54,$60,$6C,$79`
for F1--F10 respectively. The first nine are `12n`; the F10 top threshold
adds one. See `analysis/function_key_levels.md` and
`analysis/post_f10_function_key_probe.json`.

`captures/run014_post_f10_functions/` is the deterministic native replay from
that state: F1--F10 are 90 frames apart, F10 repeats after a clean 180-frame
gap, and the replay completes at frame 5,519. Its first rebased F1 event has a
390-instruction no-future-input trace from `$C0F43A` to `$C0F45C`; it supplies
raw `$50` to `$C1BD04` and stores `$0C` at `$C45870`. Canonical P-code is
`pcode/raw/run014_f1_keyboard_poll/`; details are in
`analysis/run014_post_f10_functions.md`.

The preceding function-key gate is now byte-exact source:
`source_amiga/observed/route_function_key_level_input.asm`
(`$C1BC50-$C1BD03`, 180 bytes). It joins the already reconstructed `$C1BD04`
level calculation, preserves all alternative state routes under neutral names,
and raises verified symbolic reconstruction to 78 slices / 10,230 bytes. See
`analysis/routines/c1bc50_function_key_gate.md`.

The adjacent throttle-mode tail is now byte-exact source:
`source_amiga/observed/finish_throttle_mode_update.asm`
(`$C1B5DC-$C1B601`, 38 bytes). It connects the existing selector and reset
helper, retains its exact status/mode updates and shared fallback, and raises
the verified source total to 79 slices / 10,268 bytes. See
`analysis/routines/c1b5dc_throttle_mode_tail.md`.

Two additional keyboard-dispatcher gaps are byte-exact source:
`route_target_input_to_shared_handler.asm` (`$C1B1BE-$C1B1C7`, 10 bytes) and
`finish_eject_command_state.asm` (`$C1B15E-$C1B1A3`, 70 bytes). The latter
continues the observed eject path through its state update and `$C1BA86`
handoff. Verified total: 81 slices / 10,348 bytes; see
`analysis/routines/c1b15e_eject_state_tail.md`.

The `$C1B21C-$C1B263` command continuation is now byte-exact source in
`dispatch_space_and_mode_commands.asm` (72 bytes). It preserves the
state-gated Space helper route, adjacent helper call, three-state word cycle,
and shared fallback. Verified total: 82 slices / 10,420 bytes; see
`analysis/routines/c1b21c_space_mode_continuation.md`.

`set_input_state_signed_flag.asm` now covers the 24-byte `$C1C224-$C1C23B`
bridge into the shared queued-command routine. It maps the observed `D6` state
to the exact `$01/$FF` flag values at `$C4582A`. Verified total: 83 slices /
10,444 bytes.

`queue_fire_request_action.asm` covers the 20-byte `$C1BB66-$C1BB79` command
tail. It sets the observed `$C4599D` bit-6 request and action byte `$05`, then
joins `$C1C23C`. Its per-key meaning remains unassigned pending a bounded raw
key trace. Verified total: 84 slices / 10,464 bytes.

`refresh_buffered_command_state.asm` covers `$C1C1B0-$C1C1F1` (66 bytes): a
queue-only entry plus a guarded `$C4FDxx` pointer refresh that latches `$03`
at `$C4584B` and `$01` at `$C4582A`. The producer and gameplay meaning remain
unassigned. Verified total: 85 slices / 10,530 bytes.

`dispatch_direct_command_keys.asm` covers the 184-byte `$C1AE28-$C1AEDF`
raw-key comparison table reached after the preceding command-context gates.
It structurally routes Space, M/N/R/T, Backspace/Tab, throttle keys, eject,
radar, gear, hook, flare, chaff, ECM, HUD, target, and map requests to their
handlers without claiming untraced controls are behavioural evidence.
Verified total: 86 slices / 10,714 bytes.

`dispatch_alternate_context_keys.asm` reconstructs `$C1AEE0-$C1AF7B` (156
bytes), including the alternate raw-key routes and the five raw codes that
structurally join the `$C1BB66` fire-request tail. Per-control meaning remains
limited to isolated-event evidence.
Verified total: 87 slices / 10,870 bytes.

`dispatch_function_and_indexed_keys.asm` covers `$C1AF7C-$C1B00F` (148
bytes): the raw `$45` bridge, context gates, F1--F10 range routing, and the
eight-key `D4` index table. Only the native F1--F10 route has bounded dynamic
evidence; the indexed-key mapping is retained as structural fact.
Verified total: 88 slices / 11,018 bytes.

`dispatch_fallback_command_keys.asm` covers `$C1B010-$C1B037` (40 bytes):
four raw-key checks and a final fallthrough into the nonzero-context table.
The two raw release keys structurally reach `$C1BB66`; remaining control
semantics await isolated traces.
Verified total: 89 slices / 11,058 bytes.

`dispatch_nonzero_context_keys.asm` covers `$C1B038-$C1B0F1` (186 bytes):
the mode-2 special route, paired raw-code routes, and three state-latch
families. Its release-code paths that bypass state writes are represented as
explicit jumps to `$C1C2B6`. Verified total: 90 slices / 11,244 bytes.

`finish_nonzero_context_key_dispatch.asm` completes `$C1B0F2-$C1B125` (52
bytes), including the mode gate and Return-only clear of `$C457D3` before the
common command queue.
Verified total: 91 slices / 11,296 bytes.

`gate_keyboard_command_dispatch.asm` adds the `$C1AD74-$C1AE27` (180-byte)
entry and context gates. Together with the adjacent verified slices, the
keyboard command dispatcher is now contiguous from `$C1AD74` through
`$C1B125`. `$C458AD` and `$C458AE` are retained as distinct observed fields.
Verified total: 92 slices / 11,476 bytes.

`dispatch_guarded_context_helper.asm` fills `$C1C122-$C1C171` (80 bytes):
the context-6/record-bit guarded helper call and its common queue tail. The
helper role remains structural. This makes `$C1C122-$C1C223` contiguous with
the adjacent flare, chaff, and ECM command handlers. Verified total: 93
slices / 11,556 bytes.

`dispatch_context_state_routes.asm` fills `$C1C052-$C1C0E3` (146 bytes):
the mode-`$7D` toggle, guarded context update, alternate state entry, and the
flare pre-gate into `$C1C122`. The verifier distinguished the adjacent
`$C458B0` negative-value target from `$C457AE`. Verified total: 94 slices /
11,702 bytes.

`jump_from_input_dispatch.asm` covers `$C1C2B8-$C1C2BD`, the non-returning
input-dispatch transfer to `$C06BF0`. Verified total: 95 slices / 11,708 bytes.

`write_two_bit_control_field.asm` covers `$C1B4D0-$C1B4F3` (36 bytes): three
entries select a low two-bit value for `$C461E9`, with the zero entry clearing
`$C45870`. Verified total: 96 slices / 11,744 bytes.

`write_high_nibble_control_field.asm` covers `$C1B4F4-$C1B53F` (76 bytes):
three selections `$20/$10/$00`, their `$C4582E` latch, and the conditional
bits-5--4 update of `$C461E9`. Verified total: 97 slices / 11,820 bytes.

`write_mid_nibble_control_field.asm` covers `$C1B540-$C1B58D` (78 bytes):
the matching `$08/$04/$00` selector family, `$C45830` latch, and conditional
bits-3--2 update of `$C461E9`. Verified total: 98 slices / 11,898 bytes.

`update_three_axis_control_bytes.asm` covers `$C1B410-$C1B4CF` (192 bytes):
the shared three two-bit direction fields at `$65(A1)` and bounded signed-byte
updates at `$28/$29/$2A(A1)`. Steps are 1, 1, and 3 with verified clamps.
Physical-control binding remains trace work. Verified total: 99 slices /
12,090 bytes.

`gate_three_axis_control_update.asm` covers `$C1B3EA-$C1B40F` (38 bytes):
the bit-4/word gate that clears, disables, or enters the three-axis bounded
update. Verified total: 100 slices / 12,128 bytes.

The existing sealed run003 frame-5 bounded trace at `$C1B27E` is now
canonicalized as `pcode/raw/run003_c1b27e_control_update/` and documented in
`analysis/routines/c1b27e_control_update_trace.md`. Its 65 instructions prove
the normal branch through `$C1B340`, `$C1B4D0`, `$C25A6A`, and the all-masks-
clear path of `$C1B410`; it does not promote unexecuted branches to semantics.

`update_control_field_from_record_delta.asm` covers `$C1B340-$C1B3E9` (170
bytes), with the same bounded trace proving its positive-delta-one branch to
the low-two-bit value-one helper. Threshold behavior is static unless traced.
Verified total: 101 slices / 12,298 bytes.

`advance_control_record_stream.asm` fills `$C1B27E-$C1B33F` (194 bytes): the
mode-1--3 byte/word stream advance, wrapping and `$FF` sentinel detection.
Together with adjacent slices this makes `$C1B27E-$C1B58D` contiguous verified
source. The existing frame-5 trace proves only its mode-zero branch. Verified
total: 102 slices / 12,492 bytes.

`dispatch_context_special_request.asm` covers `$C1B664-$C1B6C1` (94 bytes):
the `D6`-zero request/state route and queue transfer; the nonzero branch goes
to the still-separate calculation path at `$C1B6C2`. Verified total: 103
slices / 12,586 bytes.

`select_context_request_slot.asm` covers `$C1B77C-$C1B7EF` (116 bytes): the
0--9 request-slot selector table, its request-bit updates, and common queue
tail. Verification corrected the slot-1 zero-context branch to `$C1B906`.
Verified total: 104 slices / 12,702 bytes.

`decrement_context_selection.asm` covers `$C1B7F0-$C1B88F` (160 bytes): the
context byte decrement/`$FF` wrap and the alternate `$C45C42` `$02000000`
step with `$01000000` lower clamp. Verified total: 105 slices / 12,862 bytes.

`increment_context_selection.asm` covers `$C1B890-$C1B8F7` (104 bytes): the
complementary `$C45C42` `$02000000` increment with `$08000000` upper clamp and
the shared-byte increment/wrap path. Verified total: 106 slices / 12,966 bytes.

`select_context_command_index.asm` covers `$C1B8F8-$C1B9CB` (212 bytes): the
context command-index selection/decrement/increment entries and request-bit
routes. Verification corrected reset branches to the internal `$C1B906` entry.
Verified total: 107 slices / 13,178 bytes.

`publish_context_command_selection.asm` covers `$C1B9CC-$C1BAD3` (264 bytes):
the common selection publisher, record-type gate, `$C1BAD4` lookup use, and
output-word update paths. Verification corrected the `$A7/$B3` choice branch.
Verified total: 108 slices / 13,442 bytes.

`context_output_lookup_table.asm` covers `$C1BAD4-$C1BAE1` (14 bytes), the
exact signed-byte lookup table consumed by the shared context publisher.
Verified total: 109 slices / 13,456 bytes.

`calculate_context_special_vectors.asm` covers `$C1B6C2-$C1B77B` (186 bytes):
the signed-table split, fixed-point vector arithmetic, helper calls, and state
retry/queue tail. A no-input frame-6000 breakpoint probe did not hit it within
120 frames, so its meanings remain structural. Verified total: 110 slices /
13,642 bytes.

`prepare_indexed_context_state.asm` covers `$C1BEE8-$C1BF77` (144 bytes): the
`D1`-derived record index, helper call, state split, and special-type output
route. Verified total: 111 slices / 13,786 bytes.

`dispatch_map_command_prelude.asm` covers `$C1BF78-$C1BFBB` (68 bytes): two
queue entries and the context-gated prelude that falls into the existing map
initializer at `$C1BFBC`. Verified total: 112 slices / 13,854 bytes.

`select_indexed_command_mode.asm` covers `$C1BD78-$C1BE5F` (232 bytes): the
zero-context indexed mode selector and validation path. Exact source retains
the binary's non-canonical zero-compare and explicit-zero-displacement forms.
Verified total: 113 slices / 14,086 bytes.

`select_alternate_context_record.asm` covers `$C1BE60-$C1BEE7` (136 bytes):
the alternate-context table scan, state-gated queue preparation, retry entry,
and mode adjustment. This closes the final gap in a contiguous reconstructed
keyboard command-dispatch span `$C1AD74-$C1C2BD`. Verified total: 114 slices /
14,222 bytes.

`analysis/memory_map.md` now records the dispatcher-specific request flags,
mode/index bytes, adjacent `$C458AD/$C458AE` gates, request slot, and clamped
`$C45C42` selection value from the contiguous reconstruction.

`analysis/keyboard_command_dispatch.md` is the dispatcher handoff dossier:
contiguous source map, raw-key table regions, shared queue boundaries, and the
distinction between sealed dynamic evidence and static routing facts.

`invoke_context_helper_four_times.asm` covers `$C0F4A6-$C0F4D5` (48 bytes),
the parent-side helper that invokes `$C17B08` with longword arguments 0--3.
Verified total: 115 slices / 14,270 bytes.

`initialize_parent_update_context.asm` covers `$C0F4D8-$C0F569` (146 bytes):
parent context initialization and installation of `$C0F812` in `$C1820C`.
Verified total: 116 slices / 14,416 bytes.
Routine contract: `analysis/routines/c0f4d8_initialize_parent_update_context.md`.

`parent_update_alignment_word.asm` records the `$C0F4D6-$C0F4D7` zero word
between parent routines. The coverage ledger was corrected to match verified
source. Verified total: 117 slices / 14,418 bytes.

The run003 frontend code-0 input windows are not game-key evidence. An exact
isolated replay of `K 0 0 16 1` from the frame-5,999 snapshot did not reach
the raw-key dispatcher `$C1AD74` in 24 replay frames. Keep them classified as
an Engine9000 frontend recording/mapping limitation, not keypad or cursor
controls.

The run003 `A` command is now bounded through its post-helper tail. In the
observed `$C461E6 = $11` context, it sets `$C4599A` bit 1, toggles
`$C46186` bit 15 and `$C45847` bit 7, writes `$C45845 = 3`, then calls the
complete `$C25704` command-word publisher. Byte-exact source:
`source_amiga/observed/toggle_arrestor_hook.asm` and
`source_amiga/observed/publish_command_word_flags.asm`; evidence:
`analysis/routines/c1b616_toggle_arrestor_hook.md`. Total symbolic
reconstructed bytes: 6,168.

The run003 `R` command is now bounded through its post-helper tail. Raw `$13`
calls `$C33186`, then sets `$C4599A` bit 6 and cycles the low nibble at offset
`$63` of `$C46184 + $C458DE` through 9, 11, and 13 before requesting display
mode 3 at `$C4583B`. Byte-exact source:
`source_amiga/observed/cycle_radar_range.asm`; P-code includes
`pcode/raw/run003_r_command_child/` and the pending post-helper export.
Total symbolic reconstructed bytes: 6,252.

The run003 `M` map entry remains dynamically capped in `$C0F4A6`, but its
immediate post-helper static tail is byte-exact source:
`source_amiga/observed/initialize_map_transition_state.asm`
(`$C1BFBC-$C1C051`, 150 bytes). It initializes transition vectors, counters,
timer `$C45A94 = $1C20`, flags, and display code `$B3`; see
`analysis/routines/c1bf8c_map_command.md`. No helper-return claim is made.
Total symbolic reconstructed bytes: 6,402.

The shared command return at `$C1C23C` is now byte-exact source:
`source_amiga/observed/enqueue_command_input_event.asm` (124 bytes). It
queues non-release raw events in `$C457E1`, translates through `$C331CE` into
`$C457EB`, advances observed indexes/count, and clears `$C45878-$C4587A`.
It is reached by the bounded run003 HUD/rudder/zoom/radar/ECM/hook paths; see
`analysis/routines/c1c23c_enqueue_command_input_event.md`. Total symbolic
reconstructed bytes: 6,526.

The first bounded parent helper after input is now traced: `$C0F5F8` returns
to `$C0EFEA` in 38 no-future-input training-frame instructions. Its observed
64-byte tail is exact source at
`source_amiga/observed/run_post_input_tick_tail.asm`: it updates a tick byte,
decrements `$C45AD6`, calls `$C1820C -> $C1075A`, and clears the command-input
pending flag. P-code: `pcode/raw/training_600_c0f5f8/`; details:
`analysis/routines/c0f5f8_post_input_tick.md`. Total symbolic reconstructed
bytes: 6,590.

The next direct parent helper `$C11B44` is bounded to `$C0EFF0` in nine
training-frame instructions. Its complete 108-byte static routine is now
`source_amiga/observed/update_periodic_notification_code.asm`: it decrements
and reloads `$C45890`, selecting observed notification codes at `$C4588E`.
P-code: `pcode/raw/training_600_c11b44/`; evidence:
`analysis/routines/c11b44_periodic_notification.md`. Total symbolic
reconstructed bytes: 6,698.

The actual training false-guard route now has a bounded `$C12950` selector:
it returns to `$C0F376` in 27 instructions after publishing
`$C46184 + ($C458DC << 9)` at `$C18210`. The frame-600 state takes its
`$C45795 = 0` early return. Byte-exact observed prefix:
`source_amiga/observed/select_training_control_record.asm`; P-code:
`pcode/raw/training_600_c12950/`; documentation:
`analysis/routines/c12950_control_record_selector.md`. Total symbolic
reconstructed bytes: 6,792.

The isolated run003 `J 0 4` event reaches `$C1718E` and completes the known
83-instruction JOY0DAT callback to `$FC134C`. Its samples and observed control
accumulators are unchanged; only `$C45774` increments. This is a second
no-delta frontend code case, documented in
`analysis/routines/c1718e_joy0dat_callback.md`, with P-code at
`pcode/raw/run003_joy4_callback/`.

The next executed training-parent call is now bounded: `$C53F9C` preserves
`A6`, passes its caller-pushed longword through `D0`, and calls the `-$19E`
vector of the library base at `$C182CA`. It returns to `$C0F37E` after 53
instructions, including the ROM/library boundary. The 20-byte game-RAM wrapper
is byte-exact source at
`source_amiga/observed/invoke_library_lvo_19e_with_argument.asm`; the vector
is intentionally unnamed. P-code: `pcode/raw/training_600_c53f9c/`; evidence:
`analysis/routines/c53f9c_library_vector_wrapper.md`. Total symbolic
reconstructed bytes: 6,812.

The following parent-tail helper is also bounded: `$C2B3C2` compares
`$C458A6` with one and, in the training frame-600 state, branches to the
adjacent `$C2B3C0` return. The packet is three instructions and returns to
`$C0F386`; its canonical P-code is `pcode/raw/training_600_c2b3c2/`. The
12-byte observed early-return prefix is byte-exact source in
`source_amiga/observed/check_tail_mode_one.asm`; the mode-one continuation is
not assigned. See `analysis/routines/c2b3c2_tail_mode_check.md`. Total
symbolic reconstructed bytes: 6,824.

The next direct training-parent child `$C32CEE` returns to `$C0F3C0` in a
complete 24-instruction no-future-input packet. It selects a word from
`$C4574A` through `$C457C6`, writes `$C45772`, passes observed state guards,
then sees a zero raw-event byte in `$C457E1[$C457F8]` and returns. P-code is
canonical at `pcode/raw/training_600_c32cee/`; see
`analysis/routines/c32cee_command_event_packet.md`. This is a bounded dynamic
contract only; no speculative full-function reconstruction was added.

The existing byte-exact `$C0F3C4` input phase has an independent training-frame
boundary now: with no future input it runs 145 instructions and returns to the
next direct parent call at `$C0F5F8`. Canonical P-code is
`pcode/raw/training_600_c0f3c4/` (118 RAM starts, 560 operations, seven
observed call targets). This confirms the false-guard parent sequence across
input into the already bounded post-input tick without widening its main-loop
claim.

The normal in-flight parent route is now entered from sealed run003 frame
6,000, where `$C45795 = 1`. `$C12098` returns to `$C0F008` in 25
no-future-input instructions, selecting the current 512-byte control record
and taking its bit-1-clear return path. Its canonical P-code is
`pcode/raw/run003_6000_c12098/`. The complete `$C12098-$C12241` static routine
is byte-exact source in `source_amiga/observed/prepare_normal_update_state.asm`;
the map now records its directly accessed control fields. Total symbolic
reconstructed bytes: 7,250.

The immediate normal-route `$C25B1E` child is an observed two-byte `RTS`: it
returns from `$C0F010` to `$C0F016` in one instruction. Canonical P-code is
`pcode/raw/run003_6000_c25b1e/`; source is
`source_amiga/observed/normal_update_empty_stage.asm`; see
`analysis/routines/c25b1e_normal_update_empty_stage.md`. Total symbolic
reconstructed bytes: 7,252.

A new direct run003 frame-6,000 trace confirms `$C1C63E` remains a heavyweight
stage in the normal update route: it does not return to `$C0F01C` inside the
3,000-instruction cap. Its first live child `$C22C80` likewise does not return
to `$C1C6C0` within the same cap. Both capped packets are retained under
`build/run003_6000_c1c63e/` and `build/run003_6000_c22c80/`; do not treat them
as complete functions. The next narrow work item is an inner return-bounded
child selected from these packets, not a larger oracle.

That next inner trace is complete: in run003 frame 6,000, `$C22D88` calls
`$C25B66` and it returns to `$C22D8E` after 2,603 instructions with no future
input. Canonical P-code is `pcode/raw/run003_6000_c25b66/` (1,831 RAM starts,
12,079 operations and 29 observed call targets). It is recorded as the bounded
structural indexed-record packet in `analysis/routines/c25b66_indexed_record_stage.md`;
its next work item is a direct nested child, rather than an unsupported whole
function claim.

Within that packet, `$C25C70 -> $C1B27E -> $C25C76` is now independently
bounded at 65 instructions. Its run003 frame-6,000 route takes the
`$C4584B <= 0` branch and local helper `$C1B4D0` replaces `$C461E9` low bits
with one; `$C25A6A` immediately returns in this state. Canonical P-code is
`pcode/raw/run003_6000_c1b27e/`; see
`analysis/routines/c1b27e_control_record_input_stage.md`.

The nested `$C25A6A` gate is independently bounded and reconstructed. In the
same frame `$C45790 = 0`, so it returns through `$C25A68` to `$C1B3E8` in three
instructions. P-code is `pcode/raw/run003_6000_c25a6a/`; byte-exact source is
`source_amiga/observed/check_record_stage_enable.asm`; see
`analysis/routines/c25a6a_record_stage_gate.md`. Total symbolic reconstructed
bytes: 7,262.

The next matrix-side child in the same run003 frame-6,000 record packet is
also complete: `$C25D9E -> $C2D408 -> $C25DA4` takes 872 instructions with no
future input. Canonical P-code is `pcode/raw/run003_6000_c2d408/` (732 RAM
starts, 5,411 operations, eight targets). See
`analysis/routines/c2d408_matrix_side_stage.md`; the next trace should isolate
its direct `$C1342C` child.

That child is now complete: `$C2D618 -> $C1342C -> $C2D61E` takes 103
instructions with no nested calls. Canonical P-code is
`pcode/raw/run003_6000_c1342c/` (103 starts, 487 operations); see
`analysis/routines/c1342c_matrix_side_leaf.md`. Its arithmetic and record
ownership remain structural.

The `$C1342C` packet is self-contained on its observed route. It publishes the
current indexed record at `$C18210`, selects static table `$C3D690` for index
zero, and returns through `$C13A22`; no nested call is present. Its enclosing
static routine is much wider than the 103-instruction route, so no partial
source slice is claimed. The memory map now records its directly accessed
fields and the routine note captures the exact boundary.

The sibling record-side stage is now complete: `$C25E26 -> $C149BE ->
$C25E2C` takes 313 instructions with no future input. Its canonical P-code is
`pcode/raw/run003_6000_c149be/` (277 starts, 1,921 operations, three targets).
The observed nested path is `$C14A54 -> $C25754 -> $C1D974`; see
`analysis/routines/c149be_record_side_stage.md`.

The first nested record-numeric stage is now bounded:
`$C14A54 -> $C25754 -> $C14A5A` takes 84 instructions. P-code is
`pcode/raw/run003_6000_c25754/` (82 starts, 683 operations). Its direct child
`$C1D974` does not return to `$C25784` within the 3,000-instruction cap and is
retained under `build/run003_6000_c1d974/`; see
`analysis/routines/c25754_record_numeric_stage.md`.

The capped `$C1D974` path has a complete inner numeric packet:
`$C1CF2C -> $C1D91A -> $C1CF32` returns in 1,141 instructions with no future
input. Canonical P-code is `pcode/raw/run003_6000_c1d91a/` (520 starts, 3,920
operations, six targets). It joins existing byte-exact
`fixed_point_stage_tail.asm`; see
`analysis/routines/c1d91a_run003_numeric_packet.md`.

Another direct `$C25B66` child is complete: `$C2600E -> $C26EBE -> $C26014`
returns in 815 instructions with no future input. Canonical P-code is
`pcode/raw/run003_6000_c26ebe/` (219 starts, 1,275 operations, eight targets);
see `analysis/routines/c26ebe_record_side_stage.md`. Its record/render
ownership remains structural.

The `$C26EBE` packet's game-RAM route is self-contained through its `$C279C0`
return and has no direct game `JSR`; its exported OS callback activity belongs
to ordinary frame execution. The routine note now records that boundary so the
eight importer targets are not mistaken for its source-level child calls.

The direct `$C25B66` indexed-record edge `$C1484C -> $C26428 -> $C14852` is
now independently bounded at 62 instructions with no nested calls. Canonical
P-code is `pcode/raw/run003_6000_c26428/` (62 starts, 506 operations). It is
fully contained in the existing byte-exact `update_indexed_shared_record_fields.asm`
source slice; see `analysis/routines/c26428_indexed_record_update.md`.

The remaining direct record-selector edge from `$C25B66` is now bounded:
`$C25D7E -> $C13D84 -> $C25D84` returns in 370 instructions with no future
input. Canonical P-code is `pcode/raw/run003_6000_c13d84/` (370 starts, 2,216
operations, two targets). It extends the existing byte-exact indexed control
record selector source; see `analysis/routines/c13d84_run003_record_selector.md`.

The completed `$C13D84` selector packet has one observed direct game call:
`$C1484C -> $C26428`, already independently bounded and covered by byte-exact
`update_indexed_shared_record_fields.asm`. This closes the direct selector to
updater edge for the run003 frame-6,000 route.

`analysis/control_record_layout.md` now consolidates the evidence-backed
512-byte `$C46184 + (index << 9)` record offsets used across selector, numeric,
and update paths. It is deliberately an observed-access layout, not a claimed
complete object definition, and is linked from the memory map for reuse in
future byte-exact source names.

The capped normal matrix packet now has a complete direct child:
`$C2DB2E -> $C2E346 -> $C2DB32` returns in 38 instructions with no future
input. Canonical P-code is `pcode/raw/run003_6000_c2e346_matrix_child/` (38
starts, 338 operations), confirming existing byte-exact
`build_single_angle_matrix.asm` on the normal route. `$C2D99C` and `$C2DB18`
remain capped; see `analysis/routines/c2e346_normal_matrix_constructor.md`.

The normal matrix constructor edge `$C2E348 -> $C2E6DA -> $C2E34C` is now
return-bounded from the run003 frame-6,000 snapshot: it hit on frame 5 and
reached the caller continuation after 2,397 instructions with no future input.
Canonical P-code is `pcode/raw/run003_6000_c2e6da_matrix_lookup/`; see
`analysis/routines/c2e6da_normal_matrix_lookup.md`. The packet contains normal
frame/callback activity, so its imported targets are not treated as lookup
children. The actual static lookup remains the byte-exact
`lookup_sine_cosine.asm` (`$C2E6DA-$C2E74F`).

`analysis/run003_recorded_inputs.md` now records the actual run003 playback
inventory rather than assuming the coverage plan was followed. It confirms
menu traffic, HUD/map/radar/target/hook, J/K ECM experiments, the observed F10
throttle route, gear, rudder, zoom, pause, and a later Space/Return/mouse/
joystick window.
The zero-valued frontend key entries are explicitly left unlabelled pending
raw-key-dispatch evidence; they are not claimed as cursor coverage.

The complete three-angle matrix composer `$C2E3DE-$C2E479` is now byte-exact
source in `source_amiga/observed/compose_three_angle_matrix.asm` (156 bytes;
total symbolic reconstruction: 7,418 bytes). It scales three native angles,
uses the existing two-angle and one-angle trigonometry helpers, and writes nine
matrix words. Its mixed fixed-point `MULS`/`SWAP` sequence is retained as
observed; see `analysis/routines/c2e3de_compose_three_angle_matrix.md`.

The adjacent normal-update helper `$C2E370-$C2E38D` is now byte-exact source
as `build_single_angle_trig_matrix.asm` (30 bytes; total symbolic source:
7,448 bytes). Its observed `$C2DB12` caller loads a control-record angle; the
helper retains native trig scale with fixed axis `$4000`. See
`analysis/routines/c2e370_single_angle_trig_matrix.md`.

The terminal `$C2D9BA` matrix-pipeline block `$C2DAE0-$C2DAF1` is now
byte-exact `copy_matrix_auxiliary_words.asm` (18 bytes; total symbolic source:
7,466 bytes). It copies three words from `$C461EA` to `$C45A88` without a
stronger field-role claim; see `analysis/routines/c2dae0_copy_matrix_auxiliary_words.md`.

The matrix pipeline's `$C2D996 -> $C2E514` child is now byte-exact source:
`compose_alternate_three_angle_matrix.asm`, `$C2E514-$C2E5AB` (152 bytes;
total symbolic source: 7,618 bytes). It is intentionally distinct from the
neighbouring three-angle composer because the observed product/sign order
differs. See `analysis/routines/c2e514_alternate_three_angle_matrix.md`.

The bounded matrix-pipeline parent is now byte-exact source:
`update_matrix_pipeline.asm`, `$C2D9BA-$C2DADF` (294 bytes; total symbolic
source: 7,912 bytes). It retains its observed record selection, coordinate
update call, cache state, and direct two-angle/row-scale/one-angle builder
sequence. The one VASM-incompatible stack adjustment is emitted as the original
`ADDA.W #$18,A7` opcode. See `analysis/routines/c2d9ba_matrix_pipeline.md`.

The parent-facing matrix dispatcher `$C2D99C-$C2D9AF` is now byte-exact
`dispatch_matrix_update_route.asm` (20 bytes; total symbolic source: 7,932
bytes). `$C45785` selects the existing `$C2D9BA` or `$C2DB18` route, then the
routine returns to the parent at `$C0F030`; see
`analysis/routines/c2d99c_matrix_route_dispatch.md`.

The alternate `$C2D99C` route is now byte-exact source:
`update_control_record_matrix_route.asm`, `$C2DB18-$C2DCC1` (426 bytes; total
symbolic source: 8,358 bytes). It preserves active-record angle loading,
table/literal tuple selection, temporary matrix construction/scaling, and the
`$C2DEE0` record transform call. Its run003 packet remains capped; static
source does not promote the state fields to game-level meanings. See
`analysis/routines/c2db18_control_record_matrix_route.md`.

The CPU prefix of the verified line emitter is now byte-exact source:
`prepare_blitter_line_parameters.asm`, `$C2FA7E-$C2FB4D` (208 bytes; total
symbolic source: 8,566 bytes). It derives row offset, direction, Bresenham
error terms, line control and trigger values before the existing `$C2FB4E`
custom-chip wait/write slice. See
`analysis/routines/c2fa7e_blitter_line_setup.md`.

The hardware suffix of the `$C2FA7E` line emitter is now byte-exact source:
`submit_blitter_line_to_active_planes.asm`, `$C2FB7A-$C2FD21` (424 bytes;
total symbolic source: 8,990 bytes). It conditionally submits the prepared
line to bitplanes 0--3, waiting on DMACONR before each BLTSIZE trigger. See
`analysis/routines/c2fb7a_blitter_line_plane_submission.md`.

The `$C2FA70-$C2FA77` equal-axis fallback is now byte-exact
`handle_equal_line_axis.asm` (8 bytes; total symbolic source: 8,998 bytes).
It increments both axis registers, clears the line span word, and rejoins the
reconstructed line-parameter path at `$C2FA9C`.

Run003's first late Space event is now independently bounded from a frame-7,500
restore: it reaches `$C1AD74` as raw `$40` and returns in 64 instructions.
Canonical P-code is `pcode/raw/run003_space_first_dispatch/` (64 starts, 254
operations). It sets `$C4599A` bit 2 and reaches the common queue tail; unlike
the run002 route, `$C461E7`'s zero high nibble makes `$C0833E` return without
writing `$C457BA`. See `analysis/run003.md`.

Run003's first Return event is now independently bounded from a frame-7,700
restore: raw `$44` reaches `$C1BB7A` and returns in 2,042 instructions. Its
observed default route calls `$C33186`, sets `$C4599A` bit 4, cycles the high
nibble of `$C461E7`, requests value 3 at `$C45843/$C45844`, and clears
`$C458B4`. The byte-exact `$C1BB7A-$C1BC11` handler is
`cycle_weapon_selection.asm` (152 bytes; total symbolic source: 9,150 bytes).
Canonical P-code: `pcode/raw/run003_return_first_dispatch/`.

The memory map now includes the raw `$44` Return route: `$C4599A` bit 4,
`$C46200` low-nibble gate, `$C461E7` high-nibble `$10`-step/wrap update,
`$C45843/$C45844 = 3`, and `$C458B4 = 0`. These remain observed command fields,
not a complete weapon-state definition.

Run004 is sealed: 259 events through frame 3,854. Two ordinary full replays
match registers, cycles, Chip RAM, Slow RAM, savestate, video and audio; the
end video hash is `af537f18fc54318a052288f386c6157afefc672666e14ae0fa277021a6fcd8c2`.
`analysis/run004.md` records its stationary Base-3 weapon experiment: Return
windows at 2,471--2,560 and 2,793--2,882, first Space at 2,596, later Spaces at
2,932--3,314. Next target is the first rebased Space packet.

Run004 now has two isolated stationary Space packets. The frame-2,596 raw `$40`
packet returns in 83 instructions with `$C461E7` high nibble `$10`, setting
`$C458C6` bit 3. The frame-2,932 raw `$40` packet returns in 67 instructions
with high nibble `$30`, writing `$C457BA = 1`. Both set `$C4599A` bit 2. P-code
is `run004_first_fire_dispatch` and `run004_second_fire_dispatch`; the
run report and memory map retain these as distinct selected-fire states pending
an ammunition/object consumer trace.

The run004 `$30` selected-fire flag now has a bounded same-frame consumer:
`$C22DB4 -> $C2374C -> $C22DC8`, 214 instructions and no direct calls.
`$C2374C` clears `$C457BA`, sets `$C457B7/$C457C5`, writes `$C45797 = 8`,
`$C458B0 = $FB`, `$C45843/$C45844 = 3`, and `$C45858 = $8C`. Canonical P-code
is `pcode/raw/run004_c2374c_second_fire_consumer/`; see
`analysis/routines/c2374c_selected_fire_consumer.md`. It proves immediate
request consumption, not yet a specific ammunition/object identity.

The memory map now records only direct `$C2374C` run004 consumer effects:
`$C457B7`, `$C45797`, `$C458B0`, `$C457C5`, and `$C45858`. Its static body
extends through `$C23A24`; the observed 214-instruction route has no direct
calls, while its separate static `$C2574A` edge is unobserved.

`analysis/weapon_fire_state.md` now keeps the stationary run004 fire evidence
in one constrained table: `$10` sets `$C458C6` bit 3 with consumer open;
`$30` sets `$C457BA`, then reaches the bounded `$C2374C` request consumer.
The player's gun-then-missile report is kept distinct from the binary-level
state labels.

The `$30` selected-fire consumer is now fully reconstructed as
`consume_selected_fire_request.asm`, `$C2374C-$C23A25` (730 bytes). It clears
the request, validates and copies a selected record, selects counters,
configures its fields, applies fixed-point orientation math, and conditionally
calls `$C2574A`. This establishes a record initializer but still does not name
the resulting object. All observed source verifies: 75 slices, 9,880 bytes.

The current Engine9000 CPU watchpoint helper did not trap on the known
`$C458C6` write in the isolated first-fire packet, so its read misses are not
recorded as absence of execution. Breakpoint-plus-single-step traces remain the
authority for fire-path claims.

Run004's `$C23A7E` immediate selected-record continuation is now bounded
(46 instructions, `$C23A7E -> $C23F4A -> $C22DCC`) and has canonical P-code at
`pcode/raw/run004_c23a7e_second_fire_followup/`. Its byte-exact mode-zero tail,
`update_mode_zero_record_motion.asm` (`$C23F4A-$C23FC1`, 120 bytes), counts
down/gates the record and steers heading toward a selected target. All observed
source verifies: 76 slices, 10,000 bytes.

The first run004 Space branch (`$C461E7` high nibble `$10`) was bounded for
1,200 frames in a packet that deliberately contains Space down but not its
later release. `$C458C6` becomes `$0008` and remains there, while
`$C4599A/$C457BA` sample as zero. In the full run the bit is clear after Space
up at frame 2,723, so it is a held-input latch in this evidence, not a
persistent selected-fire mode. All ten static CPU references to `$C458C6` were
breakpoint-tested and not reached in the held-input window. The release packet
does not reach `$C1AD74` or `$C083A0`; its frame-level clear path is open.
`analysis/weapon_fire_state.md` has the exact evidence; the broken CPU
watchpoint helper is not used as negative evidence.
`scripts/check_replay_breakpoints.py` now makes this multi-PC bounded sweep
reusable and writes a compact JSON result per capture.

The actual Space-up event at run004 frame 2,723 now has a bounded input-phase
trace: `$C0F3C4 -> $C0EFE4`, 1,221 instructions, canonical P-code at
`pcode/raw/run004_space_release_input_phase/`. It does not reach `$C1AD74` or
`$C083A0`; `$C458C6` remains `$0008` through `$C16EAE` and the following
parent-update return at `$C15DA2` (99,613 steps). The later clear is therefore
still outside the bounded parent route. `find_memory_transition.py` is a new
reusable no-future-input tool that records the first write to a selected RAM
value or a supplied control-flow boundary; its JSON report is
`analysis/run004_space_release_parent_latch.json`.

`sample_input_state.asm` now reconstructs the observed `$C1715C-$C1718D`
input-state sampler (50 bytes). It builds a two-bit result from `$C1839A` bit 6
and JOYDAT (`$DFF016`) bit 10, then returns it to the input phase. All observed
source verifies: 77 slices, 10,050 bytes.

`analysis/run005_plan.md` now documents the scripted replacement for the
blocked interactive function-key experiment. It uses the compatible in-flight
state and native Engine9000 replay events. Keypad views remain deferred:
enabling PUAE's Keyrah keypad mapping changes core configuration and makes the
existing baseline savestate incompatible; top-row digits are not substituted.

Run008 first exposed a restore-package filename mismatch after a per-capture
config experiment; run009 showed that even a matching package cannot restore
the baseline when PUAE options differ. `record_run.py` is restored to its
known-good original core config/save pairing (`local/fa18.uae` and
`fa18.uae.e9k-save`).

Run010 was aborted: Engine9000 still consumed the function keys and the numeric
keypad did not reach the game. The failed `--keypad-mappings` invocation was
removed from the recording interface because it would require a different core
configuration and therefore a compatible replacement baseline savestate.
The Engine9000 source-confirmed F1--F9 configuration experiment did not make
Windows-generated keys usable by the game frontend. `record_run.py` is restored
to its known-good F8 baseline-restore pairing; function-key coverage now uses
the native replay script below.

Run011 is invalid for gameplay coverage: its Windows message injection produced
release-only events and it remained at the menu. Run012 is also invalid: the
foreground-input attempt failed to activate the packaged restore and showed the
crack message/menu path. Both were stopped without sealing.

The replacement is `captures/run013_scripted/`, made by
`scripts/build_scripted_flight_run.py`. It starts from the compatible in-flight
frame-600 serialized state and supplies native `E9K_INPUT_V1` transitions to
the stock Engine9000 core: F1--F10, repeated F10, and spaced H/M/R/T/A/J/G,
comma, period, Space, and Return. It completes at frame 2,520 in 5.707 seconds
with end-video SHA-256 `1318183316cbc7e8d441db74572d26529c2bec029bd8294737fcea084add7eb6`.
It is scripted coverage evidence; function semantics remain subject to bounded
single-event traces.

The first run013 native F1 event is now isolated with no future input. It hits
`$C0F43A` on frame one and returns to `$C0F45C` after 288 instructions; at
`$C1AD74`, the game receives raw `$50` and falls through the existing
`$C1C23C` queue tail. `$C32CEE` consumes it in the same frame, translates it
to zero through `$C331CE`, and returns without an observed command. Raw P-code
is in `pcode/raw/run013_f1_keyboard_poll/` and
`pcode/raw/run013_f1_command_event/`; the bounded route is documented in
`analysis/routines/c1ad74_native_f1_queue.md`. Plain native F2--F10 events
have not yet reached that game breakpoint, so run013 is not function-key
gameplay coverage.
Treat run008/run009 as failed setup, not gameplay coverage.

The attract display now has a direct Copper/bitplane map in
`analysis/attract_copper_display.md`: CPU loads COP1LC with `$0000A400`, then
the Copper at VPOS 41 writes BPL1--BPL4 pointers `$00012BC0`, `$00014B00`,
`$00016A40`, `$00018980` (stride `$1F40`). It also records the observed display
window/fetch and BPLCON writes. This is frame-specific mutable Chip-RAM data,
so it is intentionally not added to the baseline static-source set.

## 2026-09-21 — parent formatter reconstruction resumed

`format_fixed_width_hex_ascii.asm` now reconstructs `$C0F56A-$C0F5F7` (142
bytes). Static disassembly shows a fixed-width hexadecimal ASCII conversion:
it emits low nibbles in reverse output order and then converts leading `0`
characters to spaces, preserving the final digit. The exact caller ABI and
screen use remain unclaimed pending a bounded call trace. Contract:
`analysis/routines/c0f56a_format_fixed_width_hex_ascii.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **118
source slices / 14,560 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — complete static post-input tick

`run_post_input_tick_prefix.asm` adds the byte-exact `$C0F5F8-$C0F7D1`
prefix (474 bytes) to the pre-existing observed tail at `$C0F7D2-$C0F811`.
The pair now statically covers the post-input tick. The training-frame bounded
packet still proves only the `$C45898` early-guard route; all alternate phase
and callback routes are recorded as static, pending a matching save-state
trace. The routine contract and expanded memory-map evidence are in
`analysis/routines/c0f5f8_post_input_tick.md` and `analysis/memory_map.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **119
source slices / 15,034 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — initial post-tick callback and formatter caller

`initialize_callback_text_state.asm` reconstructs `$C0F812-$C0F91F` (270
bytes), the callback installed by the parent-context initializer. Static
analysis establishes its direct call to `$C0F56A`: a non-sentinel input word is
converted through the fixed-width hexadecimal formatter into a field at
`$C3F055`. This is a caller relationship, not yet an observed callback route;
its display use awaits a bounded trace. Contract:
`analysis/routines/c0f812_callback_text_state.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **120
source slices / 15,304 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — post-input callback transitions

`run_post_input_callback_chain.asm` reconstructs `$C0F920-$C0F991` (114
bytes). It is the static callback succession selected from `$C0F5F8`: one
callback clears three state bytes and advances to `$C0FBE0`; two timer-gated
callbacks compare `$C458A0/$C458A1`, set `$C45795`, and advance to `$C0F992`.
No untraced runtime condition is promoted as gameplay behavior. Contract:
`analysis/routines/c0f920_post_input_callback_chain.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **121
source slices / 15,418 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — post-input follow-up callback

`begin_post_input_followup.asm` reconstructs `$C0F992-$C0FA03` (114 bytes),
the callback installed after the `$C0F974` timer gate. Its exact state updates
and two successor callback addresses are retained structurally; neither route
has been claimed dynamically. Contract:
`analysis/routines/c0f992_post_input_followup.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **122
source slices / 15,532 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — timer-gated post-input callback

`finish_post_input_followup.asm` reconstructs `$C0FA04-$C0FA4B` (72 bytes).
It links the `$C0F992` callback to `$C0FA4C` after a negative countdown and
retains its `$C0FAA4` and `$C2FD22` calls without speculative helper names.
Contract: `analysis/routines/c0fa04_post_input_followup.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **123
source slices / 15,604 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — input-match post-input callback

`wait_for_post_input_match.asm` reconstructs `$C0FA4C-$C0FA7F` (52 bytes).
It completes another static timer/input-equality handoff in the callback chain,
installing `$C0FA80` after its recorded condition. Contract:
`analysis/routines/c0fa4c_post_input_match.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **124
source slices / 15,656 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — post-input completion callback

`complete_post_input_followup.asm` reconstructs `$C0FA80-$C0FAA3` (36 bytes),
ending this contiguous callback subsequence by installing `$C10C08` after its
negative-countdown condition. Contract:
`analysis/routines/c0fa80_post_input_completion.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **125
source slices / 15,692 bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — Ghidra entry coverage baseline

The union of all 109 exported Ghidra `functions.json` inventories contains 197
distinct slow-RAM function entries. Forty have a byte-exact source slice at the
same entry address; 25 routines have bounded behavioural contracts. The
remaining 157 entries are structural/unidentified at entry level. This
denominator and its definitions are recorded in
`analysis/ghidra_function_coverage.md`.

## 2026-09-21 — bounded current post-input callback

The next previously structural Ghidra entry is now identified through an
existing bounded packet: `$C1075A` is the callback reached from the
training-frame `$C0F5F8` tail. With `$C458AC = 0` it returns directly to
`$C0F808`; its nonzero path remains static. Byte-exact source:
`source_amiga/observed/run_current_post_input_callback.asm`; contract:
`analysis/routines/c1075a_current_post_input_callback.md`.

The Ghidra coverage ledger now records **41 / 197** exact-entry source matches
and **26** bounded behavioral routine contracts. Verification: `python
scripts/verify_reconstructions.py` passed with **126 source slices / 15,740
bytes** matching `captures/baseline_menu/slow.bin`.

## 2026-09-21 — bounded display primitive submission

The Ghidra entry `$C212B0` now has a complete 2,552-instruction no-input
packet from frame 602, returning to `$C1F944`. It repeatedly feeds paired
records through `$C2EE4A`, which reaches the observed blitter setup/submission
path. This establishes an evidence-backed display primitive-submission role,
without assigning record or scene semantics. Contract:
`analysis/routines/c212b0_display_primitive_submission.md`.

The Ghidra ledger now has **27** bounded behavioral contracts; exact-entry
source coverage remains 41 / 197.

## 2026-09-21 — projected display-segment contract

The frame-602 display packet calls Ghidra entry `$C2EE4A` ten times. Each
complete invocation returns to `$C2130A` in 232–234 instructions after signed
projection/clipping arithmetic and one `$C2FA7E` blitter-line submission.
This adds an evidence-backed projected-segment preparation contract at
`analysis/routines/c2ee4a_projected_segment_preparation.md`; static source is
still pending. The Ghidra ledger now records **28** bounded behavioral
contracts.

## 2026-09-21 — current-record matrix entry

`build_current_record_matrix.asm` reconstructs the compact Ghidra entry
`$C2DAF2-$C2DB17` (38 bytes). It derives a selected-record angle and invokes
the already reconstructed `$C2E370` matrix builder. Contract:
`analysis/routines/c2daf2_current_record_matrix.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **127
source slices / 15,778 bytes** matching `captures/baseline_menu/slow.bin`.
Ghidra exact-entry source coverage is now **42 / 197**.

## 2026-09-21 — empty update entry point

`return_from_empty_update_entry.asm` reconstructs Ghidra entry `$C25B1C`
(two-byte `RTS`). It is adjacent to the prior `$C25B1E` return slice but is a
distinct discovered entry. Verification: `python scripts/verify_reconstructions.py`
passed with **128 source slices / 15,780 bytes** matching the baseline image.
Exact Ghidra-entry coverage is **43 / 197**.

## 2026-09-21 — bounded record-vector normalization

`normalize_record_vector.asm` reconstructs Ghidra entry `$C25754-$C257D3`
(128 bytes). The existing run003 frame-6000 packet covers its complete
84-instruction invocation: it calls `$C1D974`, normalizes three signed
components, and writes the result at `$C45A4C`. Contract upgraded at
`analysis/routines/c25754_record_vector_normalization.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **129
source slices / 15,908 bytes** matching the baseline image. Ghidra coverage:
**44 / 197** exact entry slices and **29** bounded behavioral contracts.

## 2026-09-21 — returned table-scale helper

The existing run003 `$C25754` packet provides a complete 32-instruction
invocation of `$C1D974`, returning to its caller rather than the older capped
packet's boundary. Its static bytes were already in
`fixed_point_stage_tail.asm`; its now-bounded table-scale contract is
`analysis/routines/c1d974_table_scale_bound.md`. No overlapping source slice
was added. The Ghidra ledger now records **30** bounded behavioral contracts.

## 2026-09-21 — source-range coverage correction

The entry-coverage audit now distinguishes exact `org` matches from entries
inside any assembled byte-exact source range. **49 / 197** Ghidra entries are
represented in source; **44** begin a source slice. The five additional
contained entries are `$C1C214`, `$C1D974`, `$C25A6A`, `$C26428`, and
`$C2B3C2`. Ledger: `analysis/ghidra_function_coverage.md`.

## 2026-09-21 — matrix-record zero guard entry

`clear_zero_matrix_record_guard.asm` reconstructs Ghidra entry
`$C2DE96-$C2DEA1` (12 bytes), a compact matrix-path test/return guard with a
nonzero fall-through to `$C2DEB0`. Contract:
`analysis/routines/c2de96_matrix_record_zero_guard.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **130
source slices / 15,920 bytes** matching the baseline image. Ghidra source
coverage: **45 exact entry slices / 50 represented entries / 197 total**.

## 2026-09-21 — renderer-state initializer entry

`initialize_renderer_state_long.asm` reconstructs Ghidra entry
`$C2F490-$C2F49B` (12 bytes), writing `$000FFFFF` to the renderer state long
at `$C456E6`. Verification: `python scripts/verify_reconstructions.py` passed
with **131 source slices / 15,932 bytes**. Ghidra source coverage: **46 exact
entry slices / 51 represented entries / 197 total**.

## 2026-09-21 — indexed command-state clear wrapper

`clear_indexed_command_state.asm` reconstructs Ghidra entry `$C17B08-$C17B2B`
(36 bytes). It clears a longword in the `$C4FE38` indexed table and forwards
the index to `$C4FFB0`. Contract:
`analysis/routines/c17b08_indexed_command_state_clear.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **132
source slices / 15,968 bytes**. Ghidra source coverage: **47 exact entry
slices / 52 represented entries / 197 total**.

## 2026-09-21 — complete raw keyboard poll

`poll_raw_keyboard_event.asm` reconstructs `$C16C56-$C16CD7` (130 bytes),
the direct poll used by `$C0F43A`. Its exact normalization/filtering explains
the bounded `H` packet's `$25` press, `$A5` release, and `$FF` no-event return
without widening the raw-key mapping claim. Contract:
`analysis/routines/c16c56_raw_keyboard_poll.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **133
source slices / 16,098 bytes**. Ghidra coverage: **48 exact entry slices / 53
represented entries / 197 total**, with **31** bounded behavioral contracts.

## 2026-09-21 — raw keyboard event-source wrapper

`read_keyboard_event_source.asm` reconstructs `$C16BF2-$C16C39` (72 bytes),
the source called by `$C16C56` before its raw-key normalization. It tests the
external source handle, consumes/clears descriptor word `$C1ABAC+6`, and
releases the source through the existing wrapper. Contract:
`analysis/routines/c16bf2_keyboard_event_source.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **134
source slices / 16,170 bytes**. Ghidra source coverage: **49 exact entry
slices / 54 represented entries / 197 total**.

## 2026-09-21 — keyboard event-source vector wrappers

`invoke_external_vector_174.asm` and `invoke_external_vector_1ce.asm`
reconstruct Ghidra entries `$C53C08-$C53C1B` and `$C53C8C-$C53C9F` (20 bytes
each). These are the two external-library wrappers directly beneath `$C16BF2`;
the API identities stay unassigned. Contract:
`analysis/routines/c53c_event_source_wrappers.md`.

Verification: `python scripts/verify_reconstructions.py` passed with **136
source slices / 16,210 bytes**. Ghidra source coverage: **51 exact entry
slices / 56 represented entries / 197 total**.

### 2026-09-21 cockpit bitplane location

- Added `scripts/analyze_cockpit_bitplanes.py`, `analysis/cockpit_bitplane_assets.md`, and generated `analysis/cockpit_bitplane_map.json` plus `analysis/visuals/attract_cockpit_bitplanes_600_1800.png`.
- Copper list `$00A400` loads four contiguous 8,000-byte bitmap planes at `$012BC0`, `$014B00`, `$016A40`, and `$018980`; `$1F40 / 200 = 40` bytes per row gives the observed 320-pixel planar display width. The list enables four planes with `BPLCON0=$4200` at `$00A488` after its display wait.
- The frame-1800 composite visibly contains the cockpit panel, HUD, gauges, and labels. Frame 600 uses the same buffers for the external attract view. All planes differ across the two captures, so these are mutable renderer targets, not an immutable cockpit image blob. Next: use bounded adjacent-frame diffs to isolate persistent panel regions and trace their blitter writers.

## 2026-09-21 — cockpit renderer write path

A no-future-input ten-chipset-frame trace from the sealed attract-frame-1800 state (`build/attract_cockpit_1800_tenframe_trace/`) records CPU blitter destinations inside the Copper-visible cockpit planes. `analysis/routines/cockpit_renderer_writes.md` distinguishes unassigned plane-targeting blits (`$C2FDF0`, `$C306AE`) from the already reconstructed four-plane line emitter (`$C2FBE6/$C2FC4E/$C2FCB6/$C2FD1C`). Full deterministic captures at frames 1800–1802 have identical planes; frame 1810 differs only in small row regions. This identifies a stable cockpit panel with narrow variable HUD/scenery regions and gives the next trace targets without inventing an asset format.

## 2026-09-21 — active-plane packet entry narrowed

Disassembly plus the cockpit trace moves the first active-plane blit packet's entry from the trigger instruction `$C2FDF0` back to `$C2FD8C`. It reads the active plane-pointer table `$C456B6`, derives `BLTSIZE` from `$C45984`, offsets each target by `$28`, and fires four blits at `$C2FDF0/$C2FE3A/$C2FE90/$C2FEDA`. The exact packet disassembly is retained at `analysis/routines/c2fd8c_active_plane_blit_packet.disasm.txt`; its caller and display semantics remain deliberately unassigned pending a return-bounded trace.

## 2026-09-21 — prepared cockpit blitter submit leaf

Added byte-exact `source_amiga/observed/submit_prepared_blitter_job.asm` for `$C30668-$C306B3` (76 bytes), a repeated bounded-trace leaf that waits for blitter idle and writes a caller-prepared Custom-chip job. The trace reaches it from `$C30328/$C30340` and records active cockpit-plane destinations. Contract: `analysis/routines/c30668_prepared_blitter_submit.md`.

## 2026-09-21 — cockpit display memory map

Added the four Copper-visible cockpit plane ranges, `$C456B6` active plane-pointer table, and `$C45984` packet-size input to `analysis/memory_map.md`. Entries are tied to the deterministic attract captures and bounded blitter trace, with the mutable/static distinction retained.

## 2026-09-21 — cockpit plane-preparation caller

The ten-frame trace shows `$C0F05C -> $C0D730 -> $C2FD8C` at chipset frame 4. Added byte-exact `$C0D730-$C0D749` as `prepare_display_buffer.asm`: bit 13 of `$C458D2` chooses the observed four-active-plane packet or alternate helper `$C0DA38`. Contract `analysis/routines/c0d730_display_buffer_gate.md`.

### Label correction

`submit_prepared_blitter_job.asm` now uses the OCS register names `BLTAFWM`, `BLTBPT`, `BLTCMOD`, `BLTBMOD`, and `BLTAMOD` for its already byte-exact writes. No instruction bytes changed.

### Active plane table ordering

Measured `$C456B6` at attract frame 1800: plane table offsets 0/4/8/12 hold `$018980/$016A40/$014B00/$012BC0`, mapping the `$C2FD8C` trigger sequence to Copper planes 4/3/2/1.

### Active plane table correction

The live `$C456B6` values are table pointers, not direct Copper-plane bases. Corrected `cockpit_renderer_writes.md`; destination-to-plane claims remain based only on Custom-register trace addresses.

### Trace-state qualification

The pre-trace frame-1800 RAM snapshot is not the register state at chipset frame 4 where `$C2FD8C` executes. Future table interpretation must read the instruction row registers or `trace_final_slow.bin` for the relevant stepped interval; no table-to-plane mapping is inferred from `slow.bin` alone.

### Live table resolution

Resolved the apparent `$C456B6` discrepancy using trace registers: at the frame-4 `$C2FD8C` packet, `A2=$C4567E` and its dereferenced values produce the logged `$0189A8/$016A68/...` plane destinations. The table is mutable; use per-instruction state, not endpoint RAM, for its active mapping.

### Cockpit pointer publisher trace

Captured `build/attract_cockpit_c2f558_trace/`: restored attract cockpit state, hit `$C2F558` at frame 3, returned `$C15D9C` in nine instructions without future input. Added evidence to `analysis/routines/c2f558_outer_pointer_selector.md`.

### Cockpit plane table resolved

The `$C2F558` live `$C456B6=$C4567E` publication plus frame-1800 table contents resolve `$C2FD8C` offsets 0/4/8/12 to Copper planes 4/3/2/1. Its `$28` adjustment exactly produces traced blitter destinations `$0189A8/$016A68/$014B28/$012BE8`.

### Active cockpit table order

Added `$C4567E-$C4568D` to the memory map with its resolved plane-4-to-plane-1 order and `$C2FD8C`'s `$28` destination adjustment.

### Packet boundary guard

Documented `$C2FD8C-$C2FEDA` as the measured four-plane packet and kept `$C2FEDE` separate pending a return-bounded enclosing-function trace.

## 2026-09-21 — first active cockpit-plane submission

Added byte-exact `submit_first_active_plane_packet.asm` for `$C2FD8C-$C2FDF3` (104 bytes). It resolves the live table selected at `$C456B6`, applies the observed `$28` offset, waits for blitter idle, and starts the first plane-4 submission. It intentionally ends at the first `BLTSIZE` trigger; the following plane submissions remain separate evidence boundaries. Verification passed with **139 slices / 16,416 bytes**.

### Machine-readable cockpit table

Added the resolved `$C4567E` active cockpit plane table to `analysis/semantics.json`, with its bounded trace provenance.

### First plane contract

Added `analysis/routines/c2fd8c_first_active_plane_submission.md`, documenting the exact `$C2FD8C-$C2FDF3` slice and its proven active plane-4 destination without assigning pixel semantics.

## 2026-09-21 — reconstruction-gate overlap check

The verifier now rejects overlapping exact source ranges. It exposed `keyboard_poll_dispatch.asm` as wholly contained in `process_pending_key_events.asm`; removed the redundant 38-byte slice and retained the wider source. Corrected OCS blitter register names in `submit_blitter_line_to_active_planes.asm`: `$052` is `BLTAPTL` and `$062` is `BLTBMOD`.

### Canonical coverage denominator

`analysis/semantics.json` now records 285,976 CODE bytes from the Hunk inventory, 16,378 reconstructed non-overlapping bytes, and 5.727% coverage.
