# Complete mission-three drawing history — 2026-10-09

The independent original/native mission-three flight now passes the complete
eight-plane drawing-history gate: **4,967 observations and 317,888,000 page XOR
bytes**, with no unexplained observation. All sixteen complete aircraft/actor
cores and the named gameplay/camera fields match at every observation. Every
byte of the upper 128 scene rows matches directly on both pages, accounting
for 203,448,320 scene bytes.

This follows the flight from its common initial pages through completion and
menu return. It does not restart the model at selected matching frames. The
two actual source dispatches without a body, 22,501 and 24,007, do not advance
native drawing twice. All 4,965 native bodies are freshly checked against
original instructions on their actual inputs and captured API clock
intervals, with zero gameplay/display differences under the existing explicit
frame-body scratch, voice, busy-state and ABI contract. Complete page bytes
are not excluded by that contract.

Each original drawing input comes from the independent original's actual
call boundary. Each native input comes from original-instruction execution
of an actual native body whose complete output matches the playable runner.
The connected path is `main -> native_frontend_tick -> native_flight_tick ->
native_hud_draw`, including the C30764 panel, C31226 radar and C322EE message
owners. All 3,602 instances of each owner on each runtime pass standalone
native C/original instruction comparisons and complete live page predictions:
**21,612 owner predictions and 1,383,168,000 page bytes**. Radar phase probes
add 7,204 component executions, for 28,816 component executions in total.

Separate expected pages receive immutable bitmap copies, ordered radar
erases/marks and immutable glyph/cell writes using the actual source rules.
They carry forward with each real page swap. Scene bytes are required to
match before their shared value is copied into both expected buffers; a
changed scene byte fails. Every remaining byte must follow the owner history.
Both pages remain fully checked, including unchanged and overwritten pixels.
No pixel masks, clock offsets, fitted delays or reference RAM in native
gameplay are introduced.

The full check exposed a diagnostic error: frame-entry redraw state can
change before the panel call. Actual call-boundary captures now supply that
state. The bitmap and glyph predictors also implement the source's wrapped
ADD.L row translation, including minus-one-row destinations; positive panel
lift shortens the copy without skipping source rows. Every actual owner
prediction is checked against complete live pages. The prior 51-observation
combined report remains byte-identical.

All four radar counter/marker controls, all three radar paint controls and
all three glyph/plane/cell-clear controls reject their mutations. Omitting
source panel, radar or message writes independently fails at observations
22,149, 22,149 and 22,302 respectively. Ten new byte/role/identity/caller
guards and 23 existing guards pass. Complete streams must match their sealed
identities, metadata and successful footer; a truncated or missing late
capture cannot become full-flight acceptance.

Strict simultaneous page equality remains 287/4,967. The other observations
are explained by original drawing rules on each runtime's actual accepted
clock/cadence state. This completes the drawing-history assessment for this
recorded Release flight under the agreed timing policy; it does not assert
identical display timestamps, RGB publication, every other flight, or complete
original audio waveform timing. Audio onset/handoffs and visible performance
remain open. The uninterrupted campaign is waived; named-state cleanup stays
outside the goal.

The playable Release executable remains
`7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.
Its complete qualification/mission/menu run, earned pilot, final RAM and
zero gameplay heap violations remain preserved. These changes affect
reference diagnostics only. Passing raw owner RAM is temporary, and the
complete source/native delta streams remain compressed. No budget or pruner
hook changes are made.

```powershell
python tools/native/check_complete_cockpit_history.py --source-delta build/native-flight/original-frame-delta-panel-complete-flight --native-delta build/native-flight/frame-delta-complete-flight/Release --reference build/native-flight/frame-delta-trace-preservation/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/complete-cockpit-owner-history
python -m unittest discover -s tools/native -p test_complete_cockpit_history.py
```

The [checkpoint](figures/native_complete_cockpit_flight_checkpoint.json)
binds the full history, native/source streams, caller verification, current
traces, actual-owner example, guard results and executed tool identities.
An outdated panel-oracle header comment was corrected after validation; its
exact reversible comment change preserves every remaining source byte and
line count. Executed source identities remain recorded unchanged.
