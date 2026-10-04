# Exact compatibility work deferred

The user changed priority on 2026-10-04 to a looser compatibility layer and a
faster, smoother playable game. This supersedes the requirement to prove every
new service instruction phase before implementing the next startup dependency.
Original gameplay, assets and save formats remain the authority. The SDK is
reference material only; reusable services stay in `port/amiga`, CPU/chipset
adapters in `port/os`, and the Interceptor launch profile in `port/romfree`.

Return to the original exact plan after playable launch, menus, flight,
postflight, save/load and exit work:

- Compare service entry/exit registers, full SR and ordered hardware writes.
- Replace coarse service execution charges with original bus phases and cycles.
- Revisit interrupt entry points, nesting, simultaneous delivery, blocking
  requests, timer/input/audio completion and callback ordering.
- Compare graphics View/Copper installation and blitter contention to the
  original, including differences in OS-owned memory layout.
- Compare DOS errors, metadata/directory iteration, short reads and persistence
  at established game checkpoints.
  Existing-file (1005) handles must remain writable: the original C1643A log
  writer uses that mode. Current writes copy ADF bytes into the host overlay
  lazily, preserving file position and length. Revisit protection bits, disk
  capacity accounting, concurrent handles and packet-level failures later.
- Reconcile explicit process/library/device initialization against cold-entry
  evidence. Captured RAM must remain oracle input only.
- Replay all original recordings and add clean-launch recordings covering every
  mode. Keep inherited machine timing differences separate from service changes.

Existing exact services cover lists, messages/signals, Supervisor, memory,
scheduling, interrupts and several graphics/resource operations. Their committed
fixtures and the ROM-backed executable remain available. Passing behavior-level
compatibility checks must not be described as exact timing or full-frame parity.

Concrete concessions in the first host compatibility batch:

- Allocation metadata and file/lock handles live on the host, outside guest
  MemHeader/task/packet machinery. Save bytes stay in original formats.
- Libraries use synthetic vector targets and freshly packed minimum headers.
  Library availability covers known game dependencies; unknown names return
  the normal failure result, unknown operations terminate with context.
- New service returns use a coarse 16-cycle charge; waits use the existing
  beam/blitter services or pending requests. Host file I/O costs no disk cycles.
- A single guest process waits without captured idle-task state. Keyboard and
  timer completions are polled on the chipset line timeline. Exact device
  task scheduling, input-handler chains and additional gameport/audio commands need work.
  Gameport button edges now complete READ_EVENT with packed codes 68/E8 as
  consumed by original C16EAE. Trigger masks and timestamps are preserved;
  completion occurs on the next chipset line, without original device task
  scheduling. Motion delta thresholds, timeout events, controller-type event
  differences and close/ownership lifecycle still need reference evidence.
- Graphics builds its own CopList/cprlist layouts. LoadView publishes for the
  next vertical blank, retaining original guest palette and bitmap resources.
  Complex merging, interlace, clipping and extended modes are unproved.
- Clean frame boundaries yield before the next opcode fetch. The reference
  runner retains its historical boundary convention. Callback-call detection
  uses the supplied IR under the ROM-free guard instead of reading OS bytes.

GNU/MSVC isolated ADF-only startup, credits and keyboard-to-demo rendering
smoke passes with all three forbidden-access counters zero. Portable component
save round trips pass. Game UI reset/save and nonzero qualification-failure
logs now reload all 78 bytes on GNU/MSVC. Complete mode/guest-exit acceptance
remains pending. This is compatibility progress, not a measured speedup.

Functional acceptance now also covers all four currently available mission
entries with changing active flight, and a full first-flight/menu/second-flight
restart sequence on GNU/MSVC. Higher modes 7/8 require evidence-backed saved
progression; direct F5/F6 selection remains gated by the original availability
bytes in `indexed_commands.c`. Do not remove that gameplay gate to gain coverage.
A cold Chip-memory starvation probe returns zero from allocations and later
faults at an invalid PC. It does not prove graceful game teardown, and its
source/oracle cause is still unproved. Existing neutral failure-return contracts
remain valid; whole-game startup-error cleanup needs focused investigation.
The optional `FA18_HOST_TRACE` now reports guest SP, return address and full SR,
plus allocation results, using checked host RAM access without emulated bus
transactions. The ignored `build/amiga/oom_probe.c` reserves available Chip RAM
before original startup; `oom-probe.trace` shows AllocMem returning zero via
C53B42 followed by InitBitMap at A0=0 (return C53F80) and zero plane allocations.
This narrows the investigation but does not establish whether the original
caller or compatibility semantics cause the subsequent invalid-PC fault.
Do not treat this probe as a passing teardown test. Revisit failure cleanup
after the playable/performance work; do not spend exact-service effort on it
as a prerequisite for faster scene production.

The first clean launch found 42 captured OS RAM translations among the 624
generated entries. ROM-free code ownership now excludes those functions,
including wrappers inlined into generated routines. Only original non-BSS Hunk
payloads can supply translation spans; dynamic/self-modified code can use the
interpreter. This separation is required by both compatibility strategies.
