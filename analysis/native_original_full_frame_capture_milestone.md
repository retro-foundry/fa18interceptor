# Complete original flight and drawing-owner capture - 2026-10-09

The independently replayed original mission-three flight now has compact,
complete RAM/register evidence for **all 4,967 input observations**, **4,965
actual frame bodies**, and **3,602 actual calls/returns each** of the radar and
message owners. The stream contains **29,305 complete MiB snapshots**.

The complete original replay preserves every **27,110** trace observation,
runtime counter and final RAM byte. The same recorder reproduces **320**
previous before/after/owner RAM and register comparisons exactly. Every one of
the **7,204** actual drawing-owner returns also matches its original return PC
and restored caller stack; a wrong caller stack is rejected.

This makes the original accumulated page/cache history available across the
entire flight. It does not yet accept every original/native drawing difference.
The previous full-plane audit remains failed: 1,873 observations of plane 0
and 2,785 of plane 3 are still unexplained.

## Connected reference diagnostic

The reference runner retains its actual callers and original scheduling.
`fa18_ports_enter -> fa18_loop_iteration` adds a read-only observation before
the unchanged loop increments, trace publication and recorded input delivery.
The instruction bus wrapper captures the original C0EFEA/C0F3C0 body and
C31226/C0F18E radar and C322EE/actual message-return boundaries, then forwards
the original bus operation. Registers and RAM are read directly from host
storage, without extra emulated bus reads or clock cycles.

Both adapters live in `tools/native/` and are selected only by the existing
reference builder's source-replacement option. They reuse the host delta
writer, with fixed one-MiB previous-state storage and fixed file buffers.
No native gameplay, executable or composition is changed. Captured state
never supplies native gameplay.

The two true original resumption observations have input captures without
another body. Idle bodies have no radar/message call captures. The recorder
preserves these actual absences rather than generating extra updates or owners.

## Budget, integrity and integration

The unchanged 512 MiB diagnostic budget is split into 256 MiB for the existing
complete trace and 256 MiB for compact RAM/register data. Actual total output
is **395,649,296 bytes**: 250,829,740 trace bytes, 139,292,052 delta bytes and
5,527,504 register bytes. The retained RAM/register gzip pair is only
**23,706,753 bytes**. Temporary raw files are removed after validation.

Every decoded MiB has its own SHA-256 and each stream has a terminal count.
Register metadata is hashed separately. Source inputs, recorder source and
binary are pinned. Decoding permits original instruction PCs only when the
caller supplies the exact allowed set; native captures retain their original
boundary-enum validation. Seven decoder tests pass, including that distinction.

The current Release binary also reproduces all 27,110 original and 16,062
native enriched trace observations, final RAM and the earned pilot. Its
hash remains `7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.
The existing source-derived PCM change is explicitly recorded against the
historical baseline; all game and voice counters remain strict.

`extract_mission_delta_window.py` reconstructs bounded complete entry/body and
original owner fixtures for the existing drawing checkers. The first forty
observations reproduce every previous source/native entry hash, complete
native body and timing sidecar exactly. The passing extracted RAM copies are
removed; the full streams and earlier canonical window reproduce them.
Controls reject an unpreserved source trace and a corrupt register digest.

## Next drawing case remains unaccepted

The extracted **22,353..22,403** window includes the next plane-0 drawing
episode. Its **51 native bodies**, **102 actual message owners** and all
**6,528,000 complete owner-page prediction bytes** pass, including glyph,
plane and observable cell-clear controls. Neutral messages leave the existing
alternate-colour clear control explicitly unobservable.

The radar verifier checks its actual owner outputs, then stops at the selected
target mutation setup because this window has no selected marker (`NO SIG`).
Its assumption that every window contains an observable selected marker is
not valid here. The first actual source owner still paints another contact
(record offset 7,168, colour 5); its selected record is offset 4,096. Inactive
selected-marker controls must be distinguished from observable counter and
regular-contact controls. No radar report or combined-history acceptance is published.
The failing case stays retained for a source-backed assessment of inactive
marker controls. This is a verifier limitation, not a confirmed gameplay fault.

The [checkpoint](figures/native_original_full_frame_capture_checkpoint.json)
binds the complete source stream, register metadata, all snapshot identity
digests, recorder sources, current flight preservation, exact earlier window,
caller-return controls and this pending drawing case.

## Reproduction

```powershell
python tools/native/check_original_frame_delta.py --source-evidence build/native-flight/original-mission-three-patrol-runway --reference build/native-flight/frame-delta-trace-preservation/Release --out build/native-flight/original-frame-delta-complete-flight --window-radar build/native-flight/cockpit-first-combined-radar-bodies --window-message build/native-flight/cockpit-first-combined-message-bodies
python tools/native/check_original_frame_delta.py --source-evidence build/native-flight/original-mission-three-patrol-runway --reference build/native-flight/frame-delta-trace-preservation/Release --verify-existing build/native-flight/original-frame-delta-complete-flight --out build/native-flight/original-frame-delta-call-returns
python tools/native/extract_mission_delta_window.py --source-delta build/native-flight/original-frame-delta-complete-flight --native-delta build/native-flight/frame-delta-complete-flight/Release --reference build/native-flight/frame-delta-trace-preservation/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --first 22353 --count 51 --out build/native-flight/compact-second-combined-window
python tools/native/test_frame_delta.py
```

Independent complete-flight drawing, original audio onset/handoffs and visible
performance remain open. The uninterrupted campaign is waived; named-state
cleanup remains outside this goal.
