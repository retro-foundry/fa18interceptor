# Active port source ownership

User scope update (2026-10-09): migrating remaining byte-array state into named
C structures is deferred to a separate follow-up outside the active
complete-C-port goal. It improves ownership and clarity; the existing arrays
are ordinary PC memory and do not establish a missing mission. This overrides
older completion notes. Independent whole-flight comparisons, recorded
audio/filter fidelity and visible gameplay performance remain in scope; the
uninterrupted campaign requirement remains waived.

The original and independent native now complete mission three with gear
raised after takeoff, lowered before runway landing, grade and menu return.
The complete original recording reproduces without its controller; native
Release/Debug agree. Whole-flight state parity remains unaccepted after a
repeated original entry at tick 360. Strict comparisons and verified snapshots
retain the failure. Gameplay and Escape are unchanged. See
[success recording](../analysis/native_independent_mission_success_milestone.md).

Independent mission-three failed flight now matches all 5,216 aircraft cores
and camera/control/target state through its first crash/reset callback. The
original recording reproduces all 22,467 boundaries without its input pilot.
Native uses an ordinarily enlisted, earned pilot at the same scene level zero;
Release/Debug agree. Drawing matches 259/326 and 67 failures remain unassessed.
Successful all-mission whole flights and other completion work stay open.
Gameplay and original Escape are unchanged. See [recording evidence](../analysis/native_independent_mission_recording_milestone.md).

Independent complete qualification now matches all 43,648 record cores and
camera/control/target fields across 2,728 first-flight boundaries; 2,722
complete drawings match. Nine result/restart callback runs preserve all
1,611 distinct gameplay states. Six message differences follow the original
expiry/blink cadence at a one-step phase offset, with 83 observed states,
267 original-owner checks and four rejection probes. Release/Debug agree.
Initial pilot contexts differ, and second-flight/menu observation limits
remain explicit. Gameplay and original Escape restart are unchanged;
all-mission independent flights and other completion items remain open.
See [qualification flight evidence](../analysis/native_independent_qualification_trace_milestone.md).

The independently recorded target-info episode now obeys original clock,
countdown, context/redraw and complete-text rules across all 374 transitions.
This assesses the reported ALT/HDG difference under the accepted cadence policy.
V2 fixes a diagnostic mouse field label and observes true target selection;
Release/Debug agree and eight selected CTests pass. Strict whole-flight drawing
and all-mission comparisons remain open. See
[HUD timing evidence](../analysis/native_demo_hud_timing_milestone.md).

Initial startup and qualification-entry sorting are verified. The real C0F812
caller frame and complete C08F26 bootstrap match native lists, retained output
and compared RAM. Release/Debug agree on 22 combined intervals, 60 lists and
40 factor probes; frontend/demo regressions pass. Optional startup observation
does not change gameplay. See [startup evidence](../analysis/native_initial_startup_sort_milestone.md).

Current acceptance (2026-10-09): repeated individual mission tests now replace
the uninterrupted campaign gate by explicit user direction. Cold save/load
boundaries are allowed; retain actual earned results and objective, landing,
save and menu-return checks. Older uninterrupted-tour requirements below are
superseded. Independent original whole-flight parity and other completion work
remain open.

Repeated individual mission acceptance now passes: current Release and Debug
earned cold tours agree on all nine stages, counters and 78-byte saves. Fresh
Release qualification/final source comparisons also pass for both supported
pilot contexts; ten selected checks pass overall. See
[individual mission evidence](../analysis/native_individual_mission_acceptance.md).

Actual Demonstration drawing order now matches its original caller; its native
menu transition sorts all three lists. Mode assertions corrected a fixture that
had selected qualification under the Demonstration label. Qualification entry
also passes. Release/Debug agree on 12 intervals, 30 lists and 20 factor probes;
frontend/demo regressions pass. Initial startup is now verified above; other completion
items stay open. See [mode audit](../analysis/native_menu_sort_mode_audit_milestone.md).
The earlier sorting summary below is historical.

Menu/restart retained sorting now has direct original-caller comparisons:
eleven intervals, 27 sorted lists and eighteen incoming-factor probes agree.
Release and Debug pass; gameplay and playable binaries are unchanged. Reached
paths replace the factor through templates or uncached distances. Initial
startup and rare callers remain open. See
[menu/context evidence](../analysis/native_menu_context_sort_milestone.md).

PCM playback now retains frontend-owned host buffer spans rather than resolving
an addressed game byte for every sample. Three ordinary-key audio/state/pixel
replays and the full earned tour preserve their previous results. Original
sample/voice comparisons and ownership checks pass. Voice/request state still
uses the source arena; full typed-state migration and original recorded-audio
fidelity remain open. See [PCM ownership evidence](../analysis/native_pcm_buffer_ownership_milestone.md).

A newly enlisted pilot now earns qualification and all six menu missions,
including final aircraft objective, carrier wire landing, saved sixth result,
menu return and cold Next Mission wrap. The submarine remains active at
objective admission; a visible explosion is not required. Gameplay is unchanged.
The new per-mission gates compare sampled original instructions and playable
saved bytes. `fa18_native_new_pilot_tour` starts with an empty save directory
and carries only actual game saves across cold starts. Independent original
whole flights and uninterrupted single-process tour checks remain open; see
[native_new_pilot_tour_milestone.md](../analysis/native_new_pilot_tour_milestone.md).

Earlier acceptance notes below describe the preceding milestones.

The active final success gate now uses availability earned by normal-key rescue
and cruise saves from the original ADF pilot. It completes the aircraft
objective, carrier landing, save, menu and cold wrap with matching sampled
original comparisons and playable results. Gameplay is unchanged. See
[`../analysis/native_final_mission_earned_availability_milestone.md`](../analysis/native_final_mission_earned_availability_milestone.md).
The newly enlisted tour is accepted above; independent whole flights remain open.

Intercept Incoming Cruise Missile (F5/internal mode seven) now completes
interception, carrier deck/wire landing, save, messages, Escape/menu and cold
reload through ordinary keys. The serial `fa18_native_mission_7_sequence`
compares 42 intervals and 171 bodies with original RAM/drawing; the playable
replay agrees on saved bytes/menu. Availability comes from the normal-key
rescue save, now reproduced by its existing gate. See
[`../analysis/native_cruise_mission_sequence_milestone.md`](../analysis/native_cruise_mission_sequence_milestone.md).
Gameplay and comparison masks are unchanged. All six menu missions have
successful routes; independent original whole flights remain open.

Search and Rescue (F4/internal mode six) now completes its pod objective,
carrier deck/wire landing, save, messages, Escape/menu and cold reload with
ordinary keys. The serial `fa18_native_mission_6_sequence` compares 44 intervals
and 193 bodies with original RAM/drawing; the playable replay agrees on saved
bytes/menu. See [`../analysis/native_rescue_mission_sequence_milestone.md`](../analysis/native_rescue_mission_sequence_milestone.md).
Existing readable game owners are unchanged. The initial pilot is the original
ADF log; port-earned tour availability and independent whole flights remain
open; internal mode seven is accepted above.

Latest final-mission correction: the native record scheduler preserves C230B0's
returned selection word through C0A364/C0A3A6. Ordinary controls now complete
the final aircraft objective, carrier landing, save, menu return and cold Next
Mission wrap, with sampled original comparisons and matching playable output.
See [`../analysis/native_final_mission_sequence_milestone.md`](../analysis/native_final_mission_sequence_milestone.md).
The original success fixture was explicitly unearned; the active gate now uses
the earned log above. The earlier patrol diagnostic retains its unearned log.
Independent original whole flights and full-port acceptance remain open.

Latest connected fixes: demo attached-camera projection avoids rotating the
model vertex twice; 223 independent takeoff boundaries match both drawing
pages, player motion and camera state. The shared coarse angle clamp now uses
source long arithmetic; extended mission samples match 232 intervals/726
bodies. See [`../analysis/native_outside_camera_milestone.md`](../analysis/native_outside_camera_milestone.md).
Full game acceptance remains unfinished.

Latest direction (2026-10-06): `fa18_native` reuses the game source with a native
intro/menu entry, mission/log selection and connected demonstration/Free
Flight/carrier qualification gameplay, omitting CPU,
translations, glue and chipset objects. Build
with `python ../scripts/build_native.py` from this directory, or
`python scripts/build_native.py` from the repository root. See
[`native/README.md`](native/README.md). The historical restriction below is
superseded for this explicitly requested runner; the deleted implementation
remains retired.

The sustained mode-six/eight guidance continuation and projection/matrix fixes
reuse `game/` owners through `game/native/`. Their shared-runtime comparisons
and remaining whole-game limits are recorded in
[`../analysis/native_guidance_limits_milestone.md`](../analysis/native_guidance_limits_milestone.md).

The active game implementation is `game/`, with temporary CPU adapters in
`game/glue/`. The playable build is defined in `recomp/CMakeLists.txt` and
`../scripts/build_recomp.py`; it produces `fa18_recomp` and `fa18_romfree`.

The abandoned top-level `fa18_port` implementation, its build definition,
contract tests and disconnected native replacement modules have been removed.
Do not recreate that implementation or use historical standalone test counts
as evidence of progress in the playable runner. Previous sources remain in git
history. Some historical analysis and oracle tools reference those removed
sources and cannot be run against the current tree.

All 48 retained shared C source/header files now live in `game/`; no C sources
or headers remain at the top level of `port/`. Build and include paths use the
new locations. These retained dependencies are:

| Files | Active use |
| --- | --- |
| `game/disk.c/.h`, `game/hunk.c/.h` | Original ADF/OFS reading and Hunk parsing, used by `amiga/` loading |
| `game/map_packet_*.c/.h`, `game/map_detail_*.c/.h` | Shared map-packet core called by `game/map_packet.c` and its glue |
| `game/projection_packet.h` | Map-packet depth/result types |
| `game/command_types.h`, `game/context_command_types.h`, `game/flight_command_types.h` | Types included by the active command implementation in `game/` |

Build from the repository root:

```sh
cmake -S port/recomp -B build/recomp-cmake
cmake --build build/recomp-cmake --config Release
python scripts/build_recomp.py
python scripts/build_recomp.py --romfree
```

Removing the abandoned source does not remove the active runner's remaining
Musashi, guest-memory or chipset dependencies. Their removal must happen through
the actual game/runtime call graph.

`game/native_call_graph.json` records C-only ownership for retired CPU entry
adapters. Tooling checks known original callers and actual C call sites before
excluding these entries from deferred/unported lists. This inventory is scoped
to discovered calls; runtime edge counts in profile `_native_edges` demonstrate
exercise in the fixed suite. Neither proves whole-game CPU independence.

Headless frame diagnostics use `--rgb444 OUT.bin --index8 OUT.index8`. The
index stream contains one selected palette index per 320x256 pixel per frame,
including during black fades; it does not change guest state or the RGB stream.
`../scripts/compare_recomp_frames.py` ignores only same-index colours belonging
to the original game's 16-stage Copper fade table at C08510. It checks drawing
indices, all other colours and frame counts. The live gate, timing probe,
comparison image report and emulation meter use that policy. Python comparison
tools and their regression check require NumPy; image reports also use Pillow.

The active emulation meter uses schema 2. Its instruction total includes
`interpreted`, `generated`, `residual` and `adapter`: handwritten CPU steps
fetching original opcodes still count as emulation. Retained C scheduling
without an opcode fetch does not count. Schema-1 percentages omitted adapters
and are superseded. The canonical meter report states whether its scenario
coverage is partial or the full acceptance suite. Raw counts with failed parity
do not establish accepted independence; subsystem omission builds remain the
completion gate.
