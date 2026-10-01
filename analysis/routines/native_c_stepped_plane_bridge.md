# Resumable active-plane C bridge

Updated 2026-10-01. Source: `$C2FD8C-$C2FF44`, the original 119-instruction
plane submission routine, and the three sealed native recordings.

## Implementation

`port/game/active_planes.c` retains the readable game operation.
`port/game/glue/glue_active_planes_step.c` additionally implements each source
instruction in C while the generated callers and chipset still share a CPU
clock. It uses no opcode handler to execute the plane body. Bus accesses,
extension fetches, condition codes, branch costs, stack writes and RTS follow
the source. Custom writes also update the native register cache.

`FA18Port.step` and `step_end` opt a bridge into resumable dispatch. Entry
retains the caller's return PC and stack pointer and yields before the first
instruction, so an overdue caller JSR is serviced first. The dispatch loops
resume one step after servicing chipset work. A source child, interrupt or
frame end can suspend the bridge; its PC and source stack retain the position.
The innermost matching call owns a resumed PC. Returning to the saved caller
PC/SP releases that continuation. There is no fixed cycle charge for steps.

In sandboxed proof, the bridge steps synchronously and its children use the
existing source dispatcher with chipset deadlines held off. Ordinary ports
keep their existing whole-call behavior. This mechanism has only been applied
to active planes; map and region observation hooks remain non-resumable.

The readable plane core also now zero-extends its signed maximum busy count.
The source's `CLR.W/SWAP/MOVE.W` clears the high word even when the chosen
signed word is negative. The previous core sign-extended that word.

## Independent instruction proof

```powershell
python tools/recomp/check_active_planes_step.py --cases 64
```

The oracle reads the original bytes from sealed demo state and independently
executes each opcode through Musashi, then evaluates the C step on the same
fixture. All 119 instructions and 7,616 cases match all 16 registers, full SR,
PC, instruction cycles and RAM, including source stack writes. Fixtures cover
all 32 CCR combinations, word boundaries and randomized high register halves.
Chipset writes are held and DMA contention is off in this structural proof;
live replay supplies the separate bus/event validation below.

Both oracle launchers reuse the headless build's exact source list. On Windows
they select Git Bash explicitly; system32/bash would invoke WSL and corrupt
the MinGW script's shell variables. The existing region oracle was rerun for
512 cases and passed after extracting the shared build helper.

## Live replay proof

For these tests only, the registry had the following additional row:

```c
{0xC2FD8C, glue_C2FD8C, "submit_active_planes", 0, 0,
 glue_C2FD8C_step, 0xC2FF46},
```

Each complete replay used `--ports on --ports-only C2FD8C`, `--to-end`,
the recording's state/input and `local/system/kick13.rom`. Every RGB444 byte
matches the native shadow reference; the RAM/register artifact matches the
recording's sealed final SHA-256. No other C port was enabled in these ON runs.

| Recording | Frames | RGB differences | Sealed RAM | Isolated sandbox matches |
| --- | ---: | ---: | --- | ---: |
| demo01 | 20,833 | 0 | matches | 2,078 |
| qual_carrier_success | 12,353 | 0 | matches | 3,367 |
| qual_fail_crashes | 3,050 | 0 | matches | 351 |

The successful-carrier sandbox run has one incomplete source call, reported
explicitly and not counted as a match. All 5,796 completed isolated sandbox
comparisons have zero mismatches. Sandbox execution itself holds custom
writes and does not preserve live frame endpoints; its final frames are not
an acceptance artifact.

The first-600-frame live instruction/event CSV is byte-for-byte identical to
the original CSV in `native_c_call_boundary_timing.md`: 18,259,468 bytes,
SHA-256 `0b3c674901cb02870d0609a03afa300eff4930b5915bec88ff47bdd41dd22656`.
It includes 22 completed calls, 20 frame crossings and seven observed
interrupts. An isolated fallback log contains no PC inside the plane body.
The source and ON runs both end this window at 85,261,216 cycles and 4,815
blits.

Ignored ON artifacts under `build/recomp/`:

| RGB444 stream | SHA-256 |
| --- | --- |
| active_planes_step_on_demo01.bin | 729f2ac3adae40e10aa4b87a31a40faa9c11836de16b9c3d1610e49215e37d07 |
| active_planes_step_on_qual_carrier_success.bin | 8a8978bd6943d4cc8af26d8b4f7fda48dbf291cd3f1beca514c6f2317bfc0e0b |
| active_planes_step_on_qual_fail_crashes.bin | 9c64d09e6b5155733ab59ec9398174c2511aa747263637757be12f727a95884d |

## Registration and remaining proof issue

The candidate remains **inactive**. The temporary 414-entry full gate stopped
after demo shadow with the same five busy-counter discrepancies as the older
whole-call bridge. The native half holds custom writes, so later DMACONR
reads cannot observe blits that the live source half started. This compares
different inputs despite the identical live source/ON evidence above.
`active_planes_step_gate_414.log` retains all five failures. No comparison was
suppressed, no hardware classification was changed, and the registry row was
removed. The full gate passed again for the actual 413-entry set: 727,968
shadow matches, 1,214,836 sandbox matches, zero mismatches, sealed RAM
unchanged and identical poison frames. GNU and MSVC Release builds pass.
Its reports
describe that set, not the candidate. Resolve the proof's live-input model
before registering this routine under the current acceptance policy.

## Earlier all-native baseline failure

A separate 413-entry `--ports on --frames 400` demo replay first differs at
one-based frame **255** (RGB byte 41,631,392, zero-based frame 254). This is
earlier than the plane candidate's old failure at frame 297. Therefore the
registered shadow/sandbox proof does not establish all-native frame fidelity.

Binary partitioning the enabled registry reproduces a failure with only
`$C501E0` (`set_voice_output`) enabled: first difference at one-based frame
256. This identifies an independently failing registered entry, not the sole
cause of the all-native failure. Its current 110-cycle whole-call charge
omits source bus/instruction boundaries in audio interrupt work. Inspect the
source and compare boundaries before changing it; `update_voices` `$C50158`
is another registered parent and can bypass the leaf bridge.

Ignored evidence: `baseline_on_400.*`, `baseline_bisection_400.json`, and
`baseline_bisect_00` through `baseline_bisect_12` logs/RGB streams. Reproduce
the isolated case with `--ports on --ports-only C501E0 --frames 400` and compare
the first 400 reference frames. Other registered routines may also need timing
work. Completion still requires the remaining game C, native backend and
deferred OS services.
