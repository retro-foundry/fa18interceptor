# Grid projection packet activation, 2026-10-01

C279D0 increases the registered count from 418 to 419 of 624 game entries.
The complete C279D0-C27D23 body is readable domain C in
port/game/grid_projection_packet.c. CPU effects are reconstructed through
mathematical observers in glue_grid_projection_packet.c, while
glue_grid_projection_step.c supplies resumable original instruction timing.
The source instruction listing from tools/recomp/port_info.py C279D0 and the
existing c279d0_renderer_packet report are the behavioral authority.

The implementation retains all three tables (C28124, C28368, C2854C), the
source's mode-dependent depth gate, coordinate shifts and aligned origins,
sparse matrix products, negative-kind triangle pointers, single/pair pixel
helpers, and screen-edge clamps. Triangle pairs are written immediately:
rejecting a later vertex retains earlier output pairs. No invented mechanics
or placeholder behavior was added. The older projection_grid.c typed slice
only covers its documented normal-table route; it is not counted separately.

## Proof

GNU headless and MSVC Release builds pass. The 419-entry full gate matched
703,341 shadow calls and 1,129,309 sandbox calls over the three sealed native
recordings, with zero mismatches, identical final RAM hashes and identical
poison frames. Parent comparisons absorb nested calls, so aggregate totals
can decrease as the registered count increases.

| Recording | C279D0 shadow matches | Step sandbox matches | Readable whole-call sandbox matches |
| --- | ---: | ---: | ---: |
| demo01 | 1,536 | 899 | 899 |
| qual_carrier_success | 1,395 | 1,971 | 1,059 |
| qual_fail_crashes | 327 | 9 | 9 |

The separately built whole-call registry disables C279D0's step pointer and
matches 3,258 shadow and 1,967 sandbox calls. Incomplete calls remain explicit;
source-timed steps and whole-call glue have different sandbox timing, so their
sandbox call totals are not interchangeable.

Fresh source OFF and isolated C279D0 ON RGB444 streams match every byte on
all 36,236 frames: demo01 20,833, carrier success 12,353, crash failure 3,050.
The instruction oracle compares 271 instructions and 8,672 fixtures both
without DMA and with varied five-plane DMA phases. All timing groups together
match 2,238 instructions and 71,616 CPU fixtures.

The structural oracle compares all registers and Chip/Slow RAM outside the
dead stack in 4,096 complete source-versus-glue cases. It covers depth gates,
three table choices, signed coordinates, randomized register high halves,
negative record kinds, tilted matrices, word overflow, negative count overflow,
and row clipping.
It explicitly observes 12 partial triangle-write cases and 13 screen-edge
clamp cases. Custom writes are held equally on both sides of these fixtures;
recording checks separately establish actual DMA drawing and live timing.

The fixtures caught the distinction at C27BD6/C27BD8 and C27C98/C27C9A:
BLE follows ADD.W and uses its overflow flag. The domain rejection must test
the signed sum before word wrapping, while later comparisons use wrapped
depth. Testing only the wrapped depth prematurely rejected case 264 and left
D0 incorrect. The corrected C passes all fixtures.

```powershell
python tools/recomp/check_grid_projection.py --cases 4096
python tools/recomp/check_active_planes_step.py --group grid --bus
python tools/recomp/check_active_planes_step.py --group all
python tools/recomp/check_whole_call_glue.py C279D0
$env:PORTS_ONLY='C279D0'
& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh
Remove-Item Env:PORTS_ONLY
& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_ports_check.sh
```

The generic whole-call checker replaces repeated batch-specific verifier
setup. It builds an isolated registry with the shared object cache and runs
independent recordings on three workers; unchanged builds take no compilation.
The existing map/region command remains a wrapper with its four default entries.
Replay scratch streams are removed. The build tree is 0.17 GiB after validation.

## Remaining work

A fresh 500-frame all-registered demo probe still first differs at frame 416
by 361 pixels. Isolated packet fidelity does not establish whole-game ON
fidelity. C1D10C and the indirect-call parents C0F5F8/C1CB14/C1CB26 remain
unregistered. Native backend and deferred OS work retain the handoff order.
