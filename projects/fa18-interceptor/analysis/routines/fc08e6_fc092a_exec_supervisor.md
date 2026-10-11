# Exec Supervisor and privilege-frame continuation

Authority: pinned 1.3 Supervisor FC08E6–FC08F4, privilege handler
FC090E–FC092A, and Permit callback FC1FBE–FC1FC6. Original Engine9000
metadata is in `analysis/data/romfree_exec_supervisor_entry.json`; the SDK is
reference only. Captured RAM stays ignored and never initializes the runtime.

## Behavior

Supervisor called in supervisor mode builds the original callback return and
saved-status frame. Called in user mode, its attempted status change raises a
68000 privilege exception. The handler recognizes the Supervisor call site,
rewrites the saved return PC and transfers through A5. The callback's RTE
restores the caller's status and stack bank; the service then returns through
the original caller stack. The Permit callback tests the saved supervisor bit
and returns or transfers into the Dispatch vector as the original does.

Frame comparisons, rewrites, saved-status tests and frame pushes are reusable
semantic operations in `port/amiga/exec_task_services.c`. The fixed 1.3
addresses, bus phases and CPU integration are in `port/os/exec_supervisor.c`.
Privilege exceptions and RTE use the retained CPU's verified frame operations;
the service never fetches a Kickstart opcode or operand. Normal frame writes
retain their original ordering and exception construction reads the guest
low-memory vector. Other privilege-fault paths remain separate dependencies.

The reference runner checks the pinned signatures before activation and accepts
`--no-os-supervisor` to retain the ROM implementation. Clean-start profiles can
enable the three disjoint service registrations without inspecting a ROM.

## Proof

- All 432 registered PCs /221,184 CPU/DMA phase fixtures match full registers,
  SR, RAM, ordered accesses and machine cycles. Supervisor adds 15 PCs; the
  fixtures exercise both user and supervisor entry and recognized/unrecognized
  frame comparisons. Candidate ROM/rtarea buffers are zero and guarded.
- `python tools/amiga/check_exec_supervisor.py`: 3,072 complete calls cover
  user/supervisor entry, nested callbacks, both stack banks and supervisor-mode
  Permit/Reschedule callbacks. Original callbacks increment D0, return with
  RTE and preserve the caller's complete status and A5. CPU/DMA cycles and every
  ordered access match with zero ROM reads/fetches/unsupported services.
- Original Engine9000 cold-entry observation: user SR=0010, four instructions
  /47 OCS colour clocks from FC08E6 to FC092A, through exception construction
  and frame rewrite. The original real callback enters Switch; its complete
  return was not captured within the bound. This checkpoint is not a complete
  blocking-call proof.
- GNU and MSVC Release builds pass; four portable compatibility contracts and
  the MSVC clean-machine constructor contract pass. The native protected build
  guard still passes all 406 files.
- GNU and MSVC Release full C-service versus ROM-service comparisons pass all
  36,236 sealed frames, every RGB444 byte, final RAM seals, CPU cycles, PC,
  iterations and blitter counters. Local reports are under
  `build/amiga/exec-supervisor-recordings` and its `-msvc` counterpart.
- Fresh full 614-row routine gate passes 571,427 shadow /458,087 sandbox
  comparisons, all three RAM seals and identical poison frames with Supervisor
  enabled. Replacements introduce no difference from the matching ROM setup.

## Remaining work

Switch, Dispatch, soft-interrupt Cause and complete blocking Wait are not
implemented by this batch. User-mode Permit/Reschedule can enter Dispatch and
therefore do not yet have complete ROM-free returns. The recognized 68010 call
site comparison is retained, but the supported runtime is still a 68000; no
68010 startup or stack-frame support is claimed. Unknown privilege-handler
branches reach the strict unsupported-target guard in a ROM-free profile.

This checkpoint does not deliver `fa18_romfree`. Kernel memory and interrupt/
exception dispatch, graphics/device/DOS services, persistence, clean OS
initialization, startup and shutdown remain part of the active objective.
