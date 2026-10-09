# The C port

NO SIG drawing history (2026-10-09): all eight planes across the next
51-observation episode follow actual writes (3,264,000 XOR bytes), closing
48 more plane-0 differences. The full-flight gate still fails with 1,825
plane-0 and 2,785 plane-3 observations open. Missing selected markers now
get explicit inactive-control assessment; counters, ordinary contacts and
all paint controls remain strict. Source-only omitted writes are rejected.
The selected-target regression stays byte-identical; runtime is unchanged.
See [drawing/control evidence](analysis/native_no_sig_cockpit_history_milestone.md).

Complete original flight capture (2026-10-09): 4,967 independent input
observations, 4,965 bodies and 3,602 actual radar/message calls each now
retain complete RAM/registers in 23.71 MB gzip. All 27,110 source trace
rows, counters and final RAM stay exact; 320 old snapshots match and all
7,204 owner returns restore their caller stacks. Current native full traces
are unchanged. The next 51-body message window passes, but its radar
verifier assumes a selected marker despite NO SIG; drawing acceptance
remains failed. See [full original capture evidence](analysis/native_original_full_frame_capture_milestone.md).

Complete mission-three bodies (2026-10-09): all 4,965 unique native bodies
match original instructions with zero gameplay/display differences under
the existing frame-body contract. Compact complete-RAM capture retains
14,895 snapshots in 5.61 MB gzip, preserving the whole 42,706-frame run,
final RAM and earned pilot with zero gameplay heap violations. Release/Debug
reproduce the previous complete 40-body window; affected CTests pass.
Independent drawing history, audio timing and visible performance remain
open. See [frame-body evidence](analysis/native_complete_frame_body_milestone.md).

Combined cockpit history (2026-10-09): all eight complete planes across the
first 40-observation drawing episode follow actual radar, bitmap and message
writes (2,560,000 XOR bytes). The first 38 differing plane-0 observations are
explained; 1,873 remain open. All three omitted-owner histories are rejected;
earlier message and whole-plane-1/2 reports are unchanged. See
[combined drawing evidence](analysis/native_combined_cockpit_history_milestone.md).

Remaining drawing audit (2026-10-09): original radar writes explain the first
17 differing plane-3 observations. Full-flight audits retain 1,911 unresolved
observations for plane 0 and 2,785 for plane 3 and exit failure; whole-plane
acceptance stays strict. Plane-1/2 reports remain identical. See
[remaining drawing evidence](analysis/native_remaining_plane_audit.md).

Complete mission-three plane histories (2026-10-09): all bytes of planes 1
and 2 on both pages are accounted for across all 4,967 independent flight
observations (79,472,000 bytes per plane). Every differing observation has a
validated actual-input radar/message paint history; a missing late history
is rejected. Complete record cores stay exact. Runtime is unchanged; whole
planes 0/3 and broader completion remain open. See [plane evidence](analysis/native_complete_plane_history_milestone.md).

Complete retained audio payloads (2026-10-09): all 8,281 original Demo handler
requests resolve in final RAM to native payloads on the same channels (12
original payloads). Full PCM/event coverage and native owned-span hashes pass;
four corrupt catalogs are rejected and the 48-request startup gate still passes.
Fetch-time contents, handoff ordering and waveform timing remain unproved.
Runtime is unchanged. See [payload evidence](analysis/native_complete_audio_payload_milestone.md).

Bounded message pages (2026-10-09): ten actual C322EE inputs/returns match
original instructions; immutable glyphs and source colour/clear rules predict
all 640,000 owner page bytes. Five native bodies and live original text returns
pass. Current whole-flight message rules still pass all 4,964 transitions and
nine controls. Runtime is unchanged; cross-runtime complete frame histories
and broader drawing remain open. See [message evidence](analysis/native_message_owner_pages_milestone.md).

Bounded radar history (2026-10-09): ordered original crosshair, cache erasure
and marker writes predict all 70 complete owner page sets across 35 observations.
Actual instrument bitmap redraws explain all requested plane XOR bytes
(944,000), including the later background-bit interaction. Negative controls
and the existing five-frame delta check pass. Runtime is unchanged; complete
flight drawing remains 287/4,967, with message differences still open. See
[paint history evidence](analysis/native_radar_paint_history_milestone.md).

Free Flight model crash fix (2026-10-09): valid command $90 at C3BA04
now derives the original six-point block through C20F78; its shared $94
entry is also connected. 184 complete command cases and 32 actual-model
camera poses match original instructions. Release/Debug model, flight-start
and preallocation checks pass; canonical Release is refreshed. Precise
reported flight route is unsealed. See [crash evidence](analysis/native_free_flight_six_point_block_fix.md).

Current whole-flight allocation/cockpit check (2026-10-09): Release/Debug
preserve all qualification/mission-three traces, final RAM, counters and
earned saves, with zero gameplay heap violations. Five bounded complete
page deltas match selected-radar marker history. Corrected original owner
returns match all pages in both bounded windows (5/5 and 11/11); the previous
end address included the next HUD child and is now rejected. Bounded pixel
interaction is now explained above; broader drawing stays open. Runtime is unchanged. See
[current evidence](analysis/native_cockpit_radar_milestone.md).

Original voice ownership (2026-10-09): reference tracing now resolves the
loaded sound hunks, correcting false empty voices in a separate launch with
a 480-byte relocation. Complete original Demo and separate-launch execution,
PCM and register writes remain exact. All 48 initial music requests match
native sample bytes/order/period/volume. Original right-channel onset follows
left by 67 samples; native starts together, so onset/whole-flight sound remain
open. Native executable and gameplay storage are unchanged. See
[voice and handoff evidence](analysis/native_original_voice_layout_milestone.md).

Native PCM filter (2026-10-09): playable output now uses the original A500
fixed and enabled LED cascades, with 56 bytes of fixed state and no gameplay
allocation. Release/Debug match every sample in the complete 13,230,014-frame
original cold-start response. The complete native Demo also matches original
filter processing, preserving every audio event and game-data byte. All
33,800 intro/Free Flight/final-combat frames preserve game state and agree
across builds, with zero heap violations; music transitions pass. Visible
Free Flight presents all 6,500 frames, with 0.518 ms maximum audio work; one
406 ms SDL input stall still fails the 20 ms frame gate. Original game
onset/handoff alignment, broader drawing/performance and deferred state
cleanup remain separate. See [filter integration evidence](analysis/native_pcm_filter_milestone.md).

Original filter evidence (2026-10-09): the existing LED interface reports
filter enabled at all 21,069 Demo boundaries, preserving complete original
PCM/execution and every earlier event row. Two independent cold starts also
produce 13,230,014 stereo frames matching unchanged A500/LED filter functions
exactly, including the reference mixer gain; non-silent output and every
sample are checked. Wrong filters and corrupted LED traces are rejected.
Native executable is unchanged; filter integration, onset/handoff alignment
and whole-flight sound acceptance remain open. See [filter evidence](analysis/native_original_filter_state_milestone.md).

Native PCM averaging (2026-10-09): playable output now uses original source
interval averaging instead of sample holding, with fixed integer storage.
196,608 intervals match unchanged original accumulator functions in both
builds. Intro, Free Flight and final combat preserve complete game state;
Debug/Release agree on the intentionally changed PCM. Complete Demo requests,
stops, voices and stream phases remain exact, with zero heap violations.
Canonical Release is refreshed. Original onset/handoff alignment and Amiga
filter acceptance remain open; broader completion and state cleanup scopes
are unchanged. See [averaging evidence](analysis/native_pcm_averaging_milestone.md).

The playable native audio trace now covers complete Demo requests/stops and
per-frame voices, preserving all PCM/state/counters. Release/Debug rows agree
with zero heap violations; tracing and preallocation checks pass. Source/native
onset/handoffs and filtering remain open. See [trace evidence](analysis/native_audio_trace_milestone.md).

The complete original demo now has a validated PCM recording: all 21,069
reference calls preserve original execution and complete sample coverage.
Their 406,753-row audio-write/voice trace also preserves full state and PCM.
LED/filter pin is known at the retained endpoints only.
Native onset/handoffs and interpolation/filter fidelity remain open; the native
executable is unchanged. See [audio evidence](analysis/native_original_audio_recording_milestone.md).

Gameplay storage is reserved before the playable frame loop. Project heap and
buffered-file open/close calls are guarded; SDL uses a fixed startup arena,
PCM a fixed ring, and pilot/capture I/O direct OS handles. Release/Debug preserve
complete native PCM/state/saves. OS/driver heaps are outside the measured scope;
original RNG/timing remain unchanged. See [evidence](analysis/native_preallocation_milestone.md).

Visible qualification/mission-three playback now finishes with real audio,
exact RAM/counters/earned save, no resets and all 42,706 frames presented.
Recorded controls are isolated by an opt-in diagnostic. Two SDL poll stalls
exceed the 20 ms work budget, so performance acceptance remains open. All rows
remain counted. Other missions/views/combat are separate. See
[visible evidence](analysis/native_visible_replay_input_milestone.md).

Complete mission-three drawing localization (2026-10-09): all 4,967 observations
match rows 0..127 of both complete pages. Optional adjoining bands preserve
every earlier trace, page, final RAM byte, counter and save in Release/Debug.
The first row-191 difference is sliding target-info text at the same offset.
Strict pages remain 287/4,967; later cockpit drawing and broader completion
remain open. See [localization evidence](analysis/native_mission_drawing_bands_milestone.md).

Complete mission-three message timing (2026-10-09): all 4,964 real transitions
obey original elapsed/countdown, delay/redraw, full text and colour-cache rules,
including 1,362 paused HUD periods. Nine priority-message producer fields match
at all 4,967 observations. Both first HDG events follow two sampled-second
changes (original tick168/native185); this is assessed under the accepted
cadence policy. Release/Debug agree; nine wrong results are rejected. Optional
read-only fields preserve all earlier traces, pages, final RAM, counters and
saves. Strict pages remain 287/4,967; other drawing and completion work remain
open. State cleanup stays deferred. See [message evidence](analysis/native_mission_message_timing_milestone.md).

Ground-strip startup fix (2026-10-09): native now calls original C2527C,
generating all 40 disk-backed strip corner records before gameplay. Missing
corners caused empty ground draws and later placement skips, consistent with
the Free Flight disappearing-road report. Initializer RAM and all 240 corners
match original instructions; the omitted-call regression is rejected.
Release/Debug mission-three gameplay remains exact; first missing-ground-line
pages now match and strict drawing improves 267 -> 287/4,967. The next strict
difference is observation 22,303. Free Flight callback/restart, connected model,
frontend and new CTests pass. Other drawing remains open; state cleanup stays
outside this goal. See [ground evidence](analysis/native_ground_bounds_startup_milestone.md).

Native raster addressing (2026-10-09): negative-X lines now use the original
logical-shift starting offset and aligned word address. The connected raster
checks pass all existing cases and 32 new negative-X cases per checkpoint.
Release/Debug complete mission-three gameplay, traces and RAM remain identical
to the preceding evidence; strict drawing stays 267/4,967. A bounded capture
localizes the first visible difference to a skipped ground segment and a
different placement cache; its earlier cause remains open. Canonical Release
is refreshed. State cleanup remains deferred outside this goal. See
[drawing evidence](analysis/native_negative_line_and_drawing_window_milestone.md).

User scope update (2026-10-09): migrating remaining byte-array state into named
C structures is deferred to a separate follow-up outside the active
complete-C-port goal. It improves ownership and clarity; the existing arrays
are ordinary PC memory and do not establish a missing mission. This overrides
older completion notes. Independent whole-flight comparisons, recorded
audio/filter fidelity and visible gameplay performance remain in scope; the
uninterrupted campaign requirement remains waived.

The successful independent mission-three flight now passes complete gameplay
state in Release/Debug: all 4,967 observations and 79,472 record cores match.
An exact original probe proves two extra recorder observations were interrupt
resumptions; every observation remains compared at its real update call. Both
native builds agree on traces, RAM, counters and earned saves/menu return.
Strict drawing matches 267/4,967 and remains open. This supersedes the earlier
state-parity failure below; runtime and Escape are unchanged. See
[execution evidence](analysis/native_independent_mission_execution_milestone.md).

The original and independent native now complete mission three with gear
raised after takeoff, lowered before runway landing, grade and menu return.
The complete original recording reproduces without its controller; native
Release/Debug agree. Whole-flight state parity remains unaccepted after a
repeated original entry at tick 360. Strict comparisons and verified snapshots
retain the failure. Gameplay and Escape are unchanged. See
[success recording](analysis/native_independent_mission_success_milestone.md).

Independent mission-three failed flight now matches all 5,216 aircraft cores
and camera/control/target state through its first crash/reset callback. The
original recording reproduces all 22,467 boundaries without its input pilot.
Native uses an ordinarily enlisted, earned pilot at the same scene level zero;
Release/Debug agree. Drawing matches 259/326 and 67 failures remain unassessed.
Successful all-mission whole flights and other completion work stay open.
Gameplay and original Escape are unchanged. See [recording evidence](analysis/native_independent_mission_recording_milestone.md).

Independent complete qualification now matches all 43,648 record cores and
camera/control/target fields across 2,728 first-flight boundaries; 2,722
complete drawings match. Nine result/restart callback runs preserve all
1,611 distinct gameplay states. Six message differences follow the original
expiry/blink cadence at a one-step phase offset, with 83 observed states,
267 original-owner checks and four rejection probes. Release/Debug agree.
Initial pilot contexts differ, and second-flight/menu observation limits
remain explicit. Gameplay and original Escape restart are unchanged;
all-mission independent flights and other completion items remain open.
See [qualification flight evidence](analysis/native_independent_qualification_trace_milestone.md).

The reported ALT/HDG timing difference is assessed under the accepted cadence
policy. All 374 transitions of the independently recorded target-info episode
obey original elapsed/countdown, context/redraw and complete-text rules.
Release/Debug agree; eight selected CTests pass. V2 corrects a diagnostic mouse
field label and adds true target selection. Strict full-flight drawing and
all-mission whole-flight comparisons remain open. See
[HUD timing evidence](analysis/native_demo_hud_timing_milestone.md).

Initial startup and qualification-entry sorting now match the original callers.
Release and Debug agree on 22 startup/menu/restart intervals, 60 sorted lists,
40 incoming-factor probes and every compared RAM hash. No initial sorting fault
was found; optional startup diagnostics leave game behavior unchanged. Frontend,
demo and artifact regressions pass in both builds. The requested startup check
is complete; see [startup evidence](analysis/native_initial_startup_sort_milestone.md).

Current acceptance (2026-10-09): the user waived the uninterrupted campaign
requirement. Repeated individual qualification/mission tests, with actual earned
saves and checked objectives, landing, results and menu returns, are sufficient
for campaign qualification. Cold starts are allowed. Older references to an
uninterrupted tour as required work are superseded. Independent original whole
flights and the other completion items remain open.

Current Release and Debug both pass the earned cold tour, with all nine stages,
runtime counters and 78-byte saves identical. Fresh Release qualification and
final source comparisons pass in both supported pilot contexts. Ten selected
checks pass; this qualifies repeated individual missions under the revised
criterion. See [individual mission evidence](analysis/native_individual_mission_acceptance.md).

Actual Demonstration drawing order now matches its original caller; its native
menu transition sorts all three lists. Mode assertions corrected a fixture that
had selected qualification under the Demonstration label. Qualification entry
also passes. Release/Debug agree on 12 intervals, 30 lists and 20 factor probes;
frontend/demo regressions pass. Initial startup is now verified above; other completion
items stay open. See [mode audit](analysis/native_menu_sort_mode_audit_milestone.md).
The earlier sorting summary below is historical.

Menu/restart retained sorting now has direct original-caller comparisons:
eleven intervals, 27 sorted lists and eighteen incoming-factor probes agree.
Release and Debug pass; gameplay and playable binaries are unchanged. Reached
paths replace the factor through templates or uncached distances. Initial
startup and rare callers remain open. See
[menu/context evidence](analysis/native_menu_context_sort_milestone.md).

PCM playback now retains frontend-owned host buffer spans rather than resolving
an addressed game byte for every sample. Three ordinary-key audio/state/pixel
replays and the full earned tour preserve their previous results. Original
sample/voice comparisons and ownership checks pass. Voice/request state still
uses the source arena; full typed-state migration and original recorded-audio
fidelity remain open. See [PCM ownership evidence](analysis/native_pcm_buffer_ownership_milestone.md).

A newly enlisted pilot now earns qualification and all six menu missions,
including final aircraft objective, carrier wire landing, saved sixth result,
menu return and cold Next Mission wrap. The submarine remains active at
objective admission; a visible explosion is not required. Gameplay is unchanged.
The new per-mission gates compare sampled original instructions and playable
saved bytes. `fa18_native_new_pilot_tour` starts with an empty save directory
and carries only actual game saves across cold starts. Independent original
whole flights and uninterrupted single-process tour checks remain open; see
[native_new_pilot_tour_milestone.md](analysis/native_new_pilot_tour_milestone.md).

Earlier acceptance notes below describe the preceding milestones.

Final mission availability is now earned by the ordinary rescue/cruise save
chain from the original ADF pilot. The final gate uses the actual saved log and
completes aircraft objective, carrier landing, save, menu and cold wrap with
matching sampled original comparisons and playable results. Gameplay is
unchanged. See
[`analysis/native_final_mission_earned_availability_milestone.md`](analysis/native_final_mission_earned_availability_milestone.md).
Independent original whole flights remain open; the earlier patrol diagnostic retains its explicitly unearned log.

Intercept Incoming Cruise Missile (F5/internal mode seven) now completes its
interception, carrier deck/wire landing, saved result, messages, Escape/menu and
cold reload through ordinary keys. All 42 input/stage intervals and 171 bodies
match sampled original RAM/drawing, with matching playable saved bytes/menu.
Its availability is earned by the normal-key rescue save from the original ADF
pilot; the rescue gate reproduces that retained input/log. See
[`analysis/native_cruise_mission_sequence_milestone.md`](analysis/native_cruise_mission_sequence_milestone.md).
All six menu missions have successful native routes. Independent original
whole flights and full-port acceptance remain open.

Search and Rescue (F4/internal mode six) now completes the pod objective,
carrier deck/wire landing, saved result, messages, Escape/menu and cold reload
through ordinary keys. All 44 input/stage intervals and 193 bodies match the
sampled original RAM/drawing; the playable replay agrees on saved bytes/menu.
See [`analysis/native_rescue_mission_sequence_milestone.md`](analysis/native_rescue_mission_sequence_milestone.md).
The initial pilot comes from the original ADF. Earned tour availability,
independent complete-flight parity remain open. Gameplay and comparison masks
are unchanged; internal mode seven is accepted above.

Latest direction (2026-10-06): `fa18_native` now runs the original intro,
credits, pilot entry, menu selection, mission list and pilot-log controls using
existing `port/game/` functions. Its link
omits CPU, translations, glue and chipset objects. Demonstration, Free Flight
and carrier qualification now execute connected native flight, terrain/models
and disk-backed cockpit artwork. All three recorded scenarios have functional
outcomes; complete recorded-frame parity remains unverified. This native
runner supersedes the incremental removal work.
Build/run instructions and remaining timing/audio/state scope are in
[`port/native/README.md`](port/native/README.md).

The final Carrier Sub mission (F6/internal mode eight) now completes its
aircraft objective, carrier landing, saved result, menu return and cold Next
Mission wrap through ordinary keys. Its scheduler now preserves the original
selection-word return at C230B0/C0A364/C0A3A6. The sampled original comparisons
and playable result agree; see
[`analysis/native_final_mission_sequence_milestone.md`](analysis/native_final_mission_sequence_milestone.md).
The original success evidence used an unearned eligibility fixture; the active
gate now uses the earned log above. Independent original complete-flight parity
and full-port acceptance remain open.

Sustained mode-six/eight native flight now connects the readable guidance
continuation and corrects projection returns and matrix angle semantics.
114 input/stage intervals and 465 sampled bodies match compared original
RAM/display. Mode-six/eight success is now accepted above; independent full
flights remain open. See
[`analysis/native_guidance_limits_milestone.md`](analysis/native_guidance_limits_milestone.md).

The demo attached-camera projection now uses the original model vertex,
correcting apparent aircraft clipping after takeoff. All 223 independently
aligned takeoff boundaries match drawing, player motion and camera state.
The adjacent coarse angle clamp also matches source long arithmetic; extended
modes five through eight match 232 input/stage intervals and 726 bodies.
See [`analysis/native_outside_camera_milestone.md`](analysis/native_outside_camera_milestone.md).
Complete game acceptance remains unfinished.

## Deliverable

Recreated, readable C source for the whole game: named functions and
parameters, named globals and structs, fixed-point types and comments on
intent, with no CPU emulator and no generated 68000 code in the final build.

Everything else is scaffolding with two jobs: keep the game running and
rendering at every step, and serve as the reference each hand-written routine
is proven against.

For the 2026-10-05 milestone the user accepts static recompilation of the
remaining entries, marked for later readable decompilation. All 85 deferred
translations now have direct native instruction-helper bindings in
`port/recomp/generated/recomp_static_deferred.c`; their per-entry debt is in
`recomp_deferred.json`. Shared CPU/machine state remains an interim dependency.
This does not change the final readable, CPU-free deliverable above.

The active goal is full independence from Kickstart and emulation in the
current playable port. Game implementation belongs in `port/game/`; temporary
CPU adapters belong in `port/game/glue/`. Changes must be connected to the
active `fa18_recomp`/`fa18_romfree` runners before they count as runtime progress.

The abandoned top-level `fa18_port` build, its source files and disconnected
native replacement components were removed at the user's request. The retained
shared disk/Hunk loading, map-packet code and command headers now live in
`port/game/` with the other active sources. There are no top-level C source
or header files left in `port/`. See `port/README.md`. Historical standalone
proof notes/tools may reference removed files; those are not the active work
plan. Git history preserves the former implementation.

## Architecture

```
            recorded input / live SDL input
                         |
   +---------------------v----------------------+
   | port/machine: A500 model                    |
   |   bus, Chip/Slow RAM, ROM, custom chips,    |
   |   blitter, Copper, display, CIAs, timeline  |
   +---------------------^----------------------+
                         | memory and register access
   +---------------------+----------------------+
   | CPU state (Musashi register file)          |
   |   Musashi interpreter: Kickstart ROM,      |
   |   undiscovered code                        |
   |   port/recomp/generated: translated game   |
   |   port/game: recreated C (via glue)        |
   +--------------------------------------------+
```

- **Machine layer** (`port/machine/`). Memory map with mirroring; custom
  registers; a synchronous blitter ported from UAE (area, fill, descending,
  line; BBUSY and the BLIT interrupt follow the real duration); Copper per
  scanline; bitplane fetch to a 320x256 RGB444 frame (image origin: DIW
  `$81`, beam line `$2A`); CIA timers, TOD and keyboard; interrupts. It loads
  UAE savestates, so a run can start at any recorded frame.
- **One timeline.** Chipset work (line ends, Copper, display, interrupt
  acceptance) happens only at instruction boundaries in
  `fa18_machine_service()`, which both the interpreter hook and translated
  code reach, so both take interrupts at the same instruction.
- **Translation** (`tools/recomp/recomp.py` into `port/recomp/generated/`).
  One C function per routine, labels at every leader, native control flow;
  each data operation runs Musashi's handler for that opcode. The translator
  and interpreter share one register file, so execution can move between them
  at any label. Writes to translated bytes invalidate the routine.
- **Recreated source** (`port/game/`). Hand-written C entered through a glue
  function registered in `port/game/glue/ports.c`.
  Event-bearing glue resumes at original instruction boundaries; instruction
  fixtures include DMA contention and live batches compare fresh source OFF
  frames. Exact isolated batches can still expose combined timing debt in
  other fixed-charge entries. Current timing evidence is in
  `analysis/routines/native_c_template_placements_domain.md`.
- **OS replacement** (`port/os/`). Source-backed C Kickstart services, with a
  temporary CPU bridge while the game still uses the original register file.

## Stages

| Stage | What | State |
| --- | --- | --- |
| A | Whole-program translation, interpreter fallback | done (624 routines) |
| B | Machine layer | done; bus timing modelled to ~0.1-0.5% (STATUS.md, "Bus timing") |
| C | Machine and frame parity with Engine9000 | historical emulator comparisons documented; current acceptance uses the sealed native recordings |
| D | Readable C, proven in related batches | 539 readable translated + 85 explicitly deferred static entries cover the seeded 624; 75 additional readable source-only entries, 543 readable entries with source timing. Original callback scope remains follow-up work; see CURRENT_PORT_HANDOFF.md |
| F | Native backend: plain C memory, direct drawing and audio | pending in the active runner; removed standalone component proofs do not establish runtime completion |
| E | OS replacement (Kickstart calls), cold boot from the ADF | Last: assess which services remain necessary after D and F; existing C shims are verified on three native sessions |

The project work order is D, then F, then only the necessary parts of E.
The 2026-10-04 user instruction authorizes ROM independence now: a separate
`fa18_romfree` runner must start from the ADF without Kickstart or a savestate,
retaining the CPU and chipset model and preserving original behavior. The
Amiga SDK is reference-only. Loading and service facilities must be reusable
across games, with Interceptor-specific configuration kept separate. See the
active objective in CURRENT_PORT_HANDOFF.md; the older service deferral is
superseded.

Later 2026-10-04 steering prioritizes a playable, faster ROM-free build using
behavior-level host compatibility services. Exact OS timing/register side
effects are deferred; existing exact fixtures remain a later validation oracle.
See `analysis/routines/romfree_exact_followup.md` for the return work.

## Recreating game-source batches (stage D)

1. Pick a related batch: `python tools/recomp/port_candidates.py` lists
   routines whose callees are already C, ranked by glue burden. Also inspect
   indirect and table-dispatched families that this list cannot rank.
2. Read it: `python tools/recomp/port_info.py C2FA7E` prints its
   instructions, observed call sites, and the registers and flags live after
   it returns. Read its report in `analysis/routines/`, the memory map, and any
   earlier implementation in git history for the same address.
3. Write the C in the right `port/game/` file, as original source would be
   written (see Conventions).
4. Write the glue in `port/game/glue/`: read the inputs from registers and
   memory, call the C, rebuild every live register, flag and high word the
   original leaves, then `glue_return()`. Register it in `ports.c` with the
   cycles to charge.
5. Build and use short recording probes for the new routines while porting a
   substantial batch. Commit verified chunks as they are ready.
6. Run `sh scripts/recomp_ports_check.sh` over all native recordings after the
   larger batch, then update the proof counts and handoff notes.

When every caller of a routine is C, its glue is no longer reached; delete it.

## The proof

- **Liveness** (`tools/recomp/liveness.py`, emitted as
  `generated/recomp_liveness.c`). For every return address: which registers
  (data registers as low and high words) and flags the caller can read
  before overwriting them. Interprocedural over static and observed call
  edges (`fa18_recomp --edges`). A return into code outside the translation,
  such as a ROM interrupt dispatcher, keeps everything live.
- **Shadow** (`--ports shadow`). On every call of a recreated routine, the
  glue runs first, sandboxed (no chipset events, hardware blocked, custom
  writes held, all undone), then the generated routine live, servicing
  chipset work and stepping to the next label exactly as the dispatcher
  would. Compared: live registers and flags, every memory byte either run
  wrote (except the dead stack below the returned-to SP), and the exact
  sequence of custom-register writes. The game continues on the live run,
  so a shadow run ends byte-identical to a plain run. Calls with an
  interrupt or hardware access inside, or cut by a frame end, are not
  compared.
  A stepped bridge can opt into source-first DMACONR input replay when held
  BLTSIZE writes make the usual port-first input state differ. The source's
  ordered PC/value read stream supplies hardware inputs only; C still computes
  its own registers and writes on entry RAM. An extra/reordered read fails
  immediately, and missing reads fail comparison. The live source result is
  retained. Reports expose `busy_input_calls` and `busy_input_reads`.
  Full ON RGB/RAM parity and instruction/event traces independently establish
  live timing; this shadow input replay is not a timing proof.
- **Sandbox** (`--ports sandbox`). The older comparison: the generated
  routine first with events held off and its custom writes performed at its
  end, then the glue. It covers the calls shadow cannot (audio, joystick)
  but shifts events and blits, so it proves routines without keeping
  timing. `scripts/recomp_ports_check.sh` runs both. Chip bytes the
  blitter wrote during a live call are not compared: the sandboxed port's
  blits are held, so CPU writes over them differ for DMA reasons only.
- **Poison** (`--poison`). After every compared call, everything liveness
  declares dead is overwritten; all frames must still render identically.
- **ON mode** (`--ports on`). The recreated C runs the game; parity must stay
  the same.

## Conventions for `port/game/`

- Globals are named in `globals.h`, at their original addresses, each with the
  routine or capture that established it. Unknown meanings stay honest
  (`REC_FIELD_0C`) until evidence names them.
- Hardware registers, bits and minterms are named in `hardware.h`; write them
  with `custom_write()` and `custom_write_ptr()`, and wait with
  `wait_blitter()`.
- Game memory is reached with `rd_*`/`wr_*` on `gaddr` addresses
  (`memory.h`) while generated code shares it. They become plain C globals and
  pointers in stage F.
- Keep word-exact arithmetic where the original has it: `int16_t` casts for
  `.W` operations, arithmetic `>>` for `ASR`.
- Factor shared logic where it reads better (`setup_line` serves lines and
  polygon edges); behaviour must stay identical.
- No 68000 register or flag details in `port/game/`; those belong in the glue.

## Validation gates

| Check | Command | Must hold |
| --- | --- | --- |
| Recreated routines | `sh scripts/recomp_ports_check.sh` | 0 mismatches; poison frames identical |
| Live frame parity | `sh scripts/recomp_live_check.sh` (`PORTS_ONLY=LIST` for an isolated batch) | `--ports on` RGB444 frames identical to fresh `--ports off` source streams on every affected sealed recording; final RAM and blit totals alone are insufficient |
| Translation vs interpreter | `fa18_recomp ... --ram-out A` vs `--no-recomp --ram-out B` | identical |
| Machine vs emulator, first steps | `scripts/recomp_lockstep.py` | first divergence understood |
| Blitter | `build/recomp/blit_replay.exe CHIP WRITES OUT` | identical Chip RAM |

## Known limits

- **Timing.** Bus contention, CIA E-clock waits and the 68000's access
  order are modelled (`port/machine/bus.c`) and match cycle-exact UAE to
  ~0.1-0.5% per scene. Long recorded replays still drift (run060 from frame
  94); exact replays need UAE's cycle-exact CPU and blitter timing. Routine
  proofs do not depend on this: the shadow check compares every call.
- **Kickstart** runs on the interpreter; together with the few game
  instructions not yet translated it takes about 30% of CPU cycles. Stage E
  replaces the ROM calls the game uses.
- **Start state** is a savestate; cold boot from the ADF needs disk loading.
- **Audio** (Paula) is not modelled; sprites are not drawn.
- ON mode mixes source-timed bridges with remaining fixed-charge entries.
  These CPU bridges are transitional proof machinery; readable domain C and
  the final native backend remain the deliverable.
