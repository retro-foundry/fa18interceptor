# Complete independent Mission Five drawing history

All **13,775** observations of the independently started, successfully
completed Mission Five flights pass the complete drawing-history gate.
Every byte of the **881,600,000 eight-plane XOR-history bytes** is predicted
by the original drawing owners on each recording's actual inputs. There is
no unexplained observation, omitted-owner acceptance or masked pixel region.
The comparison includes mission entry, formation/combat, carrier arrest,
earned grade one and menu return. Native gameplay is unchanged in this batch.

The result is source-rule parity under the established timing policy;
**raw scene pixels are not identical throughout the flight**. Raw scene bands
match at 12,145 observations. The remaining differences are assessed through
their original owners, actual carried prefix state and observed timing,
rather than erased or treated as unexplained rendering faults.

## Complete scope and timing assessment

The gate checks all original observations 61,797–75,571 and all corresponding
native observations. It verifies both complete trace identities and every
gameplay/core observation, **117,099 sealed original source snapshots** and
**13,763 actual native frame-body identities**. Twelve observations contain
no new body dispatch and remain in the drawing history. Paused observations
are preserved with their original retained pages.

For every one of the **12,633 active paired drawing owners**, freshly drawn
scene geometry matches before head-up drawing: **258,723,840 active scene
bytes**. All four inactive page planes preserve the preceding actual history.
Panel, radar, head-up and message owners then predict their complete writes
in order. Both recordings start this drawing comparison with the same full
pages; no reference RAM is supplied to playable native gameplay.

Original C332BC explains the head-up marker/range differences carried from
the two genuinely earned escort prefixes. All 12,633 original and 12,633
native head-up writes are predicted on those actual inputs. The earlier
[head-up evidence](native_mission_five_headup_owner_milestone.md) traces the
first 395 differing scene bytes and their retained producers. The gate now
checks that explanation through every later drawing observation, including
shared fresh geometry and retained pages.

The complete [message assessment](native_mission_five_message_timing_milestone.md)
already checks every real elapsed/countdown transition and HUD text write.
First HDG occurs at original tick 173/native tick 181 after two sampled-second
changes in each. Different sampled-clock transitions remain explicit under
the agreed native timing policy; no artificial delay is added. The drawing
gate includes every resulting message, erase, glyph, plane and page write.

Radar selection is observed separately for each complete radar prefix.
Original source input 62,005/native 61,302 changes selection between two
tails within one owner. The [per-prefix evidence](native_mission_five_radar_prefix_selection_milestone.md)
corrects the old checker's stable-selection assumption without changing any
old event or output byte. Every actual radar point and final selector is
checked in the completed run.

## Rejection controls and preservation

All controls pass after the complete history is checked. Four phase controls
reject changed counter advancement, early markers and altered marker
coordinates; none is unobservable. Three paint controls reject wrong marker
colour, head slope and lost erase. Three glyph controls reject wrong glyph,
plane and cell clear. Both roles reject missing head-up set/clear writes,
the wrong page role and an omitted owner: eight further controls.

Omitting each original owner is also rejected at its first effective write:
head-up at observation 61,798; panel and radar at 61,814; message at 61,957.
Terminal controls use retained complete-flight inputs. The compressed progress
journal preserves every preceding verified history and paired owner.

The unchanged native runner SHA-256 is
`544bb244eb048b65436382a755cbf9bd9abd29f1337aec849967ef157fac3bd8`.
The checker verifies its bound inputs, tools and five oracle identities
before accepting the report. The earlier run rejected by the radar-control
assumption remains historical rejected evidence; it is not relabelled as a
passing complete comparison.

The [checkpoint](figures/native_mission_five_complete_drawing_checkpoint.json)
binds the complete report, every verifier source, input identities, control
results, journal, terminal inputs and exact compressed oracle executables.
The five executables are removed only after decoded hashes match the original
report; their lossless compressed forms remain reusable. The existing cache
pruner runs after completion. Canonical recordings, compressed original RAM
and user settings are preserved.

```powershell
python tools/native/check_complete_cockpit_history.py --headup --source-delta build/native-flight/original-mission-five-frame-delta-1800 --native-delta build/native-flight/mission-five-template-carry-complete-bodies --reference build/native-flight/mission-five-template-carry-message-trace --source-evidence build/native-flight/original-mission-five-touchdown-approach --source-updates build/native-flight/original-mission-five-touchdown-updates --native-prefix build/native-flight/recorded-escort-template-carry-protected --replay-evidence build/native-flight/independent-mission-five-template-carry --runner build/native/fa18_native.exe --out <new-comparison-directory>
```

This completes the Mission Five drawing assessment on these independently
started flights. It adds to the accepted complete Mission Three comparison;
it does not claim pixel identity, qualify other unrecorded flight paths,
resolve the user's unsealed Free Flight road route, or complete native sound
onset/handoff/waveform matching. Named-state cleanup remains a separate task;
the uninterrupted campaign requirement remains waived by the user.
