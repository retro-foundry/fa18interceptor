# Bounded radar paint history - 2026-10-09

Two recorded cockpit windows now have complete pixel-history explanations.
The original and native radar owners draw the same source-defined geometry
and marker colours on their actual inputs. Their observed blink counter offset
changes when a selected marker erases background bits. A later instrument
bitmap redraw restores those bits. This extends the earlier single-pixel
marker-cache result without changing gameplay, clocks or captured pages.

The C31226 owner is exercised through the playable frame body and compared
with original instructions, ending at its actual saved C0F18E return. Ordered
crosshair lines, fixed points, old-cache erasure and marker writes independently
predict every byte of all eight complete owner pages: **70 page sets,
4,480,000 bytes**, across 35 original/native observations. All live original
and native owner returns pass, with 140 non-stack comparisons (including
external phase probes) and 35 native frame-body comparisons under their
existing explicit scope. The standalone oracle is validation code; no runtime
module or allocation path changes in this batch.

| Original observations | Complete cross-runtime plane histories | Compared XOR bytes |
|---|---|---:|
| 22,302..22,325 | Planes 1 and 2 on both pages | 768,000 |
| 23,944..23,954 | Plane 2 on both pages | 176,000 |

Every requested-plane byte is checked at each observation, starting with
identical actual plane bytes. Draw/display roles follow the observed page
switches. The prediction carries ordered paints forward; it does not fit a
clock offset or mask differences. C30764 frame redraws copy the actual four
immutable 2,200-byte instrument images to all 55 instrument rows. Both
runtimes use identical image hashes and redraw events in these windows.

At x=163/y=162 in the later window, the initial crosshair colour is 3. The
selected marker's colour 1 clears its colour-2 background bit; old-cache
erasure later clears the remaining bit. A crosshair redraw restores colour 3.
The difference appears on one page, then both, then one, and disappears by
observation 23,954. The earlier plane-1 difference disappears after the
instrument bitmap redraw at body 22,318. These are source-defined destructive
writes whose different blink phases leave different retained page contents.

Lost erasure, wrong marker colour and wrong line slope each fail complete
owner-page prediction. Omitting the instrument redraw also fails the earlier
cross-runtime history. That last mutation is explicitly unobservable in the
later selected-plane window, where head redraw already removes its difference.
The existing four phase/point mutations still fail, and the prior five-frame
complete-page cache-delta check passes unchanged.

## Binding and limits

The early window retains its historical executable and trace hashes; it is
not relabelled as a fresh current-build capture. The current six-point-model
Release and Debug full-flight reports preserve all previous observation
fields, complete record cores and page hashes, final RAM, counters and earned
saves. Both retain zero project gameplay heap violations, SDL pool requests
and failures. The committed [checkpoint](figures/native_radar_paint_history_checkpoint.json)
binds both fixture generations, current whole-flight reports, tool hashes,
owner RAM/page hashes, plane histories, image hashes and negative controls.

Strict complete drawing remains **287/4,967** same-observation matches; all
rows 0..127 match. This bounded evidence explains the requested radar planes.
Later message-colour differences and broader whole-flight drawing remain
open, as do original audio onset/handoff and visible performance. The
uninterrupted campaign remains waived; named-state cleanup is deferred.

## Reproduction

```powershell
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/mission-three-second-drawing-review --original-bodies build/native-flight/mission-three-second-body-review --paint-history --paint-planes 1 2 --out build/native-flight/cockpit-early-paint-history
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/cockpit-plane-two-window --original-bodies build/native-flight/cockpit-plane-two-radar-bodies --trace-evidence build/native-flight/filtered-cockpit-trace/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --paint-history --out build/native-flight/cockpit-plane-two-paint-history
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/cockpit-bar-window --original-bodies build/native-flight/cockpit-radar-correct-return --trace-evidence build/native-flight/filtered-cockpit-trace/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --complete-page-delta --out build/native-flight/cockpit-cache-delta-regression
```

Local retained RAM is compressed; passing raw artifacts remain disposable.
