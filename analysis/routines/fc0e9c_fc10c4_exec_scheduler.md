# Exec task dispatch, interrupt exit and task exceptions

The pinned 1.3 source interval FC0E9C–FC10C6 is implemented for the retained
68000 machine. Original Engine9000 observations precede this implementation;
their committed metadata and artifact hashes are in
`analysis/data/romfree_exec_scheduler_contracts.json`. SDK declarations were
reference material only. No SDK headers, libraries or captured RAM initialize
the runtime.

## Behavior and ownership

`port/amiga/exec_scheduler.c` implements ready-list selection, signed priority
comparison, quantum/reschedule state, task identity and nesting, idle/dispatch
counters, saved contexts, and switch/launch/user exception callback preparation
and return. Every operation uses explicit guest registers, packed byte offsets
and bus callbacks. There are no Interceptor addresses, host tasks, ROM contents
or interpreter dependencies in this library.

`exec_context.c` owns ordered 68000 context transfers for both the scheduler and
the existing memory services. Normal-order register masks support explicit base
registers, predecrement save and normal/postincrement restore. Saves write each
long's low word first and retain the initial base-register value when included
in the mask. Restoring through A5 reads every saved register from the original
address even when A5 itself is restored. CCR is untouched.

`port/os/exec_scheduler_adapter.c` owns the 148 pinned ABI/timing phases,
virtual program fetches, guest calls and CPU SR/USP/RTE/STOP operations. The
memory-to-memory MOVE bus ordering fetches the destination extension after its
source data read. Reference activation verifies FNV-1a 97882438 over the complete
source interval. `--no-os-scheduler` selects the ROM oracle. A clean profile can
activate the proven service directly without reading any ROM signature.

The interpreter now accepts an explicit instruction-hook yield. Service STOP
returns to the embedding machine without fetching the stopped continuation.
This also works when lowering the interrupt mask immediately accepts an IRQ and
clears CPU_STOPPED: the IRQ handler's opcode belongs to the following slice.
The existing machine owns idle line advancement and interrupt wakeup. The resume
loop gives STOP the same ownership instead of continuing virtual service phases.
Routine-scoped resumes also yield at scheduler RTE/STOP phases so the outer
dispatcher owns context changes; this preserves the reference shadow checker
and its original comparison totals.

68010/68020 attention-flag conditionals retain their individual transfers, but
extended exception/FPU contexts are outside this 68000 profile. They do not gain
an invented implementation; unsupported targets remain subject to the guard.

## Proof

- `python tools/amiga/check_service_phases.py`: all 817 service PCs /418,304
  CPU/DMA fixtures pass registers, full SR, both stack banks, stopped state,
  complete RAM, ordered data/hardware accesses and cycles with ROM/rtarea cleared
  and all ROM-access/unsupported-service counters zero. The focused scheduler
  proof covers 148 PCs /75,776 fixtures.
- `python tools/amiga/check_exec_scheduler.py`: 8,192 complete CPU/DMA calls
  cover switching to the same/another task, empty/nonempty ready lists, priority
  and quantum decisions, user/supervisor interrupt exits, switch/launch callbacks,
  task exception delivery/return, interrupt nesting and empty exception masks.
  Blocking Wait resumes after a second guest task calls the actual Signal
  service. Blocking WaitPort resumes after actual PutMsg and the deferred
  reschedule service. That controlled guest task invokes Reschedule after
  PutMsg releases its interrupt protection; it does not substitute for the
  still-missing soft-interrupt/Cause implementation.
- The same checker verifies the committed capture hashes and replays both real
  Engine9000 entries. Switch reaches FC0FF0 in 45 instructions with identical
  registers/full SR/all RAM and 425 OCS clocks on both the native ROM and C paths,
  matching Engine9000. Interrupt exit reaches FC0EC0 in eight instructions with
  identical registers/full SR/all RAM; both native paths take 73 OCS clocks
  with bus timing disabled, versus the captured Engine9000 82. This nine-clock
  difference exists in the retained reference configuration and is not introduced
  by the C replacement. Native ROM/C ordered accesses, cycles and stack banks
  match. Both captures stop before RTE; complete returns are additionally proved
  by the controlled call fixtures.
- `python tools/amiga/check_exec_stop.py`: 1,024 calls through the real
  `m68k_execute` instruction hook prove empty-queue idle STOP and IRQ acceptance
  during STOP. CPU registers, full SR, stack banks, stopped state, full RAM,
  ordered accesses and cycles match the ROM. A poisoned IRQ opcode verifies
  that this slice yields before fetching it. Four routine-resume tests additionally
  verify context boundaries without guest instruction execution or ROM reads.
  Candidate ROM/rtarea buffers are
  zero and all forbidden-access/unsupported counters remain zero.
- The memory context-helper regression retains 21,568 complete calls and 2,048
  original Alert-vector paths. GNU/MSVC Release compatibility builds and all
  four portable contracts pass. Clean machine constructor tests pass GNU and
  MSVC Release. The protected native-build guard passes 406 files.
- GNU and MSVC Release C-service/ROM-service replays match all 36,236 sealed
  frames, every RGB444 byte, full final RAM, cycles, final PC, iterations and
  blitter counters. Reports are `build/amiga/exec-scheduler-recordings/full.json`
  and `build/amiga/exec-scheduler-recordings-msvc/full.json`.

The full 614-row integration gate passes 571,427 shadow /458,087 sandbox
comparisons, all final RAM seals and identical poison frames. Log:
`build/recomp/exec_scheduler_full_gate.log`.

## Remaining dependencies

This implements task switching, not a clean game launch. Soft interrupts/Cause,
interrupt registration and handlers, generic exception/trap/Alert handling,
memory entry lists and installed allocation wrappers still need complete
replacements. Graphics helpers/LoadView, devices, DOS persistence, asset-derived
OS initialization, startup and exit remain. No `fa18_romfree` executable is
delivered and whole-game zero-ROM acceptance is still pending.
