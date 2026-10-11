# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run037_m_map_band_walk_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **71** observed static-entry copies.  **0** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted placement / descriptor `+8` field |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C48AB0 | not reached before trace end |
| stepped | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C48AB6 | not reached before trace end |
| stepped | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C48ABC | not reached before trace end |
| stepped | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C48AC2 | not reached before trace end |
| stepped | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C48AC8 | not reached before trace end |
| stepped | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C48ACE | not reached before trace end |
| stepped | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C48AD4 | not reached before trace end |
| stepped | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C48ADA | not reached before trace end |
| stepped | $C42957 | 66 | $15 -> $0015 | $0400 $0E00 | $C49290 | not reached before trace end |
| stepped | $C4295D | 66 | $2D -> $002D | $0800 $0800 | $C48F90 | not reached before trace end |
| stepped | $C42963 | 66 | $4A -> $004A | $0800 $0800 | $C494D0 | not reached before trace end |
| stepped | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4A3D0 | not reached before trace end |
| stepped | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4A3D6 | not reached before trace end |
| stepped | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4A3DC | not reached before trace end |
| stepped | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4A3E2 | not reached before trace end |
| stepped | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4A3E8 | not reached before trace end |
| stepped | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4A3EE | not reached before trace end |
| stepped | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4A3F4 | not reached before trace end |
| stepped | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4A3FA | not reached before trace end |
| stepped | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4A400 | not reached before trace end |
| stepped | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4A406 | not reached before trace end |
| stepped | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4A40C | not reached before trace end |
| stepped | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4A412 | not reached before trace end |
| stepped | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4A418 | not reached before trace end |
| stepped | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4A41E | not reached before trace end |
| stepped | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4A424 | not reached before trace end |
| stepped | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4A42A | not reached before trace end |
| stepped | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C4A910 | not reached before trace end |
| stepped | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C4A916 | not reached before trace end |
| stepped | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4A91C | not reached before trace end |
| stepped | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C4A970 | not reached before trace end |
| stepped | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4B3F0 | not reached before trace end |
| stepped | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4B570 | not reached before trace end |
| stepped | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4B576 | not reached before trace end |
| stepped | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4B57C | not reached before trace end |
| stepped | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4B5D0 | not reached before trace end |
| stepped | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4B5D6 | not reached before trace end |
| stepped | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4B5DC | not reached before trace end |
| stepped | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4B5E2 | not reached before trace end |
| stepped | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4B5E8 | not reached before trace end |
| stepped | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4B5EE | not reached before trace end |
| stepped | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4B630 | not reached before trace end |
| stepped | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4B636 | not reached before trace end |
| stepped | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4B63C | not reached before trace end |
| stepped | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4B642 | not reached before trace end |
| stepped | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4B6F0 | not reached before trace end |
| stepped | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4B750 | not reached before trace end |
| stepped | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4B756 | not reached before trace end |
| stepped | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4B75C | not reached before trace end |
| stepped | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4B762 | not reached before trace end |
| stepped | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4B768 | not reached before trace end |
| stepped | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4B870 | not reached before trace end |
| stepped | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4B8D0 | not reached before trace end |
| stepped | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C4D310 | not reached before trace end |
| stepped | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C4D370 | not reached before trace end |
| stepped | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C4D490 | not reached before trace end |
| stepped | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C4D496 | not reached before trace end |
| stepped | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C4D610 | not reached before trace end |
| stepped | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C4D616 | not reached before trace end |
| stepped | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4D61C | not reached before trace end |
| stepped | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C4D622 | not reached before trace end |
| stepped | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C4D628 | not reached before trace end |
| stepped | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4D62E | not reached before trace end |
| stepped | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C4D634 | not reached before trace end |
| stepped | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4D63A | not reached before trace end |
| stepped | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C4D640 | not reached before trace end |
| stepped | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C4D646 | not reached before trace end |
| stepped | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4D64C | not reached before trace end |
| stepped | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C4D652 | not reached before trace end |
| stepped | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C4D670 | not reached before trace end |
| stepped | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C4D676 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, descriptor `+8` field, and three signed emitted placement words. The generic route uses that field as `$C45A36`, but type-specific gates can bypass it. These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
