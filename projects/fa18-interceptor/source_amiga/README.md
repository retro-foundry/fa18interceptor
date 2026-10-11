# F/A-18 source reconstruction

`observed/` holds bounded assembly slices linked to original runtime addresses.
They are distinct from the full-bank scaffolds `chip_ram.asm` and
`slow_ram.asm`, which include game, OS, data and unused bytes from the preserved
baseline. Exact assembly establishes bytes; interpreted routine contracts in
`analysis/routines/` establish separately supported behavior.

The general method, arithmetic/calling-context requirements and coverage limits
are in [source reconstruction](../../../docs/decompilation/05-source-reconstruction.md).
The source count and coverage percentages in older project notes are historical;
regenerate a measurement from its declared snapshot rather than copying them.

## Compare the observed slices

From the framework root, with `vasmm68k_mot` on PATH and both original baseline
banks present:

```console
python -m amiga_decomp --project fa18-interceptor run verify_reconstructions --snapshot captures/uae/baseline_menu --no-coverage
```

This assembles every observed slice and fails on any mismatch with the specified
RAM. For coverage as well, omit `--no-coverage` and write a fresh derived report
with `--write-summary build/source-coverage.json`. That report separates byte
unions, overlaps, runtime-backed/static-only slices and bounded data estimates;
none is a whole-game completion percentage. Overlays or changed code require
their own matching authority snapshot.

## Rebuild the separate full-bank scaffolds

From this project directory:

```console
vasmm68k_mot -m68000 -Fbin -o build/chip-rebuilt.bin source_amiga/chip_ram.asm
vasmm68k_mot -m68000 -Fbin -o build/slow-rebuilt.bin source_amiga/slow_ram.asm
```

Each scaffold includes its corresponding `captures/uae/baseline_menu/*.bin`
verbatim. Rebuilding it verifies that raw-bank inclusion, not semantic
reconstruction of every included byte. Generated `pcode/raw/*/observed.asm.txt`
is structural evidence and must retain its capture coordinates and limits.
