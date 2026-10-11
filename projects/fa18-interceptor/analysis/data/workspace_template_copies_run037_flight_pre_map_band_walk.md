# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run037_flight_pre_map_band_walk_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **40** observed static-entry copies.  **0** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted placement / descriptor `+8` field |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C49110 | not reached before trace end |
| stepped | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C49170 | not reached before trace end |
| stepped | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C49290 | not reached before trace end |
| stepped | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C49296 | not reached before trace end |
| stepped | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C49410 | not reached before trace end |
| stepped | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C49416 | not reached before trace end |
| stepped | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4941C | not reached before trace end |
| stepped | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C49422 | not reached before trace end |
| stepped | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C49428 | not reached before trace end |
| stepped | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4942E | not reached before trace end |
| stepped | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C49434 | not reached before trace end |
| stepped | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4943A | not reached before trace end |
| stepped | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C49440 | not reached before trace end |
| stepped | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C49446 | not reached before trace end |
| stepped | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4944C | not reached before trace end |
| stepped | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C49452 | not reached before trace end |
| stepped | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C49470 | not reached before trace end |
| stepped | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C49476 | not reached before trace end |
| stepped | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C49BF0 | not reached before trace end |
| stepped | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C49D70 | not reached before trace end |
| stepped | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C49D76 | not reached before trace end |
| stepped | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C49D7C | not reached before trace end |
| stepped | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C49DD0 | not reached before trace end |
| stepped | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C49DD6 | not reached before trace end |
| stepped | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C49DDC | not reached before trace end |
| stepped | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C49DE2 | not reached before trace end |
| stepped | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C49DE8 | not reached before trace end |
| stepped | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C49DEE | not reached before trace end |
| stepped | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C49E30 | not reached before trace end |
| stepped | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C49E36 | not reached before trace end |
| stepped | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C49E3C | not reached before trace end |
| stepped | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C49E42 | not reached before trace end |
| stepped | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C49EF0 | not reached before trace end |
| stepped | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C49F50 | not reached before trace end |
| stepped | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C49F56 | not reached before trace end |
| stepped | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C49F5C | not reached before trace end |
| stepped | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C49F62 | not reached before trace end |
| stepped | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C49F68 | not reached before trace end |
| stepped | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4A070 | not reached before trace end |
| stepped | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4A0D0 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, descriptor `+8` field, and three signed emitted placement words. The generic route uses that field as `$C45A36`, but type-specific gates can bypass it. These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
