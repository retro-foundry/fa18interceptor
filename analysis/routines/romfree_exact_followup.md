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
- Reconcile explicit process/library/device initialization against cold-entry
  evidence. Captured RAM must remain oracle input only.
- Replay all original recordings and add clean-launch recordings covering every
  mode. Keep inherited machine timing differences separate from service changes.

Existing exact services cover lists, messages/signals, Supervisor, memory,
scheduling, interrupts and several graphics/resource operations. Their committed
fixtures and the ROM-backed executable remain available. Passing behavior-level
compatibility checks must not be described as exact timing or full-frame parity.

The first clean launch found 42 captured OS RAM translations among the 624
generated entries. ROM-free code ownership now excludes those functions,
including wrappers inlined into generated routines. Only original non-BSS Hunk
payloads can supply translation spans; dynamic/self-modified code can use the
interpreter. This separation is required by both compatibility strategies.
