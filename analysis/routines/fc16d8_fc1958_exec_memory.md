# Exec memory allocation and free lists

Authority: pinned 1.3 FC16D8–FC1958 and unmodified Engine9000 contracts in
`analysis/data/romfree_exec_memory_contracts.json`. SDK declarations are
reference only; the code neither includes nor links them.

## Implemented services

`port/amiga/exec_memory.c` implements resumable semantic phases for Allocate,
Deallocate, core AllocMem, FreeMem, AllocAbs, TypeOfMem and AvailMem. All sizes,
addresses, headers and chunks are guest quantities. The API uses explicit
register/CCR state and byte/word/long bus callbacks, with no interpreter,
host allocation, ROM, savestate or SDK dependency. Verified offsets live in
`abi_13.h`; the embedding game supplies its OS structures and memory banks.

`port/os/exec_memory_adapter.c` preserves the pinned ABI, virtual instruction/
extension phases, stack effects and guest calls. Permit and Alert transfers
remain on the machine timeline. Reference activation checks hashes over the
complete source intervals. Clean profiles can activate the registry directly,
without any ROM bytes. `--no-os-memory` selects the ROM validation oracle.

The original uses eight-byte rounding and first-fit chunks. Allocate splits
or removes a chunk and subtracts the rounded size from the header. Deallocate
aligns the address/extent, inserts in address order, merges adjacent left and
right chunks, and restores the free total. Overlap/invalid-region paths enter
the original Alert vector. AllocMem filters header attributes, forbids tasks
around allocation, and clears the original number of longwords when requested.
AllocAbs retains leading/trailing fragments around a requested guest address.
AvailMem returns aggregate free bytes or the largest qualifying chunk; TypeOfMem
checks lower-inclusive/upper-exclusive header bounds.

Original partial register widths and complete condition flags remain visible.
MOVEM saves write each low word first; register restores do not change flags.
The clearing shift's timing is explicit. DBRA does not fetch its extension on
exhaustion, and the clear loop continues through the original high-word path.
Original wrapping-size behavior is retained. Zero-size AllocAbs at a chunk
start self-links that free chunk; its controlled contract asserts this source
quirk rather than inventing an empty-allocation result or repair.

## Proof

- Focused memory phase proof: 237 PCs /121,344 CPU/DMA fixtures. Full phase
  gate: 669 PCs /342,528 fixtures. Registers, complete SR, full RAM, ordered
  accesses and cycles match with ROM/rtarea cleared and the strict guard active.
- `python tools/amiga/check_exec_memory.py`: 21,568 complete CPU/DMA calls
  over all CCR values, six list/region configurations and eight request/flag
  boundaries. Independent assertions cover accounting/topology, first-fit
  results, attribute selection, region boundaries, splitting/coalescing,
  allocation failure, zero/wrapping sizes and clearing. A 65,537-longword clear
  crosses the original 16-bit loop boundary. Another 2,048 invalid-region,
  duplicate-free and overlapping-free paths match through the Alert vector;
  they do not claim an Alert return. All candidate ROM counters stay zero.
- Original Engine9000 completed contracts cover Allocate (34 instructions /
  207 OCS colour clocks), Deallocate (50 /384), core AllocMem (110 /628),
  FreeMem (77 /611), AvailMem (41 /235) and the installed AllocMem wrapper.
  Full register/SR, RAM hashes, traces and hardware-window hashes are recorded.
  Hardware windows need interval filtering. TypeOfMem was not reached during
  the 300-frame cold-start observation; TypeOfMem/AllocAbs have static contracts
  and controlled original-versus-C fixtures.
- GNU and MSVC Release library/runtime builds pass; all four portable
  compatibility contracts and the MSVC clean-machine constructor pass. The
  protected native target remains clean over all 406 files.
- GNU and MSVC Release C-service versus ROM-service replays match all 36,236
  sealed frames, each RGB444 byte, full final RAM/seals, cycles, PC, iterations
  and blitter counters. Reports: `build/amiga/exec-memory-recordings/full.json`
  and its `-msvc` counterpart. These compare replacements on the same machine
  configuration; they do not resolve inherited Engine9000 timing differences.
- Fresh full 614-row routine gate with memory services enabled passes
  571,427 shadow /458,087 sandbox comparisons, all three final RAM seals and
  identical poison frames. Replacement parity introduces no timing difference.

## Remaining dependencies

The cold public AllocMem vector points to C06550, an installed RAM stack wrapper
through C06598 and private vector base C06522. It enters ROM retry/scavenging
code FE491E before calling core FC17D0. That wrapper and retry service are
captured/inventoried, not implemented here. No captured RAM or wrapper code is
used to initialize the clean runtime. AllocEntry/FreeEntry and complete Alert
behavior also remain. This is a verified core memory batch, not whole-game
ROM independence or a complete OS bootstrap.

The full objective still requires task/interrupt/exception dispatch, graphics
and device dependencies, DOS persistence, evidence-backed OS initialization,
startup/shutdown, `fa18_romfree`, all mode recordings and zero-ROM acceptance.
