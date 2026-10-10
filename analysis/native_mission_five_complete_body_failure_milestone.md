# Complete mission-five native bodies — 2026-10-10

The complete successful native mission-five recording now has an original
instruction comparison for **all 13,763 actual bodies**. **13,762 match**;
update **63,713** remains rejected. It differs in the placement-cache result
word at `C4F6DE`: original instructions produce `FFFF`, native retains `7786`.
Every rendered page matches, including this rejected body. This exposes a
missing internal producer; it does not accept the full body or cockpit gate.

## Connected capture and ordinary prefix

The playable path is `port/native/main.c -> native_frontend_tick ->
native_flight_tick`. The diagnostic now replays the actual ordinary
qualification/patrol/escort prefix, followed by the original mission-five
controls and source-defined context/flight event anchors. Its helper verifies
the earned prefix's complete trace, actual RAM and saved pilot, reconstructs
every later control from executed source update identities, and validates the
retained event plan and exact preceding menu boundary. No captured original
RAM or constructed pilot progress initializes the native game.

The resulting game preserves every baseline runtime counter, complete final
RAM, earned grade, third completion and saved pilot. It consumes all 20,413 raw
key edges and 7,079 frozen host events, reaches the menu in 133,402 frames and
74,858 replay updates, with zero resets or queued input. The body interval is
native updates 61,094–74,856, corresponding to original observations
61,797–75,571 with all 12 duplicate dispatch observations retained in the
mapping. Capture reports zero project gameplay heap violations, SDL gameplay
pool requests and pool failures.

Every body supplies the external original-instruction oracle with its actual
input, begin/end RAM and existing API timer interval. The existing explicit
scratch, voice, busy-state and ABI contract remains unchanged. No rendered
page or placement-cache result byte is excluded. The 41,289 complete MiB
snapshots occupy **124,362,700 delta bytes**, instead of 43,294,654,464 separate
raw bytes. The complete stream and all three failing snapshots are retained
compressed.

## Actual producer trace

The failed body's begin/end ticks are 98,500/98,502 and its saved game tick is
2,620. Original `C1D974` first writes retained words `6186`, `7586`, then
`7786` during record normalization. Original `C1D3F4` subsequently saves
`D2–D5/A0–A5` on its caller stack. The high word of saved D5 overlaps the later
model's retained word at synthetic-oracle address `C7FE78`.

The first template save has D5 `12CE9679`, overwriting the word with `12CE`.
Later template saves have D5 `0000C000`, overwriting it with zero. The aircraft
model's early path reaches `C1F074 -> C1F8DE` with that zero; `C1CFBA` publishes
the normalized result `FFFF` to `C4F6DE`. Native still publishes `7786`.
Source `TEMPLATE_CELL` assigns the template translation to D5; the producer
and repeated-call ownership need completion before a gameplay fix is accepted.

The original failure reproduces twice using the retained exact fixture and
timer interval. Both complete stdout/stderr traces and the separate trace
oracle are retained. Its optional template-save logging only observes
registers and adds no emulated instruction, state store or timer sample.
The preceding native executable is also retained compressed for a future
regression rejection check.

## Capture failures and checks

The bounded original replay captures observations 64,416-64,423: all 72
actual entry/body/drawing snapshots and 24 owner returns match their caller
contracts. A changed return stack is rejected. The entire 75,573-observation
replay, consumed controls, final RAM and runtime counters remain exact. Its
494,619,840-byte trace and 1,060,772-byte delta fit the default 512 MiB budget;
compressed snapshots/registers occupy 450,132 bytes. This is a bounded
producer investigation, not a complete cockpit-history acceptance.

An independent original full drawing capture used an explicit 1 GiB diagnostic
budget because its existing full trace alone occupies about 472 MiB. It hit
the unchanged 900-second process limit. The old temporary wrapper removed
the partial data; this attempt is recorded as rejected, with its last verified
live observation 71,050/frame 171,178. That is progress observed before the
timeout, not a final boundary or accepted original replay.

The original capture wrapper now retains interrupted delta, register, trace,
final RAM and consumed-control files compressed before temporary cleanup.
A genuine one-second original-process rejection preserves all available
bytes and decoded hashes, including 13,664,256 partial trace bytes. It creates
no successful report; its empty delta/register files and incomplete execution
remain rejected. Two failure-retention tests also pass. The process limit
and capture limit remain explicit options with defaults 900 seconds/512 MiB.

The cache pruner's dry run exposed live diagnostic workspaces and their
repeatedly executed oracle as deletion candidates. The pruner now protects
the two existing temporary capture directory prefixes and their associated
active oracle. Temporary cleanup makes them disposable after the comparison;
ordinary obsolete captures and binaries remain eligible. Four artifact tests
pass, with the existing Windows symlink check skipped. The normal 4 GiB hooks
remain enabled, and cleanup runs after direct original captures.

Eight real earned-continuation controls reject changed runner/trace/RAM,
incomplete comparison, unverified prefix, changed origin, controls or anchor
input, including rehashed control claims. Ten cockpit byte/role/identity
guards and seven delta-decoder checks pass. No native gameplay code is changed.
The unchanged native executable is
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.

The [checkpoint](figures/native_mission_five_complete_body_failure_checkpoint.json)
binds the complete sweep, rejected fixtures, actual traces, retention control
and tools. The earlier complete-flight gameplay/message assessment remains
valid in its declared scope. Missing template publication, full cockpit
history, broader independent flights and complete sound matching remain open.
Campaign continuity remains waived and named-state cleanup remains separate.

```powershell
python tools/native/check_mission_frame_delta.py --runner build/native/fa18_native.exe --reference build/native-flight/mission-five-message-trace --source-evidence build/native-flight/original-mission-five-touchdown-approach --source-updates build/native-flight/original-mission-five-touchdown-updates --native-prefix build/native-flight/recorded-escort-ground-carry --replay-evidence build/native-flight/independent-mission-five-warm-prefix-v2 --out build/native-flight/mission-five-complete-body-delta-new
python tools/native/test_native_continuation.py --prefix build/native-flight/recorded-escort-ground-carry --replay build/native-flight/independent-mission-five-warm-prefix-v2 --runner build/native/fa18_native.exe --source-evidence build/native-flight/original-mission-five-touchdown-approach --source-updates build/native-flight/original-mission-five-touchdown-updates --trace-evidence build/native-flight/mission-five-message-trace --out build/native-flight/mission-five-complete-body-delta/continuation-guards.json
```

The first command currently fails the body gate at update 63,713; a new output
directory preserves the preceding rejected sweep.
