# 3. Identify loaded bytes, memory and Hunk relocations

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](02-capture-and-replay.md) · [Next](04-tracing-and-ghidra.md)

An executable's file layout is not its runtime layout. Establish how observed
bytes reached their current addresses before attaching behavior to them.

## Keep address coordinates separate

| Coordinate | Role |
| --- | --- |
| Container/file offset | Location in an extracted or packed file. |
| Hunk index/offset | Location within a CODE/DATA/BSS allocation. |
| Guest runtime address | CPU-visible placement after loading/copying/relocation. |
| Dump offset | Runtime address minus bank start. |
| Synthetic listing address | Navigation coordinate for an unresolved segment. |
| Host address | PC pointer, unrelated to guest addressing. |

For verified payload base B and offset O, guest address is B+O. In a dump
starting at M, its offset is B+O-M. State pointer width, endianness and wrapping.
A DOS segment-list link/header is not the first executable payload byte.

## Inventory the original executable

```console
python -m amiga_decomp --project your-game run parse_hunk local/extracted/program --output analysis/hunk_inventory.json
```

The parser records resident names, allocation sizes/memory flags, CODE/DATA/BSS,
payload offsets, sizes, relocation groups and symbol counts. It supports its
declared block set, including 32/16/8-bit relocation groups, and rejects unknown
blocks or external-reference flattening. It is not a universal linker/loader.
Investigate unsupported formats rather than guessing a flat image.

Overlays, decrunching, runtime copies and patches create additional byte views.
Record source/destination ranges and writer PCs. Final RAM can contain data
where startup code executed; preserve both time windows instead of overwriting
the earlier view in Ghidra.

## Prove a placement

A signature search or segment-list value produces a candidate, not a proof of
an entire segment. Require plausible bounds, unchanged non-relocated bytes and
consistent relocation addends.

```console
python -m amiga_decomp --project your-game run map_runtime_window --executable local/extracted/program --inventory analysis/hunk_inventory.json --ram build/core-window/chip.bin --ram-base 0 --address WINDOW_PC --length 16 --output build/window-map.json
python -m amiga_decomp --project your-game run map_loaded_segment --executable local/extracted/program --inventory analysis/hunk_inventory.json --ram build/core-window/chip.bin --ram-base 0 --segment HUNK_INDEX --runtime-base HUNK_BASE --output build/segment-map.json
```

Use measured bank/Hunk values. The first command locates an exact window in
non-BSS payloads, not a global uniqueness proof. The second reports mismatches
and candidate bases from 32-bit relocation words. Examine its JSON: successful
exit alone does not establish zero mismatches or consistent targets.

```text
loaded_word(O) = disk_addend(O) + runtime_payload_base(T)  (modulo 2^32)
candidate_base(T) = loaded_word(O) - disk_addend(O)        (modulo 2^32)
```

Here O is a relocation offset and T its target Hunk. Target observations must
agree. Relocated bytes are excluded from unchanged-byte comparison but require
their own validation. BSS has no disk payload. Mutated code/data needs a
version-specific placement report.

## Expand evidence-backed seeds

```console
python -m amiga_decomp --project your-game run resolve_hunk_runtime --executable local/extracted/program --inventory analysis/hunk_inventory.json --ram chip=build/core-window/chip.bin --seed HUNK_INDEX=HUNK_BASE --output analysis/hunk_runtime_resolved.json
```

Repeat `--ram region=FILE` and `--seed` as needed. Each dump must match its
manifest size. `--chip`/`--slow` name configured regions, not universal bases.
`--seed-file` accepts JSON `seeds` entries with `segment` and
`runtime_payload_base`.

The resolver propagates consistent 32-bit relocation addends from unchanged
segments and records statuses/conflicts. `bss_unverified`, `outside_exported_ram`
and `mutated_or_unmapped` are not verified placements. Parsed RELOC16/RELOC8
groups are outside this command's 32-bit propagation logic. Incomplete,
inconsistent or mutated groups need a focused investigation.

For a DOS-managed program, audit a known segment chain independently:

```console
python -m amiga_decomp --project your-game run tools/amiga/inspect_segment_list.py --state build/core-window/state.bin --executable local/extracted/program --inventory analysis/hunk_inventory.json --segment-list SEGMENT_LIST_BPTR --output build/segment-chain.json
```

Pass the observed DOS BPTR, which the tool multiplies by four, rather than the
already-expanded byte address. Its
savestate reader covers the documented CRAM/BRAM A500 view and rejects
compressed required chunks; it does not read every RAM expansion.

## Disassemble without overstating knowledge

Set manifest `executable`, `hunk_runtime_map`, `disassembly_synthetic_base`:

```console
python -m amiga_decomp --project your-game run disasm_game
python -m amiga_decomp --project your-game run disasm_game HEX_PC 40
```

`HEX_PC` is bare hexadecimal or `$`-prefixed hexadecimal. Listings/index go in
`build/disasm/`; unresolved Hunks receive labelled synthetic bases. Linear
sweeps can decode embedded data. Check map statuses before treating an address
as verified: listing consumption does not promote it. This tool's separate
Hunk reader has a different supported block set from `parse_hunk`.

## Build the state ledger

For each interval/field record address/Hunk coordinate, width, signedness,
alignment, initial value, writers/readers, lifetime and evidence. Include
overlapping pools, scratch buffers, display pages, dispatch slots, hardware
ownership and patched code. Appearance alone does not establish ownership.

For arrays, prove count, stride and slot selection. Before defining a C
structure, establish interpreted offsets, padding and aliases. Mode-dependent
storage may have several meanings. Keep unresolved offsets structural
(`field_0c`) rather than forcing a gameplay name.

Acceptance: later trace addresses correspond to preserved bytes, verified
placements and named time windows. Rejected/unknown bases remain explicit.
