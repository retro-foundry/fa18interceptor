# Complete mission-three frame bodies - 2026-10-09

All **4,965 unique native mission-three frame bodies** match original execution
with **zero gameplay differences and zero display bytes** under the existing
frame-body comparison contract. This covers original flight observations
**22,142..27,108**; two real original dispatch duplicates correspond to the
same native updates. Every native input boundary also reproduces the accepted
trace fields, sixteen complete record cores and eight complete page hashes.

The native game independently enlists a new pilot, completes qualification and
the successful mission-three route, and returns to the menu. The complete
**42,706-frame** run preserves counters, final RAM and the actual earned save.
The source-versus-native drawing-history audit remains separate: plane 0 still
has 1,873 unexplained observations and plane 3 has 2,785. Executing original
instructions from each actual native body input does not reproduce the
independent original game's accumulated cache history.

## Connected diagnostic and ownership

The active runner's `main -> native_frontend_tick -> native_flight_tick`
observer now supports `--frame-delta FIRST[+COUNT] PATH`. It observes actual
input, body-begin and body-end boundaries through existing frontend hooks.
Game composition and the hooks themselves are unchanged. The capture records
complete host memory; no captured bytes enter native gameplay.

The previous separate-file diagnostic would require 15,618,539,520 raw bytes
for this flight. The new stream stores changed, ordered 64-byte blocks and a
SHA-256 of each reconstructed complete MiB. Unchanged bytes remain present in
the reconstructed state. It captured **14,895 complete snapshots** in
**31,837,416 bytes**, below the normal 512 MiB budget; the retained gzip is
**5,605,004 bytes**. Temporary decoded input/output files are reused, then
removed. A terminal snapshot count rejects incomplete captures.

Capture ownership is in `port/native/`, attached only when requested. Its
one-MiB previous-state buffer and fixed 4 KiB file buffer are allocated/opened
before the gameplay heap lock. The real full flight reports zero project heap
violations, zero SDL gameplay allocation requests and zero SDL arena failures.
OS/driver private allocations remain unobserved. This removes the disk-budget
obstacle to complete frame-body checking; it introduces no game rules or
timing adjustments.

## Comparison contract and validation

The external reference oracle executes original instructions from C0EFEA to
C0F3C0 using each actual native before/after PAL interval and saved tick.
Its existing exclusions remain unchanged: native-local ABI scratch, dynamic
asynchronous voice records/slots, three synchronous-raster busy flags and
the final fake ABI stack area. RGB4 publication is a host boundary. Complete
live page buffers are compared; this adds no pixel masks or new exclusions.
Audio timing must be qualified separately.

Release checks every body of the complete flight. Release and Debug also
reproduce the previous 40-body window (22,302..22,341), including every full
input/before/after RAM hash and timing sidecar. Both streams are identical,
and all eighty reference body executions pass. Both builds pass the affected
preallocation, preallocation-output and flight-start CTests.

Decoder tests reject truncation, incorrect RAM hashes, invalid block offsets,
wrong terminal counts, trailing bytes and missing input boundaries. Live CLI
checks reject invalid ranges, missing recorded input, mixed capture budgets
and ranges beyond the input. A real flight with a deliberately limited 1 MiB
capture budget fails explicitly before exceeding it, without heap violations.

The [checkpoint](figures/native_complete_frame_body_checkpoint.json) binds
runner, reports, compressed captures, every body identity digest, unchanged
reference oracle and tooling. Canonical `build/native/fa18_native.exe` is
refreshed to the validated Release build. Previous proof hashes stay historical.

## Reproduction

```powershell
python tools/native/check_mission_frame_delta.py --runner build/native-cmake/native/Release/fa18_native.exe --reference build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/frame-delta-complete-flight/Release
python tools/native/check_mission_frame_delta.py --runner build/native-cmake/native/Debug/fa18_native.exe --reference build/native-flight/six-point-model-flight/Debug --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/frame-delta-first-window/Debug --first 22302 --count 40 --window build/native-flight/cockpit-first-combined-window
python tools/native/test_frame_delta.py
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(preallocation|preallocation_output|flight_start)$'
```

Broader independent full-flight drawing, original audio onset/handoffs and
visible performance remain open. The uninterrupted campaign is waived;
named-state cleanup remains outside this goal.
