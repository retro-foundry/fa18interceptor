# `$C3180C` postflight record scan

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation reaches the entry and terminal return; the scan body
is otherwise unobserved.

`source_amiga/observed/scan_postflight_records.asm` reproduces
`$C3180C-$C318F5` (234 bytes). When `$C457B9` is set, it scans fixed 16-byte
records at `$C4E2BC`, derives a record offset from a masked word, filters
record flags and a byte at `$62` of `$C46184`-relative data, and writes a
selected index/result/slot set. The empty case writes a zero slot and `-1`
current index.

The scan's gameplay role and the external fallback at `$C06C02` are unproven;
the report records only the directly decoded control and data flow.
