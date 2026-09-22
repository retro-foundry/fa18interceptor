# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_origin_mutation_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **60** observed static-entry copies.  **35** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42B47 | 67 | $55 -> $0055 | $0040 $0560 | $C4B870 | $C4E9AA / $C2282C / (514, 0, -341) |
| stepped | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4B9F0 | $C4E9C2 / $C2232C / (-512, 0, 0) |
| stepped | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4BB70 | $C4E9DA / $C22408 / (-436, 0, -84) |
| stepped | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4BB76 | $C4E9F2 / $C2241C / (-436, 0, -108) |
| stepped | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4BB7C | $C4EA0A / $C22804 / (-448, 0, -64) |
| stepped | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4BBD0 | $C4EA22 / $C22818 / (-576, 0, -64) |
| stepped | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4BBD6 | $C4EA3A / $C22318 / (-624, 0, -80) |
| stepped | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4BBDC | $C4EA52 / $C22584 / (-624, 0, -110) |
| stepped | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4BBE2 | $C4EA6A / $C22728 / (-624, 0, -110) |
| stepped | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4BBE8 | $C4EA82 / $C22430 / (-623, 0, -121) |
| stepped | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4BBEE | $C4EA9A / $C227F0 / (-528, 0, -80) |
| stepped | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4BC30 | $C4EAB2 / $C22458 / (-660, 0, -96) |
| stepped | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4BC36 | $C4EACA / $C2246C / (-672, 0, -92) |
| stepped | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4BC3C | $C4EAE2 / $C22480 / (-674, 0, -91) |
| stepped | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4BC42 | $C4EAFA / $C22444 / (-648, 0, -100) |
| stepped | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4BCF0 | $C4EB12 / $C22354 / (-448, 0, -192) |
| stepped | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4BD50 | $C4EB2A / $C22368 / (-576, 0, -192) |
| stepped | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4BD56 | $C4EB42 / $C22AAC / (-580, 0, -164) |
| stepped | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4BD5C | $C4EB5A / $C22430 / (-590, 0, -155) |
| stepped | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4BD62 | $C4EB72 / $C22430 / (-601, 0, -144) |
| stepped | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4BD68 | $C4EB8A / $C22430 / (-612, 0, -133) |
| stepped | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4BE70 | $C4EBA2 / $C2237C / (-448, 0, -320) |
| stepped | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4BED0 | $C4EBBA / $C22390 / (-576, 0, -320) |
| stepped | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4CDD0 | not reached before trace end |
| stepped | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4CDD6 | not reached before trace end |
| stepped | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4CDDC | not reached before trace end |
| stepped | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4CDE2 | not reached before trace end |
| stepped | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4CDE8 | not reached before trace end |
| stepped | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4CDEE | not reached before trace end |
| stepped | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4CDF4 | not reached before trace end |
| stepped | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4CDFA | not reached before trace end |
| stepped | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4CE00 | not reached before trace end |
| stepped | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4CE06 | not reached before trace end |
| stepped | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4CE0C | not reached before trace end |
| stepped | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4CE12 | not reached before trace end |
| stepped | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4CE18 | not reached before trace end |
| stepped | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4CE1E | not reached before trace end |
| stepped | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4CE24 | not reached before trace end |
| stepped | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4CE2A | not reached before trace end |
| stepped | $C42B3F | 67 | $58 -> $0058 | $0E00 $0480 | $C4B8D0 | $C4F03A / $C22868 / (496, 0, -348) |
| stepped | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4BB70 | $C4F052 / $C22700 / (-424, 0, -56) |
| stepped | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4BBD0 | $C4F06A / $C22700 / (-630, 0, -112) |
| stepped | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4BBD6 | $C4F082 / $C22764 / (-563, 0, -128) |
| stepped | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4BC30 | $C4F09A / $C22A34 / (-736, 0, -96) |
| stepped | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4BD50 | $C4F0B2 / $C22778 / (-596, 0, -172) |
| stepped | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4BD56 | $C4F0CA / $C22A84 / (-576, 0, -224) |
| stepped | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4BED0 | $C4F0E2 / $C22A98 / (-576, 0, -320) |
| stepped | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4CC50 | not reached before trace end |
| stepped | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4CC56 | not reached before trace end |
| stepped | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4CDD0 | not reached before trace end |
| stepped | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4CDD6 | not reached before trace end |
| stepped | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4CDDC | not reached before trace end |
| stepped | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4CE30 | not reached before trace end |
| stepped | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4CFB0 | not reached before trace end |
| stepped | $C429D1 | 67 | $56 -> $0056 | $0600 $0100 | $C49C50 | $C4F6FA / $C22840 / (-84, 0, -30) |
| stepped | $C429D7 | 67 | $4A -> $004A | $0C00 $0400 | $C49DD0 | $C4F6CA / $C22750 / (-72, 0, -56) |
| stepped | $C429DF | 67 | $2A -> $002A | $0800 $0800 | $C4AD90 | $C4F72A / $C224D0 / (-16, 0, 112) |
| stepped | $C429E5 | 67 | $4A -> $004A | $0C00 $0400 | $C4B2D0 | $C4F712 / $C22750 / (-72, 0, 8) |
| stepped | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4C170 | not reached before trace end |
| stepped | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4C176 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
