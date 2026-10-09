# Cockpit history when the selected marker is absent - 2026-10-09

All eight complete planes are explained across independent mission-three
observations **22,353..22,403**: **3,264,000 actual XOR bytes**. The second
plane-0 episode contains **48 differing observations**, now accounted for by
actual radar, instrument bitmap and message writes under the accepted
rendering-cadence policy. No pixels or clocks are changed or excluded.

The full-flight audit still exits failure. Plane 0 has **1,825** unexplained
observations, down from 1,873; plane 3 retains **2,785**. Missing certificates
and complete record-core differences remain failures. This closes a complete
drawing episode, not the whole flight.

## Verifier correction

The selected record exists at offset 4,096 but is not painted in this `NO SIG`
window. Another contact, offset 7,168, is painted in colour 5. The old radar
control setup required a visible selected marker even for its counter and
ordinary-contact tests, and its colour mutation targeted only colours 1/8.
Neither assumption is valid for this actual original input.

The counter and ordinary-contact controls now use their own actual cases.
Selected-marker coordinate/premature-draw probes are explicitly unobservable
only when all source/native actual and phase-probe outputs contain no selected
marker. A report cannot hide missing counter/contact controls or label visible
markers as absent. The colour control uses an actual visible contact when
there is no colour-1/8 marker. All erase, colour and head-line controls remain
mandatory and observable here.

The existing selected-target window is rerun through the changed code. Its
complete report remains byte-identical, including all four phase controls,
all live owner pages and every prior paint-history byte. Eleven tests using
actual captured control cases also reject missing controls, false inactive
claims, wrong ordinary-contact coordinates and wrong history-control claims.
The existing five complete-plane acceptance tests pass.

## Independent history and meaningful omission controls

The combined model starts from identical actual pages and follows the actual
draw/display page changes. Instrument bitmap copies, ordered radar operations
and original immutable-font glyph writes predict every next complete page XOR.
It consumes actual source owner calls/returns and original execution from
actual native frame inputs, without supplying any RAM to native gameplay.

Removing identical bitmap/radar writes from **both** predicted histories can
leave their XOR unchanged. That paired removal is explicitly unobservable in
this episode. Removing actual source bitmap copies instead first fails at
**22,383**, and removing actual source radar writes fails at **22,355**.
Removing message writes fails at **22,354**. Reports identify the actual
single-source probe and the unobservable paired probe; the plane audit rejects
wrong owner substitutions or failures claimed outside the certified window.
The positive check still compares every captured page byte.

The actual-owner checks cover **204 non-stack radar component executions**,
**102 message component executions**, and **204 complete live owner page
predictions / 13,056,000 bytes**. All 51 native bodies also execute original
instructions with zero differences under the unchanged frame-body exclusions.
Neutral message colour clearing remains explicitly unobservable; losing the
clear that removes old glyph pixels is rejected.

## Runtime scope and evidence

Only the diagnostic verifiers and their control fixtures change. The playable
runner, game behavior, allocation ownership and source timing remain unchanged.
The current Release hash stays
`7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.
Native composition remains `native_frontend_tick -> native_flight_tick ->
native_hud_draw`; the captured bitmap/radar/message children follow that actual
caller order. Reference CPU instructions remain outside the native runner.

The [checkpoint](figures/native_no_sig_cockpit_history_checkpoint.json) binds
the complete streams, all selected window/body/owner identities, component
reports, the unchanged selected-target regression, actual controls and failed
full-flight inventory. Previous capture and history proof hashes stay historical.

## Reproduction

```powershell
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/compact-second-combined-window/window --original-bodies build/native-flight/compact-second-combined-window/radar --trace-evidence build/native-flight/frame-delta-trace-preservation/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --paint-history --paint-planes 1 2 3 --out build/native-flight/compact-second-combined-radar-history-final
python tools/native/assess_mission_cockpit_history.py --window build/native-flight/compact-second-combined-window/window --radar-history build/native-flight/compact-second-combined-radar-history-final/report.json --radar-bodies build/native-flight/compact-second-combined-window/radar --message-pages build/native-flight/compact-second-combined-message-pages --message-bodies build/native-flight/compact-second-combined-window/message --trace-evidence build/native-flight/frame-delta-trace-preservation/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --out build/native-flight/compact-second-combined-history-final.json
python tools/native/test_radar_control_observability.py
python tools/native/test_mission_plane_history.py
```

Whole-flight drawing, other full-flight comparisons, original audio timing and
visible performance remain open. The uninterrupted campaign is waived;
named-state cleanup remains outside this goal.
