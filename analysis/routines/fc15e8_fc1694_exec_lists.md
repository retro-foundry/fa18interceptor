# ROM-independent Exec lists

The complete pinned 1.3 list family is implemented as host C:
Insert FC15E8, AddHead FC1614, AddTail FC1624, Remove FC163C,
RemHead FC164A, RemTail FC165A and Enqueue FC1670. The last phase is FC1694.

`port/amiga/exec_lists.c` owns list behavior through explicit guest registers,
CCR and ordered memory callbacks. It requires no SDK, ROM, CPU interpreter or
game assets. `port/os/exec_lists_adapter.c` owns the 1.3 ABI addresses and
68000 bus/cycle phases. The reference runner verifies the source signature
before activation; `--no-os-lists` retains the original validation oracle.

The implementation preserves original sentinel links, Insert's null-anchor and
tail-anchor branches, empty RemHead/RemTail returns, scratch registers, flags,
MOVEM ordering and priority comparison. Enqueue inserts after existing nodes
with equal signed byte priority. No instruction or operand bytes are read from
ROM on the candidate side.

## Original evidence and differential proof

Original unmodified Engine9000 entry/exit contracts were captured before
implementation from the demo recording. Binary snapshots, full registers/SR,
instruction traces and hardware observation windows remain under ignored
`build/amiga/exec-list-*-contract`. Hardware window rows require interval
filtering; they are not presented as an already bounded hardware fixture.

| Service | Original caller return | Instructions | OCS colour clocks |
| --- | --- | ---: | ---: |
| Insert | FE92B8 | 9 | 56 |
| RemHead | FC4AA2 | 4 | 25 |
| Enqueue | FC1ED0 | 14 | 80 |

AddHead, AddTail, Remove and RemTail were not reached within the sampled
300-frame window. Their static contracts come from the pinned original service
bytes and focused full-call fixtures; they are not claimed as live capture
coverage.

`python tools/amiga/check_service_phases.py` now passes 208 PCs /106,496 fixtures
across all converted families. The new list family accounts for 63 PCs /32,256
fixtures. Every phase compares registers, full SR, PC, complete RAM, ordered
guest/hardware accesses, bus microcycles and instruction cycles in CPU-only and
display DMA modes. Candidate ROM/expansion-ROM buffers are cleared and guarded.

`python tools/amiga/check_exec_lists.py` passes 21,504 complete calls and covers
all 63 original list phases. Fixtures include zero through three existing
nodes, insertion at head/first/last/tail, removing first/last/single nodes,
empty removals, all CCR values, positive/negative/equal priority boundaries and
DMA contention. Expected list topology and stable priority order are checked
independently as well as original-versus-C register/SR/RAM/access/cycle parity.
Removing a node from an empty list is not a valid original API call and is
excluded from that operation's fixtures.

GNU and MSVC Release reference builds pass. The portable library builds and
its four existing contract tests pass both compilers; the new neutral source
also compiles with GNU `-Wall -Wextra -Werror`. MSVC C-versus-ROM smoke comparisons
match 300 frames from each recording. The full GNU service comparison matches
all 36,236 sealed frames, RGB444 bytes, complete final RAM, CPU cycles, PC and
blitter counters. The original final RAM/RGB444 hashes remain those recorded
in `romfree_foundations.md`.

The fresh full 614-row routine gate passes 571,427 shadow /458,087 sandbox
comparisons, exact final RAM seals and identical poison frames. Local logs are
`build/recomp/exec_lists_full_gate_verified.log`,
`exec_lists_service_recordings.log`, `exec_lists_msvc_service_smoke.log`, and
`build/amiga/exec_lists_{phases,calls}.log`. The first gate invocation used an
invalid WSL Python shim; it did not complete and is not acceptance evidence.
The completed gate uses Git Bash with the actual Python directory on PATH.

The original cold entry-to-main checkpoint is additionally captured in
`analysis/data/romfree_startup_checkpoint.json`: C0DEB0 to C0E27E, 1,632 original
instructions and 11,677 OCS colour clocks. The trace contains 1,065 unique
Kickstart/expansion-ROM instruction PCs and 74 ROM-boundary transitions,
including interrupts and OS task activity. It covers one Workbench launch;
other startup paths and the full remaining service inventory are unfinished.
Captured RAM remains test evidence only.

Whole-game ROM independence is still unfinished. This batch completes list
semantics, not task scheduling, Wait/Signal, memory allocation, DOS, devices or
clean machine/OS startup.
