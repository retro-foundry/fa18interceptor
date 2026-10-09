# Complete mission-three drawing localization — 2026-10-09

Both independent games now record five adjoining full-width drawing bands.
Every byte of all eight complete page planes remains compared. The bands locate
differences; they do not replace the strict page gate or exclude pixels.

All **4,967 observations** match rows **0..127** of both pages. Rows 128..159
match 4,501 observations; rows 160..191 match 1,222; rows 192..199 match 1,551.
The first differences are at observations 22,303, 22,442 and 22,309 respectively.
Strict complete pages remain **287/4,967**. Broader drawing acceptance is open.

The first lower-band difference is also target-information text. Four bounded
snapshots, 22,441..22,444, reproduce every live trace field, complete aircraft
core, full page and new band hash. All changed bytes are retained. At 22,442,
the difference reaches row 191: both games have REDRAW_STATE_WORD = -1 and
REDRAW_STATE_LONG = -40. Original C322EE's message owner draws at 0x1E0C;
`small_text_line` adds that retained slide offset. Original shows ALT 5110 while
native shows HDG 150. This localizes the first difference to the already
assessed message sequence; it does not establish the cause of every later
lower-band difference.

## Connected runtime and preserved evidence

The playable entry remains `port/native/main.c` -> `native_frontend_tick` ->
the C0EFD4 pre-input observer. The original reference uses that same boundary
through `port/os/loop_input.c`. Shared `port/native/flight_trace.c` reads host
spans directly. `FA18_TRACE_DRAWING_BANDS=1` adds hashes only; ordinary V2 output
is unchanged. This removes the need to retain a MiB of RAM for every update
just to locate a drawing difference. No gameplay dependency is removed.

Release and Debug preserve all 27,110 original and 16,062 native observations,
complete final RAM, counters, landing/result and earned save. The instruction-
proven JSR/LINK mapping pairs every observation; no fitted frame alignment is
introduced. Both builds give identical band and complete message assessments.
The whole-flight message assessment still checks all 4,964 real transitions,
including nine rejected wrong results.

The current runner also preserves radar-counter continuity for the complete
flight. Across all 4,967 observations the incoming phase difference stays 123
modulo 256 and gauge-refresh state agrees. All 4,964 real transitions have the
same increment: 3,040 zero-prefix transitions, 1,669 one-prefix and 255 two-
prefix transitions. Two instruction-proven duplicate observations do not
advance either counter. Wrong counter and refresh values are rejected.
Release/Debug agree. This extends the earlier bounded phase proof to complete
observed counter continuity; it does not prove every marker pixel or gate.

Default and enabled trace contracts both pass live RAM/core/page verification
and their existing truncation/budget checks. Enabled hashes reproduce actual
RAM slices. The parser rejects incomplete or overlapping geometry, missing
row hashes and malformed planes. The band assessment rejects a corrupted
matching-plane hash. Existing frame-timing integration also preserves RAM,
pixels and counters across 6,500 headless frames and reports all eight dummy-
window presentations; these are integration checks, not visible performance.

## Reproduction

```powershell
python tools/native/check_mission_message_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --drawing-bands --out build/native-flight/mission-three-drawing-bands/Release
python tools/native/assess_mission_drawing_bands.py --traces build/native-flight/mission-three-drawing-bands/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/mission-three-drawing-band-assessment/Release
python tools/native/check_mission_drawing_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --native-evidence build/native-flight/original-mission-three-ground-bounds-release --trace-evidence build/native-flight/mission-three-drawing-bands/Release --first 22441 --count 4 --body-capture --out build/native-flight/mission-three-lower-cockpit-window
```

For Debug, select its runner and native baseline, and reuse the already
verified original via `--source-reuse build/native-flight/mission-three-drawing-bands/Release`.
Compressed traces and bounded snapshots remain local; the committed checkpoint
retains hashes and results. Other missions, later cockpit drawing, recorded
audio/filter fidelity and visible performance remain open. State cleanup is
deferred outside the goal, and the uninterrupted campaign remains waived.
