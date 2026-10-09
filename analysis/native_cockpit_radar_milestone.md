# Current whole-flight allocation and cockpit checks — 2026-10-09

The current filtered Release and Debug runners reproduce the independent
qualification and successful mission-three replay. Each independently enlists
an actual pilot for 9,000 frames, then completes the 42,706-frame flight/menu
recording. Every previous observation, complete record core, drawing hash,
final RAM byte and earned pilot save is preserved. Both runs report **zero
project gameplay heap violations, zero SDL pool requests and zero SDL failures**.
OS/driver private heaps remain outside this measurement.

Release/Debug agree on the complete 27,110-row original and 16,062-row native
traces, final RAM, every counter, saves and memory reports. The explicit
`--pcm-change` comparison allows only `nonzero_sample_frames` to change from
the earlier unfiltered baseline: enlist 8,299,845 -> 8,511,816, flight
40,187,341 -> 40,521,617. Every game/voice counter remains strict. Implicit
PCM changes, changed postflight resets or sample requests, missing counters
and impossible nonzero counts are rejected. No native runtime code or storage
changed in this batch.

Complete drawing remains **287/4,967 observations**. All rows 0..127 still
match. The full-flight message timing assessment and its nine negative
controls pass on the current recording. No clocks or pixels are adjusted.

## Later radar pixel and corrected owner return

Five bounded observations, 23,334..23,338, reproduce their complete live
trace fields, cores and pages. Every difference is a single pixel in plane
3 of each page: x=155, with y moving from 159 to 160. Both retained point
tables contain the same regular markers; the selected marker alternates
between pages with the observed counter offset of 123 modulo 256.

The original C31226 owner is captured at its actual entry and C0F18E return.
Every preceding observation still matches the original flight. On these ten
unmodified original/native owner inputs and their ten external phase probes,
native C and original instructions match every non-stack RAM byte. All five
complete native bodies also pass their existing explicit comparison scope.
Equivalent blink phases give identical record/coordinate/pair/colour outputs;
four wrong counter/point results are rejected.

For the five **entry** snapshots, every one of the 64,000 actual page XOR
bytes equals the delta predicted from the retained point tables and observed
selected-marker colour. This checks 320,000 bytes without changing or masking
a page. Wrong marker colour and an unrelated page pixel are rejected. It
localizes these bounded differences to retained marker history, while strict
same-phase page comparisons continue to differ.

The **live original owner-return page gate passes: 5/5 match**, alongside 5/5
native returns. The previous failed capture ended at C0F194, after the caller
had also run C332BC (postflight HUD). The extra page writes belonged to that
next child. This was a reference capture error, not evidence of missing live
chipset context. The original C0F188 JSR saves C0F18E on the stack; the collector
now verifies the requested end against that actual saved return address.
The radar assessor independently rejects a retained capture ending later.
The incorrect five fixtures are rejected before replay; correct recaptures
preserve all 23,339 preceding original observations and compare complete pages,
cores, the radar list/cache, counter and threat state without pixel exclusions.
The earlier 24-observation radar window still passes all live original/native
pages, 96 complete owner comparisons and 24 native bodies with the updated
checker.

A separate 11-observation window, 23,944..23,954, retains every later plane-2
difference around x=163/y=162 alongside target speed/message differences.
Correct C0F18E captures preserve all 23,955 preceding original observations.
All 11 original and 11 native live owner-return pages match; 44 non-stack
owner comparisons and 11 complete native body checks pass. The marker colour
can interact with the underlying page, so this result does not generalize the
five-observation colour-1 cache-delta rule to this window or the complete flight.
Complete cross-runtime pixel interaction and message differences remain open.

## Reproduction and retained evidence

```powershell
python tools/native/check_mission_message_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --source-reuse build/native-flight/mission-three-drawing-bands/Release --drawing-bands --pcm-change --out build/native-flight/filtered-cockpit-trace/Release
python tools/native/check_mission_drawing_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --native-evidence build/native-flight/original-mission-three-ground-bounds-release --trace-evidence build/native-flight/filtered-cockpit-trace/Release --first 23334 --count 5 --body-capture --out build/native-flight/cockpit-bar-window
python tools/native/check_original_mission_body.py --source-evidence build/native-flight/original-mission-three-patrol-runway --iteration 23334 --count 5 --owner C31226 --owner-return C0F18E --out build/native-flight/cockpit-radar-correct-return
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/cockpit-bar-window --original-bodies build/native-flight/cockpit-radar-correct-return --trace-evidence build/native-flight/filtered-cockpit-trace/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --complete-page-delta --out build/native-flight/cockpit-radar-correct-assessment
```

The last command passes the complete live page gate. For Debug, select its runner/native baseline and
reuse the verified original trace. The second drawing window uses `--first
23944 --count 11`, with `cockpit-plane-two-radar-bodies` and
`cockpit-plane-two-radar-assessment`; omit `--complete-page-delta` for that
window. Local RAM is compressed; reports and the committed
[checkpoint](figures/native_cockpit_radar_checkpoint.json) bind hashes and results.
Broader complete-flight drawing, original sound onset and visible performance
remain open. The uninterrupted campaign is waived and named-state cleanup
remains deferred outside the goal.
