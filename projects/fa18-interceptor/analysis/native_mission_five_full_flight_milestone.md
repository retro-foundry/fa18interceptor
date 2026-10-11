# Independent complete mission-five comparison - 2026-10-10

The native runner completes mission five using the successful original flight's
recorded controls and matches its gameplay throughout the entire flight. All
**13,775 observations**, **220,400 complete object record cores** and every
declared camera/control field match. Both games earn grade one, the third
completion and menu return. Full drawing and timer/HUD equality remains open;
this result establishes complete-flight gameplay parity for this recording.

The original recording reproduces all **75,573 observations**, final RAM and
consumed controls exactly. Executed `C15DA2` JSR calls and `C0EFD4` LINK
instructions establish **75,546 real updates**, retaining all **27 duplicate
observations** and the order/PAL timestamps of **25,954 key edges**. The first
arrest snapshot at observation **74,641** agrees with all 16 complete record
cores, 42 named fields and eight drawing planes. Four caller/LINK/resumption
guards reject altered evidence. The probe timeout now follows the existing
mode-specific recording budgets, including 900 seconds for this longer flight;
capture limits and playable timing are unchanged.

Native starts from ordinary enlistment, then replays the verified qualification,
patrol and escort inputs before taking the next original mission's controls.
Every byte of the preceding **27,176 native trace rows**, including its actual
ordinals and frames, reproduces the earlier successful native recording. That
prefix independently earned qualification, grades three/four and two completions.
Its saved pilot is checked against actual captured RAM and disk-export hashes;
the qualified save itself is not used to initialize this continuation.

The source's next menu observation **47,815** is a proven new update **47,800**,
with no inherited held keys. Its later controls retain every key edge and source
PAL timestamp. The native prefix input ends at logical source position 47,595;
the combined input ends at 75,342. These are replay-source ordinals, not native
update counts. Declared menu, `C10C08` context and `C10D8A` flight-start events
bind the segments. The preceding native menu boundary is the already-counted
update **47,349**, frame **75,601**; flight traces omit that menu-only boundary,
so its witness is the independently verified prefix's complete replay counters.
There is no state search, fitted offset, synthetic progress, reference RAM or
clock substitution.

The new native replay consumes **20,413 raw key edges** plus all **7,079 frozen
host input events**, completes **74,858 native replay updates** over **133,402
frames**, and returns to mode zero, `C0FCB4`, with zero queued input and zero
postflight resets. Actual qualification, grade vector and completion count agree
with the original. Pilot naming/date/history bytes remain separately reported.
CPU and chipset emulation are both absent from this native run.

The comparison covers original observations **61,797 through 75,571**, aligned
to native updates **61,094 through 74,856**. It retains all **13,763 real update
identities** and **12 duplicate observations** in that interval, including
formation, combat, approach, carrier arrest and postflight. The two following
original menu observations have no flight-trace rows; actual final RAM and
runner counters verify the native menu return separately. All 16 record cores
and all declared gameplay fields match at every compared boundary.

Complete eight-plane page sets match at **8,393** observations and differ at
**5,382**. The first page difference is observation **61,957**, game tick 161.
No complete combined timer/HUD field set matches; that diagnostic includes
absolute clock samples as well as refresh/message state. These captures have
no drawing-band hashes, so they do not establish which screen regions contain
every difference. The full differences are retained without masks. Gameplay
parity passes under the existing separation of clock-driven presentation from
flight state; acceptance of the HUD's different cadence still needs the agreed
equivalent-state/event assessment. Original sound onset/handoff acceptance also
remains open, as do broader independent mission comparisons.

Two earlier setup attempts remain rejected. Loading only the genuine earned
pilot into a cold native start leaves the original message path at `C1075A`:
`CONTEXT_REQUEST` is zero and `MESSAGE_BUFFER_SPACE` is zero, while the verified
preceding-flight menu has buffer byte `FF`. `start_menu_outcome` and the source
message handler's `MC_ACCEPT_TYPED_CODE` branch account for the different wait.
The source's later Return belongs after the context event, so this cold replay
cannot advance without changing its setup/input contract. Its budget failure,
partial trace and actual short setup snapshot remain retained. No game rule was
patched to bypass the wait.

The first warm attempt consumed all controls and earned the third completion,
but a validation assertion incorrectly required one additional native menu
update. Its logs remain retained, without claiming a complete retained flight.
The corrected replay retains captures before validation assertions, and the
completed comparison rechecks the entire prefix. Ten earned-prefix/menu-boundary
guards, two missing/wrong menu-witness guards and three core/control/page
sensitivity guards pass. Reassessment also preserves the preceding complete
escort comparison exactly, including its rejected gameplay outcome.

The connected playable path remains `port/native/main.c ->
native_frontend_tick -> native_flight_tick`; this batch changes validation and
evidence only. No playable dependency is removed. The production executable
remains SHA-256
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.
Campaign continuity stays waived and named-state cleanup remains outside this
goal. Compressed captures, rejected cases, input and reports remain under
`build/native-flight/`; the [checkpoint](figures/native_mission_five_full_flight_checkpoint.json)
binds their hashes and the source/validation code. Passing raw captures are
removed and the build pruner remains enabled.

```powershell
python tools/native/check_original_update_entries.py --source-evidence build/native-flight/original-mission-five-touchdown-approach --out build/native-flight/original-mission-five-touchdown-updates --snapshot-observation 74641 --assess-existing
python tools/native/check_recorded_original_mission_trace.py --runner build/native/fa18_native.exe --source-evidence build/native-flight/original-mission-five-touchdown-approach --source-updates build/native-flight/original-mission-five-touchdown-updates --event-anchors --native-prefix build/native-flight/recorded-escort-ground-carry --out build/native-flight/independent-mission-five-warm-prefix-v2 --assess-existing
python tools/native/test_native_earned_prefix.py --prefix build/native-flight/recorded-escort-ground-carry --runner build/native/fa18_native.exe --comparison build/native-flight/independent-mission-five-warm-prefix-v2 --out build/native-flight/mission-five-comparison-guards/native-prefix.json
```
