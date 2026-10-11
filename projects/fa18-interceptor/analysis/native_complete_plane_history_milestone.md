# Complete mission-three plane histories - 2026-10-09

Every byte of bitplanes 1 and 2 on both pages is now accounted for across the
independently started successful mission-three flight. Each plane covers all
**4,967 observations / 9,934 complete page-plane observations / 79,472,000
bytes**. Exact matching hashes establish zero delta; every nonzero delta is
bound to separately validated actual-input paint histories and retained pages.
This extends bounded radar/message evidence to these two complete-flight
planes. It does not accept the other planes or the complete scene.

| Plane | Strict matching observations per draw/display role | Differing flight observations | Complete explanation |
|---|---:|---:|---|
| 1 | 4,953 / 4,967 each | 24 | Radar erasure, message glyph/colour writes, instrument redraw |
| 2 | 4,944 / 4,967 each | 26 | Radar head/marker/background erasure and instrument redraw |

The new gate rechecks the complete current original/native traces, original
ordinary-recording preservation, native baseline preservation and the actual
consumed-update mapping. All sixteen complete record cores match at every
flight observation. For every bounded history row, retained original/native
pages must reproduce all eight current live page hashes, and every actual
XOR byte/hash/count must match the certified paint history. Removing the last
history fails the full-flight gate. No page region, different pixel or matched
phase is discarded, and no clock or recording offset is fitted.

## Later message window and connected integration

The later original observations 26,098..26,104 retain every remaining plane-1
colour episode. All 26,105 preceding original observations are unchanged.
Seven native bodies, fourteen actual message owners, seven live original text
returns and **896,000 complete owner-page bytes** pass. Wrong plane, lost cell
clearing and wrong glyph pixels fail; captures including another caller
instruction are rejected. The source/native caches display HDG versus ALT,
then SPD versus ALT, using their actual source-defined countdowns and cached
values. Both turn neutral after the target loses its signal.

The earlier five-observation message window now also predicts every plane-1
XOR byte through both pages. Both message histories start from identical actual
planes, follow actual draw/display role switches, and carry actual glyph and
clear writes forward. Instrument copies use all four actual immutable
2,200-byte images when C30764 requests a redraw. Shared bitmap-copy logic now
serves the radar and message predictors; both earlier radar histories preserve
every prior row and image hash through that helper.

The playable caller remains `native_flight_tick -> native_hud_draw ->
draw_message_line`. Native body exports come from the current runner; complete
original frame execution on those actual inputs supplies message entry and its
saved C0F286 return. The live original owner is captured independently at its
actual entry/return, not reconstructed from a native state. Components compare
every non-stack byte; full frame-body integration retains its existing explicit
scratch/audio/busy-state scope. No playable code or allocation changes here.

## Timing policy, binding and limits

The complete current message contract already passes all 4,964 real transitions
and nine negative controls. Its source-defined elapsed-second page cycling,
cache, kind and redraw rules explain the differing message contents under the
agreed rendering-cadence policy. Radar paint histories likewise retain actual
counter phases, source heads and erasure operations. The new full-plane gate
accounts for their exact differences; strict same-observation pages remain
**287/4,967**. Rows 0..127 still match across all four planes of both pages.

The early radar window keeps its historical fixture identity, bound through
whole-trace preservation and actual current page hashes. The two later message
windows use the current six-point-model Release runner. Release and Debug's
complete current trace hashes agree, as do final RAM, counters, earned saves
and memory reports; both retain zero project gameplay heap violations, SDL
pool requests and failures. No historical executable is relabelled as current.

The [checkpoint](figures/native_complete_plane_history_checkpoint.json) binds
whole-flight reports, every differing observation/history, actual message-owner
RAM/page hashes, bitmap events, negative controls, source/tool hashes and both
current builds. Local RAM remains compressed. Whole planes 0 and 3, other
full flights, original audio timing and visible performance remain open. The
uninterrupted campaign is waived; named-state cleanup stays deferred.

## Reproduction

```powershell
python tools/native/check_mission_message_pages.py --window build/native-flight/cockpit-message-colour-window --original-bodies build/native-flight/cockpit-message-colour-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --plane-history --out build/native-flight/cockpit-message-plane-one-history
python tools/native/check_mission_message_pages.py --window build/native-flight/cockpit-later-colour-window --original-bodies build/native-flight/cockpit-later-colour-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --plane-history --out build/native-flight/cockpit-later-plane-one-history
python tools/native/assess_mission_plane_history.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --history build/native-flight/mission-three-second-drawing-review build/native-flight/cockpit-early-paint-history/report.json --history build/native-flight/cockpit-message-colour-window build/native-flight/cockpit-message-plane-one-history/report.json --history build/native-flight/cockpit-later-colour-window build/native-flight/cockpit-later-plane-one-history/report.json --plane 1 --out build/native-flight/mission-three-complete-plane-one.json
python tools/native/assess_mission_plane_history.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --history build/native-flight/mission-three-second-drawing-review build/native-flight/cockpit-early-paint-history/report.json --history build/native-flight/cockpit-plane-two-window build/native-flight/cockpit-plane-two-paint-history/report.json --plane 2 --out build/native-flight/mission-three-complete-plane-two.json
```
