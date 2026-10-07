# Reusable Amiga compatibility foundations

This is host C11 code. The Amiga SDK is reference material only: this library
does not include SDK headers, link SDK libraries, or invoke an Amiga compiler.
It builds independently of Interceptor, Musashi, SDL, ROMs and savestates.

`host_keys.c` owns physical Amiga raw-key identities for character/SDL2 host
symbols and SDL1/libretro replay symbols. Both native and reference runners
consume this shared mapper; game text translation remains a separate owner.

```sh
cmake -S port/amiga -B build/amiga-compat
cmake --build build/amiga-compat --config Release
ctest --test-dir build/amiga-compat -C Release --output-on-failure
```

`amiga_compat` currently exports:

- `ofs.h`: read-only OFS files, raw directory/metadata records, case-insensitive
  ASCII paths and ranges clipped at EOF. Checksums, invalid data chains and hash
  cycles fail. DOS return values, error codes, ExNext ordering, writable files
  and service cycles belong in a separately validated DOS adapter. The legacy
  disk API remains in `../disk.c`.
- `sha256.h`: portable SHA-256 identities for host bytes, with empty, short,
  padding-boundary and million-byte fixtures. Game-version policy lives in
  `port/romfree/media.c`; neither assets nor game checksums live in this library.

The host file overlay accepts optional profile-owned read-only path prefixes.
Those resources read directly from OFS and reject writes/truncation with error
223. With no prefixes configured the normal writable overlay behavior remains;
game-specific path policy belongs to the embedding profile.
- `guest_memory.h`: explicit 24-bit guest banks and big-endian stores. Host
  structures never stand in for packed Amiga structures.
- `hunk.h` and `hunk_loader.h`: parse CODE/DATA/BSS and RELOC32, preserve
  CHIP/FAST requirements, install DOS segment headers, zero BSS and relocate
  original disk bytes. The caller supplies placement. Unsupported Hunk records
  fail; malformed layouts leave memory unchanged. `../hunk.c` retains legacy
  native-port aliases alongside the neutral API.
- `runtime_guard.h`: configurable forbidden ROM/expansion-ROM ranges, separate
  read/fetch/unsupported-service counters, and fault context. CPU/bus adapters
  must check before fetching data and terminate when a check fails. Virtual
  service bus phases consume timing without reading memory.
- `service_dispatch.h`: a neutral phase registry used before opcode fetch.
  Callbacks use the embedding timeline and guest continuations; the registry
  checks ambiguous registrations and supplies fault context. Nested calls and
  interrupts can run between phases. Unknown phases remain explicit failures
  under the runtime guard.
- `abi_13.h`: a small set of verified guest ABI offsets, without SDK includes
  or host struct packing assumptions. SDK revisions after 1.3 are not assumed
  to describe the pinned kernel.
- `exec_lists.h`: resumable Insert, AddHead, AddTail, Remove, RemHead, RemTail
  and Enqueue semantics. Explicit guest register/CCR state and bus callbacks
  preserve sentinel links, insertion after equal priorities and access order.
  ABI addresses and CPU timing stay in the embedding machine adapter.
- `exec_task_services.h`: resumable message queues, signals, signal/trap bit
  allocation, interrupt/task nesting and reschedule requests. Explicit 68000
  register/CCR state and byte/word/long bus callbacks retain access ordering.
  The caller supplies semantic phases and their verified field/value arguments;
  this layer has no ROM entry addresses or interpreter dependency. Guest calls,
  Supervisor, scheduler continuations, IRQ delivery and timing remain adapter
  responsibilities. Verified Supervisor exception-frame comparisons/rewrites
  and saved-status tests also live here; CPU privilege/stack-bank changes and
  RTE remain in the CPU adapter. Blocking Wait and WaitPort need a scheduler.
- `exec_memory.h`: resumable Allocate/Deallocate, core AllocMem/FreeMem,
  AllocAbs, TypeOfMem and AvailMem over guest MemHeader/MemChunk lists. Eight-byte
  alignment, first-fit splitting, sorted coalescing, attribute filtering and
  original clear-loop behavior are preserved. The caller supplies guest OS
  structures, memory banks, timing and Permit/Alert continuations. This never
  allocates from a host heap. Original boundary behavior, including zero-size
  AllocAbs self-linking at a chunk start, is retained by the verified ABI.
- `exec_context.h` and `exec_scheduler.h`: ordered 68000 register contexts,
  ready-list task selection, priorities, reschedule/quantum state, task identity,
  nesting and switch/launch/exception callback state. Guest bus callbacks and
  registers are explicit; the embedding CPU owns SR/USP/RTE/STOP and the machine
  owns interrupt delivery and idle time. Memory services share the context
  transfer implementation. No host scheduler or captured OS state is used.
- `rom_audit.h`: optional reference observations of nested flow, CPU state,
  accesses and machine time. Its bounded table fails closed if exhausted. This
  is an inventory, not ordered entry/exit fixtures or complete game coverage.
- `exec_interrupt_services.h`: resumable vector installation, server chains,
  Cause coalescing and priority/FIFO software-interrupt queues. Guest callback
  pairs, interrupt acknowledge/enable writes, claimed-server ordering and
  callback requeueing are explicit. The CPU adapter owns seven interrupt-level
  roots, privilege changes, nesting and exception-frame returns. Timing and
  callback execution use the embedding machine timeline. The pinned empty-chain
  removal behavior is preserved, including its multiplied interrupt-bit index.

For another game, supply its disk resources, segment placements, OS ABI profile,
CPU/bus adapter, service contracts and validation fixtures. Interceptor's
addresses and checkpoint metadata live outside this library. The resumable
service phases currently live in `../os` and use the existing machine timeline;
their CPU/bus adapter still uses Interceptor names. Future service families
should keep neutral state and operations here, with machine adapters outside.

Oracle tooling under `../../tools/amiga` can inspect a DOS segment chain, check
disk-derived Hunk installation, capture an Engine9000 power-on entry, and compare
C service phases with original instructions. Captured RAM/ROM belongs only in
ignored test evidence. No captured RAM is used to initialize this library.

This library implements the services needed so far, not all of AmigaOS.
Interceptor now reaches visible clean startup and its demo. Further service
coverage and game-level persistence/exit checks remain before whole-game
ROM independence is accepted.
- `host_compat.h`: behavior-level host allocation outside caller-reserved
  regions, synthetic library vectors, ADF-backed file handles and a writable
  save overlay, locks/current directories, metadata and merged directory
  enumeration, keyboard events and matrix state. Guest structures remain
  packed. Files retain short reads, seek positions and original byte formats.
  Host bookkeeping and I/O replace Amiga task/packet execution. Device timing,
  signal delivery and CPU entry/return conventions belong to the adapter.
- `host_graphics.h`: construct intermediate CopLists and Chip-RAM OCS lists
  from guest ViewPort/BitMap/ColorMap structures; patch palette moves and free
  owned lists. The embedding chipset installs them for vertical blank. This
  supports the game's current display paths, with exact 1.3 layout/timing,
  interlace, complex viewport merging and extended modes deferred.
