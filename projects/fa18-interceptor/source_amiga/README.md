# Source reconstruction

No semantic assembly conversion is claimed yet. The initial byte-exact bank
scaffold can be rebuilt from the preserved runtime snapshot using:

```powershell
vasmm68k_mot -m68000 -Fbin -o build/chip-rebuilt.bin source_amiga/chip_ram.asm
vasmm68k_mot -m68000 -Fbin -o build/slow-rebuilt.bin source_amiga/slow_ram.asm
```

These are memory-bank scaffolds including game, OS, data and unused bytes, not
reconstructed game modules. Replace only bounded, evidenced regions with
instructions/constants as their contracts become known. Keep the raw snapshot
and compare assembled bytes after each conversion. Generated observed assembly
in `pcode/raw/menu/observed.asm.txt` is structural scaffolding too.
