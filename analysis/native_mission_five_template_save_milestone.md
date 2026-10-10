# Native template-save ownership — 2026-10-10

All **13,763 actual native Mission Five bodies** now match original
instructions, including update **63,713**, which previously failed at the
placement-cache result `C4F6DE`. Its actual entry and body-begin RAM are
unchanged; its complete body-end RAM changes only the two result bytes,
from `7786` to the original `FFFF`. All rendered bytes match. The preceding
rejected sweep and exact failed fixture remain retained.

## Connected ownership fix

The playable path is `port/native/main.c -> native_frontend_tick ->
native_flight_tick -> refresh_native_context -> refresh_child ->
refresh_template_placements_observed -> template_sort_factor ->
native_model_retain_result`. The change removes reliance on an earlier sort's
retained word when a template expansion produces the original replacement.

Original `C1D3F4` saves D2–D5/A0–A5. In the ordinary `C0EFD4` caller frame,
saved D5's upper word overlaps the later model result. The first incoming D5
comes from `C1C5D8`'s published projection-origin Z. In independent views it
comes from the final signed matrix-row product at `C2E5EE`, preserved through
the word-only cosine operations and projection save/restore. Source matrix
and zoom bounds preserve that product's sign in the stored final coefficient.
Later template cells replace D5 with their actual signed translation at
`C1DCF6/C1DCFE`, which survives into the next expansion.

The native observer now publishes that incoming upper word at expansion
begin, using local C state. It introduces no gameplay allocation, captured
original RAM, clock offset, fitted timing, result mask or new random behavior.
Broad state migration remains outside this goal.

The exact original producer trace retains 2,347 lines. Logging enabled and
disabled produce identical failure outputs, instruction counts and timer
samples on the preceding failed fixture. D5 begins `12CE9679`; subsequent
cell translations leave `0000C000`, producing the zero later normalized to
`FFFF`. This is observed source ownership, rather than a replacement value
chosen from the failed result.

Startup/menu callers have a different stack frame and keep their existing
sort-result owner. An unscoped implementation was rejected by the actual menu
comparison: source retained `012C`, native `0000`, with zero sort lists and
otherwise identical compared RAM. Its executable, source fixture and rejection
remain retained. The scoped implementation passes the affected original
startup/menu comparisons and independent-view bodies.

## Complete playable regression and original capture

The current executable independently enlists a pilot and replays ordinary
qualification, patrol and escort controls. All **27,176 prefix trace rows**,
complete final RAM, counters and saved pilot exactly preserve the preceding
accepted recording. Gear is raised after takeoff and lowered before landing;
the prefix earns both earlier completions and returns to the menu without a
reset. It does not initialize gameplay from captured RAM or invented progress.

The subsequent Mission Five replay preserves its entire previous native
trace, final RAM, counters, event plan and earned pilot. All **13,775 flight
observations**, **220,400 record cores** and camera/control fields still match
the independently started original. Grade one, the third completion and menu
return remain exact. Optional message logging preserves all 75,573 original
and 45,570 native observations. The complete timing assessment reproduces the
prior accepted result, including nine rejection controls and the ALT/HDG
sampled-second explanation.

The complete native body capture has **41,289 full-MiB identities**, retained
as 124,362,700 delta bytes instead of 43,294,654,464 separate raw bytes.
Every body executes the external original-instruction oracle under the
existing explicit scratch, voice, busy-state and ABI contract, using its
actual timer interval. Placement-cache and rendered bytes remain compared.
The report seals the actual oracle executable and source hashes and checks
that they remain unchanged throughout the sweep.

The independent original full drawing capture also completes: **117,099
snapshots** and **37,899 drawing-owner returns** across observations
61,797–75,571. All 75,573 complete recording rows, consumed controls, final RAM
and runtime counters remain exact. This diagnostic explicitly uses 1 GiB and
1,800 seconds after the retained 900-second failure; defaults remain
512 MiB/900 seconds. Its compressed delta and register evidence occupy
88,504,617 bytes. Capturing owners does not itself accept the complete
independent cockpit history.

## Validation and remaining work

The scoped implementation passes six affected Release CTests, including
cleanup, covering startup/menu sorting, record expiry, complete bodies,
independent-view depth and models. Debug expiry and cleanup pass. The final
comment-only rebuild is bound by the complete current executable replays and
13,763-body sweep. Eight actual continuation mutations and nine message
mutations are rejected; ten cockpit byte/role/identity guards pass.

The native capture reports zero project gameplay heap violations, SDL gameplay
pool requests and pool failures. Normal 4 GiB pruning hooks remain enabled.
One preceding replay was rejected because concurrent cleanup deleted its
unprotected live trace; that failure remains documented. Replay workspaces
now use the pruner's existing `ram-` protection, and the actual protected
rerun passes complete trace/RAM equality. Compressed reference and rejected
evidence remain retained; passing temporary raw captures are removed.

The production executable SHA-256 is
`544bb244eb048b65436382a755cbf9bd9abd29f1337aec849967ef157fac3bd8`.
The [checkpoint](figures/native_mission_five_template_save_checkpoint.json)
binds actual reports, producer sources, the exact two-byte fix, complete
original capture, rejected implementations and memory evidence.

Complete independent cockpit drawing history, broader independent flights
and complete original sound onset/handoff/filtering remain open. Campaign
continuity remains waived and named-state cleanup remains separate.

```powershell
python tools/native/check_mission_frame_delta.py --runner build/native/fa18_native.exe --reference build/native-flight/mission-five-template-carry-message-trace --source-evidence build/native-flight/original-mission-five-touchdown-approach --source-updates build/native-flight/original-mission-five-touchdown-updates --native-prefix build/native-flight/recorded-escort-template-carry-protected --replay-evidence build/native-flight/independent-mission-five-template-carry --out build/native-flight/mission-five-template-carry-complete-bodies-new
```

A new output directory preserves the accepted sweep and preceding rejected one.
