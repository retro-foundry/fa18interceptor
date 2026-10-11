# Deferred static recompilation

2026-10-05 user instruction: statically recompile the remaining functions now,
mark them explicitly, and return to readable decompilation later.

The reconciled seeded inventory is 539 readable translated entries plus
**85 deferred static entries**, covering all 624 existing translation entries.
There are another 75 already-readable original source-only entries. The
handoff's 75 was not a count of remaining translations. None of the new static
entries increases the readable registry or its 543 source-timed proof count.

The authoritative translation bodies come from the existing byte-backed
`recomp_000.c` through `recomp_004.c`. All 624 bodies are identical after
partitioning, including their control flow and instruction comments. The
function table, IDs, labels, spans, call graph and liveness are unchanged.
The deferred subset has 2,723 distinct original instruction boundaries,
5,563 instructions including shared ownership, and 854 constant opcode words.
Its 225 native instruction helpers and three arithmetic timing helpers are
copied exactly from `tools/musashi/m68kops.c`. Their ordered opcode mask
selection independently matches the initialized interpreter table for all
854 bindings.

`port/recomp/generated/recomp_static_deferred.c` contains the compiled code.
Each owner has `STATIC_RECOMP` and `TODO(decompile)` comments. The source uses
direct native helper calls rather than an opcode-table lookup. The original
CPU register file, memory helpers, exceptions and bus/chipset timing remain
dependencies. `FA18_EXEC_USING` shares all fetch, abort/event boundary,
register setup and cycle accounting with opcode-table reference execution.
This introduces no game substitutes or newly invented behavior.

`port/recomp/generated/recomp_deferred.json` is the per-entry return inventory:
address, original spans, static calls, indirect calls/transfers, original CODE
hunk membership, translation hash and follow-up status. The normalized body
hash is stable when an entry moves between the ordinary and static modules.
Forty-four deferred entries begin inside original ADF CODE hunks; forty-one
are reference-runtime wrappers outside them. CODE membership proves byte
provenance, not an independently callable function contract. Existing ROM-free
code-range restrictions still exclude the reference wrappers.

## Reproduce

```powershell
python tools/recomp/static_recomp.py
python tools/recomp/static_recomp.py --check
python scripts/build_recomp.py
python scripts/build_recomp.py --romfree
cmake --build build/recomp-cmake --config Release -j 8
python tools/recomp/check_static_recomp.py
& 'C:/Program Files/Git/bin/bash.exe' scripts/recomp_ports_check.sh
python tools/recomp/check_active_planes_step.py --group all --bus
python tools/amiga/check_romfree_game_paths.py --flight --missions --restart
python tools/amiga/check_romfree_game_paths.py --runner build/recomp-cmake/Release/fa18_romfree.exe --flight --missions --restart
ctest --test-dir build/recomp-cmake -C Release --output-on-failure
python scripts/check_native_build.py
```

The full translator calls the partitioner automatically. Both build drivers
reject stale helper copies or registry classifications. The comparison checker
can build an opcode-table reference by defining `FA18_STATIC_RECOMP_REFERENCE`
only in its separate generated translation module. This is test machinery;
normal builds use direct helpers. `--reference PATH` also accepts an independent
pre-change executable, as used for this checkpoint. The checker hashes every
RGB444 frame and full final RAM, compares full diagnostic CPU/timing state and
checks the recording seal. It removes each scratch frame stream before starting
the next one.

## Validation checkpoint

All gates pass. The committed checkpoint is
`analysis/figures/static_recomp_deferred_checkpoint.json`; raw logs are ignored
under `build/recomp/static_*`.

| Gate | Result |
| --- | --- |
| Structural preservation | All 624 function bodies, table/IDs/spans, graph and liveness unchanged |
| Independent native binding selection | All 854 opcodes match the interpreter's actual table |
| GNU headless and MSVC Release builds | Both runners build; generated-source freshness checks pass |
| Complete replay against the pre-change executable | All 36,236 frames, diagnostic CPU/cycle state, full RAM/register seals and RGB444 streams exact |
| Existing 614-row registered-port gate | 571,427 shadow + 458,087 sandbox matches, zero mismatches; every RAM seal exact and poison frames identical |
| Combined original instruction/DMA oracle | 30,239 boundaries / 967,648 cases match registers, SR, PC, cycles and all RAM |
| GNU/MSVC ROM-free game paths | Original 78-byte save/reload, active free flight, all four available missions and mission/menu/second-mission restart pass; ADF unchanged and zero ROM/fault counters |
| MSVC CTest | 11/11 pass |
| Native build audit | 406 files pass |

The complete reference comparison covers demo01 (20,833 frames),
qual_carrier_success (12,353) and qual_fail_crashes (3,050). Final seals include
Chip/Slow RAM and D0-D7/A0-A7/SR/PC. Cold, hardware and incomplete recording
classifications remain visible; zero recorded calls are not promoted to
whole-owner decompilation proof. No speedup is claimed from these parity gates.

## Return to readable decompilation

1. Start with the per-entry inventory rather than treating 75 already-complete
   source-only entries as new work. The meaningful original game parents include
   C096BC/C096CA, C0F116, C1CFA8, C1ED3C/C1ED4C/C1F2C8/C22AC0 and C2C392.
   Keep shared tails and internal dispatch labels inside their complete owners.
2. Prove original installed/called entry contracts before declaring cold owners
   complete. C500D8 still needs callback installation evidence. C2C392 still
   needs its computed transfer at C2C46E reconciled; static dispatch preserves
   the current original execution path without explaining that producer domain.
3. C1612C retains its existing readable domain/CPU/step implementation and
   frozen-event graphics-wait rejection. Its direct static reference is now
   compiled in the deferred module; it is not promoted to a proved readable row.
4. Preserve all original child effects, partial-write order, register high
   halves, flags, return addresses and instruction/event boundaries. Gather
   complete whole-owner original/controlled-child evidence and isolated live
   frame evidence before registering each readable replacement.
5. Once registered, rerun `static_recomp.py`. The entry leaves the deferred
   module/inventory and returns to its ordinary reference bucket. Never edit
   copied instruction helpers or generated owners directly. Changes to the
   original helper source also require regeneration and comparison.

Whole-original-call-graph coverage, final CPU-free native backend work and
ROM-free outcomes/progression/guest teardown remain separate open work.
The inherited combined readable-C timing difference (frame 424 / 34,144
pixels) and deferred Copper-fade work are retained. This static checkpoint
does not claim those debts have been resolved or that every cold original
function has recording coverage.
