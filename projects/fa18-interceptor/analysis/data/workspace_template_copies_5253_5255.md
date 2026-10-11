# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_frame05250_placement_thirtyframe_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **73** observed static-entry copies.  **21** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| 5253 | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C49DD0 | not reached before trace end |
| 5253 | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C49DD6 | not reached before trace end |
| 5253 | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C49DDC | not reached before trace end |
| 5253 | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C49DE2 | not reached before trace end |
| 5253 | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C49DE8 | not reached before trace end |
| 5253 | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C49DEE | not reached before trace end |
| 5253 | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C49DF4 | not reached before trace end |
| 5253 | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C49DFA | not reached before trace end |
| 5253 | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C49E00 | not reached before trace end |
| 5253 | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C49E06 | not reached before trace end |
| 5253 | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C49E0C | not reached before trace end |
| 5253 | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C49E12 | not reached before trace end |
| 5253 | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C49E18 | not reached before trace end |
| 5253 | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C49E1E | not reached before trace end |
| 5253 | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C49E24 | not reached before trace end |
| 5253 | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C49E2A | not reached before trace end |
| 5253 | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4A1F0 | not reached before trace end |
| 5253 | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4A370 | $C4F06A / $C22700 / (11008, 0, 6400) |
| 5253 | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4A376 | not reached before trace end |
| 5253 | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4A37C | not reached before trace end |
| 5253 | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4A3D0 | $C4F082 / $C22700 / (8864, 0, 9216) |
| 5253 | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4A3D6 | $C4F09A / $C22764 / (13120, 0, 8192) |
| 5253 | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4A3DC | not reached before trace end |
| 5253 | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4A3E2 | not reached before trace end |
| 5253 | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4A3E8 | not reached before trace end |
| 5253 | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4A3EE | not reached before trace end |
| 5253 | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4A430 | $C4F0FA / $C22A34 / (2048, 0, 10240) |
| 5253 | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4A436 | not reached before trace end |
| 5253 | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4A43C | not reached before trace end |
| 5253 | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4A442 | not reached before trace end |
| 5253 | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4A4F0 | not reached before trace end |
| 5253 | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4A550 | $C4F0CA / $C22778 / (11024, 0, 5376) |
| 5253 | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4A556 | $C4F0E2 / $C22A84 / (12288, 0, 2048) |
| 5253 | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4A55C | not reached before trace end |
| 5253 | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4A562 | not reached before trace end |
| 5253 | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4A568 | not reached before trace end |
| 5253 | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4A670 | not reached before trace end |
| 5253 | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4A6D0 | $C4F0B2 / $C22A98 / (12288, 0, -4096) |
| 5254 | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C49C50 | not reached before trace end |
| 5254 | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C49C56 | not reached before trace end |
| 5254 | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C49DD0 | not reached before trace end |
| 5254 | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C49DD6 | not reached before trace end |
| 5254 | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C49DDC | not reached before trace end |
| 5254 | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C49E30 | not reached before trace end |
| 5254 | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C49FB0 | not reached before trace end |
| 5254 | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4A370 | not reached before trace end |
| 5254 | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4A3D0 | not reached before trace end |
| 5254 | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4A3D6 | not reached before trace end |
| 5254 | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4A430 | not reached before trace end |
| 5254 | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4A550 | not reached before trace end |
| 5254 | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4A556 | not reached before trace end |
| 5254 | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4A6D0 | not reached before trace end |
| 5254 | $C42C37 | 67 | $30 -> $0030 | $0980 $0980 | $C48690 | not reached before trace end |
| 5254 | $C42C3D | 67 | $30 -> $0030 | $0580 $0640 | $C48696 | not reached before trace end |
| 5254 | $C42C43 | 67 | $4A -> $004A | $0780 $07C0 | $C4869C | not reached before trace end |
| 5254 | $C42C49 | 67 | $30 -> $0030 | $0E00 $0200 | $C48870 | not reached before trace end |
| 5254 | $C42C4F | 67 | $46 -> $0046 | $0540 $0800 | $C48930 | not reached before trace end |
| 5254 | $C42C55 | 67 | $22 -> $0022 | $0880 $0380 | $C48936 | not reached before trace end |
| 5254 | $C42999 | 66 | $24 -> $0024 | $0660 $0050 | $C48C90 | $C4F75A / $C22458 / (6960, 0, 10280) |
| 5254 | $C4299F | 66 | $25 -> $0025 | $0000 $0260 | $C48C96 | $C4F772 / $C2246C / (6144, 0, 10544) |
| 5254 | $C429A5 | 66 | $26 -> $0026 | $0F60 $02A0 | $C48CF0 | not reached before trace end |
| 5254 | $C429AB | 66 | $23 -> $0023 | $0C00 $0E00 | $C48E10 | $C4F78A / $C22444 / (7680, 0, 9984) |
| 5254 | $C42C5D | 67 | $2D -> $002D | $0200 $0040 | $C49590 | $C4F7A2 / $C2250C / (14592, 0, 6176) |
| 5254 | $C42C63 | 67 | $2D -> $002D | $0040 $0400 | $C49596 | $C4F7BA / $C2250C / (14368, 0, 6656) |
| 5254 | $C42C69 | 67 | $2E -> $002E | $0800 $0800 | $C495F0 | $C4F802 / $C22520 / (13312, 0, 7168) |
| 5254 | $C42C6F | 67 | $2F -> $002F | $0A00 $0300 | $C495F6 | $C4F81A / $C22534 / (13568, 0, 6528) |
| 5254 | $C42C75 | 67 | $2D -> $002D | $0100 $0840 | $C495FC | $C4F832 / $C2250C / (12416, 0, 7200) |
| 5254 | $C42C7B | 67 | $22 -> $0022 | $0960 $02A0 | $C49650 | $C4F84A / $C22430 / (11440, 0, 6480) |
| 5254 | $C42C81 | 67 | $22 -> $0022 | $03C0 $0840 | $C49656 | $C4F862 / $C22430 / (10720, 0, 7200) |
| 5254 | $C42C87 | 67 | $22 -> $0022 | $0E20 $0DE0 | $C496B0 | $C4F892 / $C22430 / (10000, 0, 7920) |
| 5254 | $C42C8D | 67 | $2D -> $002D | $0C00 $0800 | $C49710 | $C4F7D2 / $C2250C / (15872, 0, 5120) |
| 5254 | $C42C93 | 67 | $2D -> $002D | $0200 $0C00 | $C49716 | $C4F7EA / $C2250C / (14592, 0, 5632) |
| 5254 | $C42C99 | 67 | $75 -> $0075 | $0D56 $0ED2 | $C497D0 | $C4F87A / $C22AAC / (11947, 0, 5993) |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
