# Complete postflight mode scheduler (2026-10-03)

`port/game/postflight_scheduler.c` recreates the complete C09E06 dispatcher,
its eight mode handlers and C0A12E/C0A3EA helpers. Ten entries are newly
registered; the existing C0A2F0 now shares the domain and uses source timing.
Internal C0A3A6 completion and C0A3C6 readiness tails remain parts of their
owners. They are not additional registered functions.

## Original source and behavior

`analysis/data/postflight_scheduler_source_scope.json` seals 359 unique
original instructions, including 16 shared boundaries. The audit follows
every static branch and jump to its return, while preserving calls as child
boundaries. It checks the generated source scopes against the sealed original
bytes. Reproduce it with `python tools/recomp/audit_postflight_scheduler.py`.

C09E06 clears its three input latches, calls C230B0, checks status bit 6 and
the recorder byte, then dispatches signed modes 3/4/5/6/7/9/125 or the other
mode route. The handlers retain their source-owned record predicates, counter
changes, view saving, indexed context setup and shared phase publication.
C0A12E reads the original five-word parameter table and sign extends the
fifth word into its long store. C0A3EA returns the source's player-readiness
predicate. The shared continuation calls that real helper at C0A3D6 and uses
the returned zero condition, retaining its original child save/return bytes.

The two pair/target distance paths wrap their long subtraction, but BGE
uses the signed unwrapped difference. Later CMP.L tests the wrapped result.
The C0A364 completion also retains the incoming event word's high byte;
unlike the other completion routes it does not run MOVEQ #0 before C0A3A6.
The context view byte is VIEW_SIDE (C458B2), distinct from VIEW_MODE (C457A7).
The initial structural probe caught and corrected that distinction.

CPU adaptation and resumable source CPU/bus/event boundaries live separately
in `port/game/glue/glue_postflight_scheduler*.c`. Domain C contains no CPU
registers or flags. The old `begin_phase_three` API delegates to the same
mode-nine domain, removing a second implementation of that predicate.
No invented gameplay, fixed average timing charges or substitute behavior
were added.

## Independent readable-C proof

`python tools/recomp/check_postflight_scheduler.py` executes complete original
byte owners and their real children, then compares each independent normal C
adapter. All eleven entries pass 16,384 calls each: 180,224 complete calls,
checking every register and high word, PC, full SR and every Chip/Slow RAM
byte, including the stack, without exclusions. Every owned source boundary
is visited. The fixtures separately force the pair restoration and near/far
target paths so random inputs cannot silently leave them uncovered.

Recorded whole-call proof disables the timing steps in a temporary registry.
The complete dispatcher passes 4,086 shadow and 4,086 sandbox comparisons
across the three recordings. The independently enabled C0A2F0 passes 1,919
shadow and 1,919 sandbox comparisons. There are no hardware, incomplete or
mismatching classifications for these active entries.

The other nine entries have zero calls even with the dispatcher restored to
source. The generic whole-call tool correctly rejects those groups with
`C09E98: no completed comparisons`; both rejection logs and raw reports remain
under `build/recomp/postflight_scheduler_*whole_call.log` and
`build/recomp/whole_call_*.json`. No gate or caller mask was relaxed. Their
proof is the complete original-byte CPU/RAM oracle; live recordings supply
nonregression evidence for these cold entries.

Reproduce active recorded proof with:

```text
python tools/recomp/check_whole_call_glue.py C09E06
python tools/recomp/check_whole_call_glue.py C0A2F0
```

## Timing and integration

The eleven-entry local instruction group passes 359 instructions and 11,488
fixtures with DMA bus contention, comparing registers, full SR, PC, cycles
and RAM. The fresh combined oracle also passes 14,599 instructions / 467,168
DMA cases. There are 246 source-timed entries at this checkpoint. Reproduce
the local group with:

```text
python tools/recomp/check_active_planes_step.py --group postflight_scheduler --bus
```

All 36,236 isolated live RGB444 frames and final RAM seals match fresh source
OFF streams across all three recordings. The full registered gate passes
554,025 shadow / 413,303 sandbox calls, zero mismatches, sealed RAM exact and
poison frames identical. Parents absorb earlier child comparisons, explaining
the changed aggregate totals. GNU and MSVC Release builds pass. Build artifacts
occupy 0.485 GiB after replay cleanup. Full classifications and fixture hashes
are in `analysis/figures/native_postflight_scheduler_checkpoint.json`.

The 600-frame isolated demo probe is exact. ALL still first differs at frame
416 by 361 pixels. The deferred Copper/HUD timing issue remains unchanged.
These OFF/ON checks share the native machine model; they do not establish
independent Amiga timing parity.
