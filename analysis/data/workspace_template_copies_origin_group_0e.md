# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_origin_group_0e_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **58** observed static-entry copies.  **10** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C48BD0 | not reached before trace end |
| stepped | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C48BD6 | not reached before trace end |
| stepped | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C48BDC | not reached before trace end |
| stepped | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C48BE2 | not reached before trace end |
| stepped | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C48BE8 | not reached before trace end |
| stepped | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C48BEE | not reached before trace end |
| stepped | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C48BF4 | not reached before trace end |
| stepped | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C48BFA | not reached before trace end |
| stepped | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C48C00 | not reached before trace end |
| stepped | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C48C06 | not reached before trace end |
| stepped | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C48C0C | not reached before trace end |
| stepped | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C48C12 | not reached before trace end |
| stepped | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C48C18 | not reached before trace end |
| stepped | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C48C1E | not reached before trace end |
| stepped | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C48C24 | not reached before trace end |
| stepped | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C48C2A | not reached before trace end |
| stepped | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C49110 | not reached before trace end |
| stepped | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C49116 | not reached before trace end |
| stepped | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4911C | not reached before trace end |
| stepped | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C49170 | not reached before trace end |
| stepped | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C4AEB0 | not reached before trace end |
| stepped | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C4AEB6 | not reached before trace end |
| stepped | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C4AEBC | not reached before trace end |
| stepped | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C4AEC2 | not reached before trace end |
| stepped | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C4AEC8 | not reached before trace end |
| stepped | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C4AECE | not reached before trace end |
| stepped | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C4AED4 | not reached before trace end |
| stepped | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C4AEDA | not reached before trace end |
| stepped | $C42951 | 66 | $52 -> $0052 | $0800 $0300 | $C4BC90 | $C4E9C2 / $C227F0 / (-320, 0, -232) |
| stepped | $C42957 | 66 | $15 -> $0015 | $0400 $0E00 | $C4BC96 | $C4E9DA / $C2232C / (-352, 0, -144) |
| stepped | $C4295D | 66 | $2D -> $002D | $0800 $0800 | $C4B990 | $C4E9AA / $C2250C / (-320, 0, 64) |
| stepped | $C42963 | 66 | $4A -> $004A | $0800 $0800 | $C4BED0 | $C4E9F2 / $C22750 / (-576, 0, -320) |
| stepped | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C48A50 | not reached before trace end |
| stepped | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C48A56 | not reached before trace end |
| stepped | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C48BD0 | not reached before trace end |
| stepped | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C48BD6 | not reached before trace end |
| stepped | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C48BDC | not reached before trace end |
| stepped | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C48C30 | not reached before trace end |
| stepped | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C48DB0 | not reached before trace end |
| stepped | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C49050 | not reached before trace end |
| stepped | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C49056 | not reached before trace end |
| stepped | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C49170 | not reached before trace end |
| stepped | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C491D0 | not reached before trace end |
| stepped | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C491D6 | not reached before trace end |
| stepped | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C49230 | not reached before trace end |
| stepped | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C49290 | not reached before trace end |
| stepped | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C493B0 | not reached before trace end |
| stepped | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C49410 | $C4F03A / $C229A8 / (-352, 0, -1312) |
| stepped | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C49470 | not reached before trace end |
| stepped | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C4AE50 | $C4F052 / $C228CC / (-576, 0, 576) |
| stepped | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C4AEB0 | not reached before trace end |
| stepped | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C4B030 | not reached before trace end |
| stepped | $C429D1 | 67 | $56 -> $0056 | $0600 $0100 | $C49C50 | $C4F6FA / $C22840 / (-84, 0, -30) |
| stepped | $C429D7 | 67 | $4A -> $004A | $0C00 $0400 | $C49DD0 | $C4F6CA / $C22750 / (-72, 0, -56) |
| stepped | $C429DF | 67 | $2A -> $002A | $0800 $0800 | $C4AD90 | $C4F72A / $C224D0 / (-16, 0, 112) |
| stepped | $C429E5 | 67 | $4A -> $004A | $0C00 $0400 | $C4B2D0 | $C4F712 / $C22750 / (-72, 0, 8) |
| stepped | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4C170 | not reached before trace end |
| stepped | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4C176 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
