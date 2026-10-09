# Native line addressing and mission-three drawing window — 2026-10-09

Native line drawing now preserves the original starting address when X is
negative. C2FAB2/C2FAD8 calculate the byte offset with a logical word shift,
signed word addition and sign extension; BLT pointers select an aligned word.
`setup_line` already produced that offset, but `native_raster_line` rebuilt it
from signed screen coordinates. It now uses the supplied offset and advances
relative to the initial word and row.

The playable caller remains native flight -> scene -> line submission ->
`native_raster_line`. This corrects a native raster translation; it does not
introduce an emulator or alter controls, physics, clocks or state ownership.
Typed-state migration remains a separate follow-up outside the completion goal.

## Validation

An original-instruction line fixture failed with 13 changed chip bytes before
the fix. The expanded regression checks 32 negative-X lines: eight endpoint
pairs, including reversed, horizontal, vertical and crossing lines, over four
plane masks. All complete chip buffers match original C2FA7E after the fix.
Both connected setup checkpoints also pass the existing 256 clipping cases,
128 line-return cases, 128 clipped-segment cases and 160 polygon cases, plus
original horizon and complete map drawing. No comparison pixels are excluded.

Release and Debug builds pass the successful independent mission-three replay.
Every one of the 4,967 observations, 79,472 complete aircraft cores and all
camera/control/target fields still match. Their complete traces, final RAM,
counters, earned saves and comparison reports agree apart from executable
fingerprints. These traces and final RAM also equal the preceding build's
mission-three evidence. Strict drawing remains 267/4,967: this particular
flight does not expose the negative-X correction in its recorded pages.

The canonical Release executable is refreshed to SHA-256
`c2e188fcfb95a664381b301ba547d674ed8b1adcf651ed9d0e1d61687234e0cc`.
The structured summary is
`analysis/figures/native_negative_line_and_drawing_window_checkpoint.json`.

## First drawing difference remains open

`check_mission_drawing_window.py` captures a bounded window from independently
started games. Every captured trace field, all sixteen full aircraft cores and
all eight complete page hashes must reproduce their retained flight traces.
The four-observation window starts at original 22,285. Its first pages match;
at 22,286 planes 4/6/7 differ in two bytes each, representing nine pixels on
the left edge at row 90. The next observations preserve and extend that small
line. Message text/countdown and warning state agree in this window.

A reference-only bus probe exports original RAM and actual registers at
C0EFEA/C0F3C0. It forwards original bus operations without changing their timing
or controls. All 22,288 captured update observations match the original trace
byte for byte; its shorter-run end marker is checked separately. Passing trace
copies are removed after verification, and reusable body RAM is compressed.

The existing full-body oracle reproduces native's before/after body with zero
gameplay or display differences, subject to its existing explicit scratch,
voice and busy exclusions. The same original instructions, starting from the
actual source body fixture, reproduce all source display bytes. This latter
diagnostic supplies native PAL samples and a synthetic caller frame: it retains
166 nonvisual differences and is **not** accepted as full-body state parity.

Original line-call observation identifies a source-only colour-seven clipped
segment from (8,89) to (0,90), returning to C2F08E. It clears the nine pixels
that remain set in native. Source and native fixtures also differ in the
alternate placement record at C4F1D2: descriptor C22A48 dispatches C096CA, and
its cached result is source 03EF versus native FFFF. The placement traversal
uses the negative result to skip subsequent calls. This is an investigation
lead; the reason for the earlier different result remains unproven. No cache
value, placement gate or clock has been forced to make the pictures match.

## Reproduction

```powershell
python tools/native/check_raster.py --runner build/native-cmake/native/Release/fa18_native.exe
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-line-address-release
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-line-address-debug
python tools/native/check_mission_drawing_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --native-evidence build/native-flight/original-mission-three-line-address-release --first 22285 --count 4 --out build/native-flight/mission-three-first-drawing-review-current
```

The initial window and body fixtures predate the raster correction; their
reports retain the older executable fingerprint. The current window was also
recaptured with the updated Release executable and reproduces every field,
core and page hash, including the same changed bytes. The new full-flight
replays establish identical trace/page output for this mission. `--body-capture`
retains bounded native body fixtures for original-owner investigation. Optional
`--active-planes`/`--active-map` raster oracle modes compare all eight complete
pages; `FA18_FRAME_TRACE_LINE` logs source line calls without altering them.

All-mission independent whole flights, remaining drawing assessment, recorded
audio/filter fidelity and visible gameplay performance remain open. The
uninterrupted campaign requirement is waived; state cleanup stays deferred.
