# Combined cockpit drawing history - 2026-10-09

Every byte of all eight complete page planes is now explained across original
observations **22,302..22,341** of the independently started successful
mission-three flight. The combined radar, instrument bitmap and message
history predicts **2,560,000 actual XOR bytes** without masks or clock changes.
This closes the entire first plane-0 episode: **38 differing observations**,
including the return to identical pages at 22,341.

The full-flight audits still fail. Plane 0 now has **1,873 unexplained flight
observations**; plane 3 retains **2,785**. The first plane-3 episode's seventeen
observations remain explained, and the complete plane-1/2 reports remain
identical. This is bounded drawing progress, not full-flight acceptance.

## What explains the episode

The early difference is the selected radar marker and the pixels erased from
its previous position. From observation 22,309, the retained cockpit message
differs too: the original has switched to HDG while native still holds ALT.
The existing full-flight message contract already verifies both sequences
against the source's elapsed-second, cache and redraw rules. The new check
accounts for their exact glyph pixels and colour-plane writes under the
accepted rendering-cadence policy.

Both histories start with all eight actual complete pages identical. Expected
buffers follow the actual draw/display page switches. Each body applies the
original instrument image copy, then the actual radar writes, then message
glyph writes from the immutable font and layout. No captured page is modified.
The complete XOR of every plane must match at every next observation.

Omitting radar writes first fails at **22,303**, omitting message writes at
**22,309**, and omitting instrument redraw at **22,319**. The owner sequence
therefore matters to the result; complete page comparison cannot silently
accept any of those omissions.

## Actual owners and runtime integration

Two independent original probes retain actual C31226/C0F18E radar and
C322EE/C0F286 message boundaries. Each preserves all **22,342** preceding
recording observations. Their body-begin RAM hashes agree. Current native
captures reproduce every prior trace field, all sixteen complete record cores
and all eight live page hashes. Forty unique native bodies pass original
execution with the existing explicit scratch/audio/busy-state exclusions.

The component checks add **160 non-stack radar comparisons**, **80 non-stack
message comparisons**, and **160 complete live owner page predictions /
10,240,000 bytes**. Original text returns and saved caller identities remain
strict. Native composition remains `native_flight_tick -> native_hud_draw`;
the actual radar and message children are exercised in that caller order.
No playable code, runtime allocation or binary changes are introduced.

All messages in this window use the neutral colour. Alternate message colour
planes are already blank, so the existing lost-alternate-plane-clear mutation
is explicitly unobservable here. Losing the clear that removes old glyph
pixels is observable and rejected. The tool permits this substitution only in
neutral-message windows, retains the unobservable control in its report and
requires wrong-plane and wrong-glyph controls to fail as well. Earlier five-
and seven-observation message reports remain identical, including their
observable alternate-plane-clear controls.

The [checkpoint](figures/native_combined_cockpit_history_checkpoint.json) binds
current full-flight and executable hashes, every owner input/return identity,
component reports, all forty complete XOR histories, bitmap image hashes and
negative controls. The audit gate also rejects a false omitted-owner control
before publishing any result. Passing raw RAM remains disposable; retained
original RAM and native message-owner fixtures are compressed.

## Reproduction

```powershell
python tools/native/check_mission_message_pages.py --window build/native-flight/cockpit-first-combined-window --original-bodies build/native-flight/cockpit-first-combined-message-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --retain-native-owners --out build/native-flight/cockpit-first-combined-message-pages
python tools/native/assess_mission_radar_cadence.py --window build/native-flight/cockpit-first-combined-window --original-bodies build/native-flight/cockpit-first-combined-radar-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --paint-history --paint-planes 1 2 3 --out build/native-flight/cockpit-first-combined-radar-history
python tools/native/assess_mission_cockpit_history.py --window build/native-flight/cockpit-first-combined-window --radar-history build/native-flight/cockpit-first-combined-radar-history/report.json --radar-bodies build/native-flight/cockpit-first-combined-radar-bodies --message-pages build/native-flight/cockpit-first-combined-message-pages --message-bodies build/native-flight/cockpit-first-combined-message-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --out build/native-flight/cockpit-first-combined-history.json
python tools/native/assess_mission_plane_history.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --history build/native-flight/cockpit-first-combined-window build/native-flight/cockpit-first-combined-history.json --audit --plane 0 --out build/native-flight/mission-three-plane-zero-combined-audit.json
```

The last command intentionally exits 1 for the remaining unexplained
observations. Whole-flight drawing, other flights, original audio timing and
visible performance remain open. The uninterrupted campaign is waived;
named-state cleanup remains outside this goal.
