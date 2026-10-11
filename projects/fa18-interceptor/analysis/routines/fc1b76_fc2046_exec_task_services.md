# Exec messaging, signals and task protection

The authority is the pinned 1.3 implementation, FC1B76–FC1C58 and
FC1E54–FC2046, plus original Engine9000 entry/exit observations in
`analysis/data/romfree_exec_task_contracts.json`. The SDK is reference only.

## Implemented behavior

`port/amiga/exec_task_services.c` contains reusable semantic phases for PutMsg,
ReplyMsg, WaitPort, SetSignal, SetExcept, Signal, Wait, signal/trap allocation,
Forbid, Permit and Reschedule. Guest data is reached through explicit bus
callbacks; no host structure packing, SDK headers, interpreter types or ROM
addresses enter that layer. `abi_13.h` defines verified byte offsets.

`port/os/exec_task_services_adapter.c` supplies the original ABI identifiers,
instruction/extension bus costs, register/SR integration and guest transfers.
Nested calls preserve the original vector and stack conventions. CPU state
and the guest PC retain resumable continuations between phases and IRQ events.
The dispatcher uses five disjoint intervals, leaving GetMsg and scheduler
callbacks under their own owners. The reference runner checks signatures at
activation; the runtime phases never fetch ROM instructions or operands.

Notable original details retained include byte-sized interrupt/task nesting,
full condition flags on wraparound, low-word-first predecrement writes,
dynamic bit-operation timing, SNE's true-result cost and DBRA's absent
extension read on exhaustion. Signal removes a waiting task and enqueues it
by signed priority before requesting a reschedule. SetExcept returns the old
exception mask; SetSignal returns the old received mask. Allocation searches
from the highest available bit, reports -1 on exhaustion and preserves the
original partial register widths and signal-mask clearing behavior.

## Validation

- `python tools/amiga/check_service_phases.py`: all 417 currently implemented
  PCs /213,504 CPU and display-DMA fixtures match original registers, full SR,
  RAM, ordered memory/hardware accesses and cycles. This batch adds 209 PCs.
- `python tools/amiga/check_exec_task_services.py`: 43,008 complete nonblocking
  calls, every CCR, six structural modes and eight bit/mask boundaries. Checks
  include queue topology, reply types, all port actions, controlled callback
  ordering, pending Wait return/mask consumption, task wakeups, allocation
  exhaustion and deferred rescheduling. Candidate ROM and expansion-ROM bytes
  are cleared; reads, instruction fetches and unsupported counters stay zero.
- GNU and MSVC Release compatibility builds and all four portable contracts
  pass. The MSVC reference runtime and clean-machine constructor test pass.
- C-service versus ROM-service configurations match all 36,236 frames in the
  three sealed recordings on GNU and MSVC Release. Every RGB444 byte, final
  full RAM/seal, CPU cycles, PC, iterations and blitter counters match.
  Reports: `build/amiga/exec-task-recordings/full.json` and
  `build/amiga/exec-task-recordings-msvc/full.json`.
- `python scripts/check_native_build.py` passes the 406-file protected target.
- Fresh full `scripts/recomp_ports_check.sh` passes all 614 routine rows,
  571,427 shadow /458,087 sandbox comparisons, the three final RAM seals and
  identical poison frames. No replacement timing difference was introduced.

## Explicit remaining dependencies

The semantic Wait loop and its original Supervisor/Switch vector calls exist;
complete blocking task switching does not. Permit and immediate Reschedule
also need Supervisor and its dispatch callback. Real Cause/soft-interrupt
behavior remains absent. Controlled RAM callbacks prove the message action
calling convention, not those underlying services. No success is fabricated
for a blocked call. The real blocking Wait trace reaches idle STOP FC0F90
and has no captured return within 100,000 instructions.

This is one verified service batch, not a clean ROM-free game launch.
Exec memory/scheduler/interrupt/exception services, graphics helpers/LoadView,
devices, DOS persistence, original startup/shutdown and `fa18_romfree` remain.
Whole-recording ordered interrupt/audio timelines and all game modes still
need their acceptance proofs. Inherited machine timing differences remain
separate from replacement parity. Captured RAM is test evidence only.
