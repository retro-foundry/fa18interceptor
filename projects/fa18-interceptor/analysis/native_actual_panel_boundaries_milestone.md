# Actual panel boundaries throughout mission three — 2026-10-09

The independent original replay now retains **3,602 actual C30764 calls and
C0F182 returns**, alongside the previous input, body, radar and message
boundaries. All **29,305 previous complete RAM/register snapshots remain
identical**, and 408 retained snapshots from the second bounded drawing
episode match too. The full 27,110-row trace, final RAM and runtime counters
remain unchanged.

This resolves a verifier assumption exposed by following the drawing history
from the start of the flight. At original observation 22,441, the redraw
counter is zero at C0EFEA, but three when C30764 consumes it; its return leaves
two. The row changes from zero to minus one before that call. Reading the
counter and row at frame entry therefore misses an actual bitmap copy. This
was a diagnostic error, not evidence of a visible game fault.

The recorder exports directly from the original's host storage without adding
emulated bus cycles. Its new panel return requires the saved C0F182 caller
address and restored stack pointer. All **10,806 actual panel, radar and
message returns** satisfy their caller contracts; an incorrect caller stack
is rejected. The original C0F17C call is authoritative, not an inferred
counter value or a fitted delay.

The complete capture contains 36,509 full-MiB snapshots in **25,785,248 bytes
of gzip RAM/register files**. Raw RAM deltas, register metadata and the full
trace together occupy 409,644,702 bytes, within the existing 512 MiB diagnostic
budget. Passing raw files are removed. Neither these captures nor reference
CPU execution supply native gameplay state.

The playable native executable is unchanged. The connected drawing path is
`main -> native_frontend_tick -> native_flight_tick -> native_hud_draw ->
draw_panel_frame`, corresponding to the original C0F17C/C30764 call. These
source captures support the ongoing complete drawing-history check; they do
not themselves grant full-flight pixel acceptance. Audio timing and visible
performance remain open. The uninterrupted campaign is waived and named-state
cleanup remains outside the goal.

```powershell
python tools/native/check_original_frame_delta.py --source-evidence build/native-flight/original-mission-three-patrol-runway --reference build/native-flight/frame-delta-trace-preservation/Release --previous-stream build/native-flight/original-frame-delta-complete-flight --window-radar build/native-flight/compact-second-combined-window/radar --window-message build/native-flight/compact-second-combined-window/message --out build/native-flight/original-frame-delta-panel-complete-flight
python tools/native/check_original_frame_delta.py --source-evidence build/native-flight/original-mission-three-patrol-runway --reference build/native-flight/frame-delta-trace-preservation/Release --verify-existing build/native-flight/original-frame-delta-panel-complete-flight --out build/native-flight/original-frame-delta-panel-caller-returns
```

The [checkpoint](figures/native_actual_panel_boundaries_checkpoint.json) binds
the capture, prior stream, current traces, counter example, caller verification
and observer sources. Earlier sealed reports keep their original identities.
