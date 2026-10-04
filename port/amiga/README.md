# Reusable Amiga compatibility foundations

This is host C11 code. The Amiga SDK is reference material only: this library
does not include SDK headers, link SDK libraries, or invoke an Amiga compiler.
It builds independently of Interceptor, Musashi, SDL, ROMs and savestates.

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
- `rom_audit.h`: optional reference observations of nested flow, CPU state,
  accesses and machine time. Its bounded table fails closed if exhausted. This
  is an inventory, not ordered entry/exit fixtures or complete game coverage.

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

This library is a foundation, not a complete AmigaOS replacement. Interceptor
still needs Exec, graphics, device and DOS service work before `fa18_romfree`
can run the whole game without Kickstart.
