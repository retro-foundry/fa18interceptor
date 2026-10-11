# Complete independent escort comparison — 2026-10-10

The successful original escort recording now has an exact update-call proof
and an actual arrest snapshot. The current native runner executes the entire
independent replay, consumes every key edge and returns to the menu without
a reset, but **does not earn the escort grade**. Whole-flight parity remains
unaccepted. This is a completed comparison with retained differences, not a
successful native escort qualification.

The original probe reproduces all **47,814 observations**, complete final
RAM/register bytes and consumed controls exactly. Explicit `C15DA2` JSR
calls and executed `C0EFD4` LINK instructions identify **47,799 updates**;
all **15 duplicate observations** remain represented. The mapping preserves
the order and source PAL timestamps of all **6,973 key edges**. Four existing
caller/LINK/stack/resumption guards reject altered evidence.

The new optional `--snapshot-observation` reads one complete pre-input RAM
export from the original recorder. At first arrest, observation **46,885**,
all **16 complete record cores**, **42 named fields** and **eight drawing
planes** match the complete trace. The one-MiB RAM snapshot is retained
losslessly compressed with SHA-256
`0992bb2afa0a237a32bc5452b13653bd27b8e9f448a5a5602aba4eb115d497a9`.
Reassessment passes without rerunning the original; requesting a different
observation and changing the snapshot's player X byte both reject. The
default probe behavior remains unchanged.

The live snapshot confirms carrier slot 14 at `(1136640, 0, 1071104)`, scale
three, with actual arrestor vertices `(1136584, 112, 1070684)`,
`(1136704, 112, 1070520)` and `(1136560, 112, 1070548)`. Player contact is
`C082`; the second cached probe is `[32, 0, -184]` before scaling. Unlike
final menu RAM, this snapshot retains the live carrier. These bytes support
the preceding successful original landing rather than reconstructing geometry
from a menu that has already cleared the carrier.

The connected native path is `port/native/main.c -> native_frontend_tick ->
native_flight_tick`, driven by the existing declared replay events. Ordinary
enlistment, qualification and the complete preceding mission earn the native
pilot's progress; no eligibility, grades, coordinates, clocks or gameplay RAM
are copied from the original. This batch changes only validation capture and
evidence. No playable dependency is removed, and the native executable remains
SHA-256 `36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.

Declared menu, `C10C08` context and `C10D8A` flight-start events bind the input
segments. Native completes **47,553 replay updates** over **80,484 frames**,
consumes every key edge and has zero queued input, zero postflight resets and
no CPU/chipset emulation. The native final screen is menu, mode zero,
`C0FCB4`. That screen/counter check passes, while earned-pilot checks fail:
original completions are two and escort grade is one; native completions are
one and escort grade is zero.

The complete flight comparison covers original observations **40,643–47,812**:
**7,170 observations**, **7,157 actual update identities** and all **13**
duplicate observations in that interval. It compares all 114,720 record cores,
every named field and all eight drawing planes at each boundary. No complete
record boundary, complete page set, camera/control field set or timer/HUD field
set matches. There are 66,038 individually matching cores, including inactive
slots; that count does not establish complete gameplay parity. All three
record/control/page sensitivity guards reject. Full differences remain in the
retained report, without masks, state searches or fitted alignment.

The first genuine difference is already present at flight initialization.
At actual `C0FECE` entry the stored microsecond samples are **976,083** in the
original and **600,000** in this native headless PAL diagnostic. The existing
source placement rule selects different carrier positions: native's carrier
and starting aircraft are **16,384 X units east** of the original. These are
observed inputs and coordinates. Earlier source-instruction evidence explains
this rule; default visible gameplay now uses the actual host clock. This
fixed-key diagnostic therefore starts from different worlds. It does not
attribute every later difference to initialization alone, establish a gameplay
bug, or justify changing original rules to force this recording to pass.

At the mapped original arrest observation the native aircraft is instead at
`(1190968.171875, 2.9296875, 1289743.140625)`, contact `8080`, away from its
carrier. It later stops there and earns no escort grade. This outcome and the
strict comparison remain rejected under the agreed timing policy. Independent
whole-flight acceptance requires further evidence at equivalent state/events;
original sound onset/handoff timing also remains open. Campaign continuity
stays waived, and named-state cleanup stays outside this goal.

Complete compressed captures, reports, logs and input remain in
`build/native-flight/original-escort-wire-updates` and
`build/native-flight/independent-escort-wire-input`. The pruner completes;
no passing raw RAM or duplicate source recordings remain. The
[checkpoint](figures/native_escort_wire_full_flight_checkpoint.json) retains
the exact update/snapshot results, actual aligned positions, constructor clock
observations, native rejection, guard results and artifact/source hashes.

```powershell
python tools/native/check_original_update_entries.py --source-evidence build/native-flight/original-escort-wire-target --out build/native-flight/original-escort-wire-updates --snapshot-observation 46885
python tools/native/check_original_update_entries.py --source-evidence build/native-flight/original-escort-wire-target --out build/native-flight/original-escort-wire-updates --snapshot-observation 46885 --assess-existing
python tools/native/check_recorded_original_mission_trace.py --runner build/native/fa18_native.exe --source-evidence build/native-flight/original-escort-wire-target --source-updates build/native-flight/original-escort-wire-updates --event-anchors --out build/native-flight/independent-escort-wire-input
```

The final command intentionally exits nonzero on the retained earned-pilot
and state differences. Its complete recording and assessment are preserved.
