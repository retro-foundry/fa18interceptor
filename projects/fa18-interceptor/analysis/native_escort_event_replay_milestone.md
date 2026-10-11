# Escort comparison with declared input events — 2026-10-10

The complete **3,816-observation escort comparison now runs**, preserving the
original flight's one resumed observation and its **3,815 executed updates**.
It **fails gameplay/drawing parity from initial state** and the native final
outcome differs. This is progress beyond the rejected global input origin,
not successful escort qualification or an accepted original timing match.

The original remains the independently reproduced failed route described in
[the recording evidence](native_earned_escort_recording_milestone.md).
Both games start with real newly enlisted pilots, play qualification and the
complete preceding mission, and earn their progress normally before escort.
Original RAM never initializes native gameplay. The original JSR/LINK proof
and all three resumed observations remain authoritative; see
[the update-origin review](native_escort_initialization_review_milestone.md).

The actual playable path is `port/native/main.c -> native_frontend_tick ->
begin_update/native_replay_update -> native_flight_tick`. The optional diagnostic
`--input-anchors FILE --input-anchors-out REPORT` removes the assumption that
independently paced menus share a global update index. It loads at most 32
declared events before gameplay, using fixed storage. Event matching reads
the mode, callback and game tick; it does not write gameplay state, clocks or
RNG. Ordinary interactive and recorded playback retain their existing path.

The plan comes solely from first occurrences in the verified original trace:

| Event | Original observation | Actual original update | Actual native update |
| --- | ---: | ---: | ---: |
| Cold main menu, C0FCB4/tick 0 | 1 | 1 | 1 |
| Escort context, C10C08/tick 36 | 40,170 | 40,168 | 39,931 |
| Escort flight, C10D8A/tick 1 | 40,643 | 40,641 | 40,395 |

Each input segment follows original executed-update ordinals from its declared
event. All **2,028 key edges** are delivered in order. The runner counts all
**44,209 actual native updates** and **69,476 native frames** through source
position 44,455. Every flight observation retains its actual native index.
Future-segment keys wait for their declared event, and an event that would
skip a pending key fails explicitly. No state/pixel matching, search or fitted
offset selects an origin; no observations are removed to produce equality.

At first flight entry, complete cores in slots **0, 4, 8 and 14** differ,
along with observer/view state. None of the 3,816 complete core boundaries,
camera/control boundaries or complete page sets match. Individual cores match
in **38,175 of 61,056** comparisons; that partial count does not establish
whole-flight parity. The independent native route reaches two postflight
resets and finishes at player phase four; the original ends at FE without
an escort grade. Earned qualification, level, grades and completion count
still match. Full differences remain retained and unmasked.

The setup review narrows the next investigation. At C0FECE, all 16 complete
cores match. By the first C103E4 event, slots 8 and 14 already differ in the
independently initialized scene. Later C10C08-to-C10C68 progress takes 119
observations in the original and 110 native updates, but this later difference
cannot alone explain the earlier scene difference. The cause still requires
assessment of original scene selection/initialization and its real inputs;
neither an inserted delay nor copied reference state is justified. The player
weapon-selection byte difference in the rejected global replay is absent
with declared event ownership.

Validation distinguishes input ownership from gameplay acceptance:

- The current binary's ordinary replay exactly matches the previous playable
  executable's RAM, pixels, PCM and every reported counter for the checked
  1,960-update qualification prefix. A cold-only anchor is also byte-identical.
- Skipped-key and unreachable-event runs fail explicitly. Twelve malformed
  anchor plans are rejected. All exercised gameplay memory reports show zero
  project heap violations and zero SDL pool failures.
- Twelve altered event-evidence claims are rejected against the unchanged
  retained native trace, including wrong event fields, origins, frames,
  counters, completion and omitted anchors.
- Release CTest passes `fa18_native_replay`, `fa18_native_event_replay` and the
  artifact-cleanup fixture (3/3). The unchanged complete first-mission
  assessment remains identical over all 4,967 observations and its three
  core/control/page sensitivity checks.
- Reassessing the retained escort capture completes all comparisons and
  intentionally exits with failure because the native outcome differs.

The capture executable is retained at
`build/native-flight/independent-escort-event-input/runner.exe`, SHA-256
`545fd59fb8211a27cad831f03fa60bd90915e5c0612a5a22f797c5af1052cba9`.
The final executable adds the new options to help text and has SHA-256
`918f633511a0661f32fa928ca85a1cfed16d9144681c904fdaabf34d8c65c8b4`.
The previous baseline is
`7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.
The [checkpoint](figures/native_escort_event_replay_checkpoint.json) binds
these identities, complete retained reports/captures, event plans, guards and
initial-state reviews. Raw duplicate capture files are removed after verified
lossless compression; the 512 MiB diagnostic and 4 GiB build limits remain.

Initialization assessment, successful escort coverage, broader independent
flight comparisons and recorded original/native sound timing remain open.
Campaign continuity remains waived; general named-state cleanup stays outside
this goal. No flight physics, AI, mission content or RNG behavior changes in
this batch.
