# Mission-three selected-target radar cadence — 2026-10-09

The first remaining strict drawing difference after the ground-strip fix is
observation 22,303. It comes from the selected-target cockpit marker, rather
than ground geometry: original C31702 plots a colour-eight pixel pair at
(131,157), while native's matching body skips it. The two affected pixels are
x130/x131, within byte x128..135. Original C3164A tests bit zero of C45883;
C313EA increments that retained counter when the radar prefix runs.

The independently started runs enter this window with counters 126 and 249.
The counter's difference stays 123 modulo 256. Under the existing rendering
cadence policy, its incoming phase need not be identical. Its source-defined
increments, gates, positions, colours and alternating selected-target marker
must still match. No counter, cache, clock or page was changed in gameplay.

## Actual callers and evidence

The playable caller is `native_flight_tick` -> `native_hud_draw` ->
`draw_postflight_renderer_dispatch` -> C31392's common tail ->
`submit_postflight_variant_record` -> pixel plotting. Original C0F188 calls
C31226 and returns at C0F18E. The reference-only bus probe exports complete RAM
and actual registers at those two boundaries, as well as C0EFEA/C0F3C0.
Every bus operation continues unchanged. All 22,326 observed rows reproduce
the independent original recording exactly; the shorter successful footer is
checked independently.

Twenty-four bodies, 22,302..22,325, are captured from both games. Every field,
all sixteen complete aircraft cores and all eight page hashes reproduce their
accepted full-flight traces. Original snapshots are compressed, and passing
oracle RAM remains temporary. The native executable is unchanged at SHA-256
`64cc3278ee928c352230574dba455e56ecfb62c6c787a90e877b45054c5686ee`.

## Checked contract

All 24 native bodies match original instructions from their actual native
before-state and timer interval, with the existing explicit scratch, voice and
busy exclusions. These are connected composition checks, not assertions that
original and native clocks or entire independent RAM images match.

The complete C31226 owner matches original instructions across every
non-stack RAM byte in **96** component cases: both actual incoming states at
each observation, plus external phase-reversal probes. The phase probes are
copies used only by the oracle; they never initialize or alter the playable
game. For each of the **24 actual original returns**, every complete drawing
page, all sixteen complete record cores, the entire C4E2BC..C4E76B list/cache,
the counter and threat-state output also reproduce the live original. Actual
ABI stack and asynchronous OS changes are outside this latter owner-output
comparison; it is not claimed as complete original frame-state parity.

Both runs take eleven one-prefix calls, two two-prefix calls and eleven
suppressed-prefix calls. The positive-activity dispatcher executes the shared
tail twice; this original branch is retained. The final counters are 141 and
8. Every selected-target point is submitted on its actual odd prefix phase;
all record identities, positions, pair/single choices and colours agree at
equivalent blink phases. Ordinary markers agree without changing phase.
Counter increments, premature selected markers, wrong marker coordinates and
lost ordinary markers are each rejected by the assessment's mutation probes.

This accepts the selected-target radar phase in this bounded window under
`native_gameplay_acceptance.md`. Every strict changed byte remains reported;
no drawing mask, image search or new timing exclusion is added. Full-flight
strict drawing remains **287/4,967**, and broader drawing acceptance is open.

The next unassessed difference begins at 22,309 in the target-information line:
original shows HDG 150, while native retains ALT 5115. Original's incoming
INFO_REQUEST is negative and native's remains one. The earlier demonstration
assessment proves that owner's seconds/request rules in its own episode; it
does not automatically accept this mission's episode.

## Reproduction

```powershell
python tools/native/check_mission_drawing_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --native-evidence build/native-flight/original-mission-three-ground-bounds-release --first 22302 --count 24 --body-capture --out build/native-flight/mission-three-second-drawing-review
python tools/native/check_original_mission_body.py --source-evidence build/native-flight/original-mission-three-patrol-runway --iteration 22302 --count 24 --owner C31226 --owner-return C0F18E --out build/native-flight/mission-three-second-body-review
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/mission-three-second-drawing-review --original-bodies build/native-flight/mission-three-second-body-review --out build/native-flight/mission-three-radar-cadence
```

`figures/native_mission_radar_cadence_checkpoint.json` preserves identities and
scoped results. Optional `FA18_FRAME_TRACE_PLOT` and `FA18_FRAME_RADAR_DUMP`
observe original plotting and radar boundaries in the external body oracle.
The runtime, original recordings, original Escape behavior and deferred state
cleanup are unchanged. All-mission independent flight comparisons, other
drawing, recorded audio/filter fidelity and visible performance remain open.
