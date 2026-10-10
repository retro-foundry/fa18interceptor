# Escort comparison: update identities and rejected origin ? 2026-10-10

All **44,458 original observations** map to **44,455 actual update calls**
and executed LINK instructions. The probe preserves every original trace,
final RAM/register byte and consumed input. It retains three actual pre-LINK
resumptions, and all **2,028 consumed key edges** keep their order and PAL
columns. Four caller/LINK/stack/stop corruption controls reject their changes.
The original escort recording is still a failed route ending at player phase
FE, with no escort grade earned.

The fresh native attempt enlists a real level-zero pilot, replays qualification
and the complete first mission, and earns its actual grade/completion. It then
replays the original escort inputs at absolute update indices. This comparison
is **rejected before escort body assessment**: first C10D8A/tick-one entry is
original observation 40,643/actual update **40,641**, versus native update
**40,395**. The global input schedule is therefore 246 updates late relative
to native flight initialization. No timing offset is applied or accepted.

The earliest stage-count difference is already in the timed briefing path:
C10418 begins at source 36,453/native 36,451, but C10458 follows at source
36,722/native 36,496. Later preview and setup paths also have different update
counts. The first equivalent flight-start observations also retain explicit
core differences in slots 0, 4, 8 and 14, plus observer/pan state. Those states
are not normalized or seeded to make them match. The evidence identifies an
invalid shared global input origin; it does not yet assess all initialization
rules or attribute each initial-state difference to presentation timing.

Native subsequently reaches C11788 and one postflight reset, finishing at
player phase four rather than source FE. This is retained as a failed replay
attempt, not a confirmed gameplay defect: the declared input origins already
fail the gate. The checker stores the full native trace, RAM, statistics,
actual saved-pilot context and initialization rejection before reporting the
failure. It does not continue with a fitted alignment or claim whole-flight
parity. The original/native earned grades, completion count, qualification,
saved level and scene level match; final player phase differs.

The playable path remains `port/native/main.c -> native_frontend_tick ->
native_flight_tick`. No runtime behavior, clocks, RNG, assets or allocation
policy changes. The diagnostic `check_recorded_original_mission_trace.py`
supports mode four and preserves failed captures/rejections. Its default mode
three assessment remains byte-identical over all 4,967 observations and its
three NPC/control/page mutations still reject. Original storage never seeds
native gameplay.

The next comparison must establish input ownership at declared mission events
through each independently run briefing, then assess the actual independent
initialization states under the original rules. Flight controls should begin
at the real flight event and follow explicit source JSR/LINK identities within
that flight. Shared global update counts across differently paced menus are
insufficient. No source/native RAM matching or offset search may select the
origin.

Complete compressed original probe and native capture are retained under
`build/native-flight/escort-failure-update-entries` and
`build/native-flight/independent-escort-failure-retained`. The first native
attempt retains its log separately; the later attempt retains the full trace
and RAM. Captures remain bounded by the normal 512 MiB diagnostic budget and
4 GiB build pruner. The committed
[checkpoint](figures/native_escort_initialization_review_checkpoint.json)
binds every retained artifact and the full stage/core review.

Successful escort coverage, its complete gameplay/drawing assessment, broader
flight coverage and recorded original/native sound timing remain open.
