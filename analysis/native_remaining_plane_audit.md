# Remaining complete-flight drawing audit - 2026-10-09

This earlier inventory is superseded for the first plane-0 episode by the
[combined cockpit history](native_combined_cockpit_history_milestone.md).
That episode is now explained; the current plane-0 unresolved count is 1,873.

The first mission-three plane-3 episode is now explained by the original radar
head, erasure and marker writes, including instrument bitmap redraws. All
complete plane-3 bytes on both pages match the predicted XOR history through
observations **22,302..22,325**, covering all **17 differing observations** in
the first episode. This adds 384,000 compared bytes to the bounded histories;
it does not complete plane 3 or the whole flight.

The full-flight inventory now reports every unexplained observation instead
of stopping at the first missing certificate. Audit mode exits **1** whenever
any difference remains unexplained. The default complete-plane acceptance
still rejects missing histories, and complete gameplay record cores remain
strict in both modes. Accounted bytes include only exact matching complete
planes and complete planes with validated histories.

| Plane | Exact matching observations, draw / display | Explained differing observations | Unresolved flight observations |
|---|---:|---:|---:|
| 0 | 3,489 / 3,488 | 0 | 1,911 |
| 3 | 2,182 / 2,182 | 17 | 2,785 |

The plane-3 work still open forms two continuous intervals: 23,311..25,705 and
25,710..26,099. Plane 0 has fifteen intervals, recorded in the checkpoint.
Every upper-screen row 0..127 continues to match across all eight planes.
The five adjoining diagnostic bands cover every byte of the complete pages;
no band or pixel is excluded from the complete-plane gate.

## Evidence and integration

The retained early window keeps its historical executable identity. Its
complete pages reproduce the preserved current Release flight hashes before
any bounded result is counted. All **96 non-stack owner comparisons**, 24
native body comparisons and 48 actual original/native owner page sets pass.
The native caller is `native_flight_tick -> native_hud_draw ->
draw_postflight_renderer_dispatch`; the original owner is C31226 and its actual
saved return is C0F18E. This diagnostic adds no playable code or allocation.

The radar predictor first fails to explain plane 0 at observation **22,309,
display-role plane 4**. That is an investigation boundary, not a confirmed
rendering fault or evidence that the remaining differences are all radar.
Other HUD writes must be accounted for before those bytes can pass.

The earlier plane-1 and plane-2 complete-flight reports remain identical.
Five acceptance tests cover missing history, explicit unresolved inventory,
accounted-byte scope, contradictory certificates and complete record cores.
Lost erase, wrong marker colour, wrong head slope, lost instrument redraw and
the existing four radar phase/point mutations remain rejected.

The [checkpoint](figures/native_remaining_plane_audit_checkpoint.json) binds
current flight hashes, tool hashes, full audit report hashes, unresolved
intervals, every bounded owner-page prediction and actual bitmap image hashes.
Passing raw RAM remains disposable; retained reference RAM is compressed.
Broader full-flight drawing, original audio timing and visible performance
remain open. The uninterrupted campaign is waived and named-state cleanup is
outside this goal.

## Reproduction

```powershell
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/mission-three-second-drawing-review --original-bodies build/native-flight/mission-three-second-body-review --paint-history --paint-planes 1 2 3 --out build/native-flight/cockpit-early-three-plane-history
python tools/native/assess_mission_plane_history.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --audit --plane 0 --out build/native-flight/mission-three-plane-zero-audit.json
python tools/native/assess_mission_plane_history.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --history build/native-flight/mission-three-second-drawing-review build/native-flight/cockpit-early-three-plane-history/report.json --audit --plane 3 --out build/native-flight/mission-three-plane-three-audit.json
python tools/native/test_mission_plane_history.py
```

Both audit commands intentionally return failure while their reported
unresolved observations remain open.
