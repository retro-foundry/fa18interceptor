# Exec interrupt vectors, server chains and hardware/software interrupts

The pinned Kickstart 1.3 implementation is the behavioral authority; the local
Amiga SDK is reference material only. `port/amiga/exec_interrupt_services.c`
contains reusable guest-state/bus semantics, without SDK, CPU, ROM addresses,
game resources or captured RAM dependencies. The original ABI identifiers and
68000 timing are isolated in `port/os/exec_interrupt_adapter.c`.

The reference runner checks three complete source signatures before activation.
`--no-os-irq-services` retains the original ROM implementation. Clean profiles
can enable the proven service registry entries without performing a signature
read. Instruction and extension timing operations never read ROM bytes.

| Source interval | Implemented scope | Phases |
| --- | --- | ---: |
| FC0C88-FC0E9A | Seven hardware interrupt roots, register save/restore, vector callbacks, audio rescan and RTE | 138 |
| FC11CA-FC1296 | SetIntVector, AddIntServer, RemIntServer | 54 |
| FC1338-FC1426 | Server-chain dispatch, Cause and deferred software-interrupt dispatch | 65 |

Register contexts preserve the original 68000 ordering, including low-word-first
predecrement stores. Memory-to-memory phases fetch destination extensions after
source reads. Fixed-address context restore keeps the initial address while
loading A1/A5, even when A1 itself is the source base. Interrupt returns yield
to the outer execution context, as scheduler RTE/STOP phases already do.

Server callbacks run in guest code on the existing machine timeline. A clear
Z flag claims the interrupt and stops the chain. The chain's saved clear mask
is acknowledged after callbacks. Cause coalesces node type 11, groups signed
priorities into five FIFO queues and requests the software interrupt. Dispatch
marks each node type 2 before calling it and restarts at the highest priority
after each callback, permitting requeueing. The audio root scans bits
8, 10, 7, 9 and rereads pending enabled audio bits after each callback.

Preserve the pinned RemIntServer boundary behavior: D2 is copied after the
interrupt number has been multiplied by 12. Empty-chain removal therefore
writes the low word containing bit `(number * 12) & 31`, rather than correcting
it to the requested interrupt number. This has an independent whole-call
assertion against the original. Bit 14's reserved request branch is retained
and exercised as a focused fixture; it is not a normal chipset interrupt.

## Original evidence captured before implementation

`analysis/data/romfree_exec_interrupt_contracts.json` pins entry/exit registers,
full SR, Chip/Slow RAM and savestate hashes, instruction traces and ordered
hardware windows from the original Engine9000 DLL. Binary captures stay under
ignored `build/amiga`. Runtime initialization never consumes captured RAM.

| Original capture | Instructions | Engine OCS clocks | Native ROM/C OCS clocks |
| --- | ---: | ---: | ---: |
| SetIntVector | 14 | 132 | 130 /130 |
| AddIntServer | 49 | 348 | 288 /288 |
| Software interrupt with no pending queue bit | 4 | 32 | 32 /32 |
| Complete level-three IRQ to interrupt exit | 554 | 3,624 | Nested service closure pending |
| Server-chain dispatch | 539 | 3,504 | Nested service closure pending |

The first three real captures replay to their original registers, full SR and
full RAM checkpoints. Native ROM and C have identical ordered accesses and
cycles. These replays disable the native bus timing model; the existing native
ROM-to-Engine cycle differences of 2 and 60 OCS clocks are retained, not
introduced by the replacements. No claim of matching these Engine cycle
totals is made.

The two longer captured IRQ paths also execute graphics FC6D48-FC6DB4, device
paths FE5824-FE5AAC and FE935A-FE9514, plus the already implemented potgo path.
Their full captured evidence is hash-checked, but they cannot yet run to the
captured checkpoint with ROM cleared because those remaining callbacks need
replacement. Controlled full-call tests use actual guest callbacks, not stand-ins
for those missing OS implementations. Cause was not reached in bounded 300-frame
warm or cold captures; RemIntServer was not reached in the bounded cold capture.
Their source contracts and focused full-call fixtures remain explicitly distinct
from observed game coverage.

## Verification

The focused phase gate proves all 257 new phases in 131,584 CPU/DMA fixtures,
including full CCR, nesting boundaries and branch alternatives. An additional
148 scheduler phases pass in the same run after extracting shared CPU/bus
adapter helpers: 405 PCs /207,360 fixtures total.

`python tools/amiga/check_exec_interrupts.py` proves 19,968 complete CPU/DMA
calls with ROM and expansion-ROM bytes cleared and the strict guard active.
Independent assertions cover install/remove, empty/nonempty chains, stable
priorities, callback claim order, coalescing, five soft queue groups,
priority/FIFO ordering, self and higher-priority requeueing, user/supervisor
returns, all seven IRQ levels, master masking, simultaneous requests and audio
ordering. A hardware level-three IRQ interrupts a software callback using the
CPU's actual exception frame. Registers, full SR, both stack banks, full RAM,
ordered accesses and cycles match; all three forbidden-access counters stay zero.

Memory and scheduler regressions pass 21,568 calls plus 2,048 Alert-vector paths,
and 8,192 scheduler calls with both original captured scheduler checkpoints.
GNU and MSVC Release service comparisons match all three recordings: 36,236
RGB444 frames, final RAM/seals, cycles, PC, iterations and blitter counters.
Proof reports are `build/amiga/exec-interrupt-recordings/full.json` and
`build/amiga/exec-interrupt-recordings-msvc/full.json`. The independent portable
library's four CTests and explicit machine-constructor tests pass both compilers;
the protected native-build check passes all 406 files.
The real STOP hook regression retains 1,024 CPU/DMA fixtures and now checks six
routine-resume boundaries, including both additional hardware IRQ RTE phases.

The combined phase gate passes 1,074 PCs /549,888 CPU/DMA fixtures. The complete
614-row routine/poison gate retains exactly 571,427 shadow /458,087 sandbox
comparisons, all seals and identical poison frames. Logs:
`build/amiga/exec-interrupt-all-phases.log` and
`build/recomp/exec_interrupt_full_gate.log`.
This is component progress. Clean startup, OS boot interrupt-list initialization
at FC1298-FC1337, generic exception handlers, remaining graphics/devices, DOS,
persistence and whole-game zero-ROM acceptance remain pending.
