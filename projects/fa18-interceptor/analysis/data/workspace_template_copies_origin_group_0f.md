# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_origin_group_0f_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **108** observed static-entry copies.  **75** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C489F0 | not reached before trace end |
| stepped | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C48B70 | not reached before trace end |
| stepped | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C48B76 | not reached before trace end |
| stepped | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C48B7C | not reached before trace end |
| stepped | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C48BD0 | not reached before trace end |
| stepped | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C48BD6 | not reached before trace end |
| stepped | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C48BDC | not reached before trace end |
| stepped | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C48BE2 | not reached before trace end |
| stepped | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C48BE8 | not reached before trace end |
| stepped | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C48BEE | not reached before trace end |
| stepped | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C48C30 | not reached before trace end |
| stepped | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C48C36 | not reached before trace end |
| stepped | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C48C3C | not reached before trace end |
| stepped | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C48C42 | not reached before trace end |
| stepped | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C48CF0 | $C4E9AA / $C22354 / (64, 0, -1216) |
| stepped | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C48D50 | $C4E9C2 / $C22368 / (-64, 0, -1216) |
| stepped | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C48D56 | $C4E9DA / $C22AAC / (-68, 0, -1188) |
| stepped | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C48D5C | $C4E9F2 / $C22430 / (-78, 0, -1179) |
| stepped | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C48D62 | $C4EA0A / $C22430 / (-89, 0, -1168) |
| stepped | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C48D68 | $C4EA22 / $C22430 / (-100, 0, -1157) |
| stepped | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C48E70 | $C4EA3A / $C2237C / (64, 0, -1344) |
| stepped | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C48ED0 | $C4EA52 / $C22390 / (-64, 0, -1344) |
| stepped | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C49110 | not reached before trace end |
| stepped | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C49170 | not reached before trace end |
| stepped | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C49290 | not reached before trace end |
| stepped | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C49296 | not reached before trace end |
| stepped | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C49410 | $C4EA6A / $C225FC / (-382, 0, -1328) |
| stepped | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C49416 | $C4EA82 / $C225E8 / (-362, 0, -1361) |
| stepped | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4941C | $C4EA9A / $C22624 / (-381, 0, -1377) |
| stepped | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C49422 | $C4EAB2 / $C226C4 / (-369, 0, -1367) |
| stepped | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C49428 | $C4EACA / $C226C4 / (-357, 0, -1357) |
| stepped | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4942E | $C4EAE2 / $C22610 / (-345, 0, -1347) |
| stepped | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C49434 | $C4EAFA / $C22688 / (-369, 0, -1302) |
| stepped | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4943A | $C4EB12 / $C226EC / (-370, 0, -1308) |
| stepped | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C49440 | $C4EB2A / $C2269C / (-372, 0, -1313) |
| stepped | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C49446 | $C4EB42 / $C2264C / (-369, 0, -1326) |
| stepped | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4944C | $C4EB5A / $C22638 / (-378, 0, -1323) |
| stepped | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C49452 | $C4EB72 / $C226D8 / (-380, 0, -1330) |
| stepped | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C49470 | not reached before trace end |
| stepped | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C49476 | not reached before trace end |
| stepped | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4A9D0 | $C4EB8A / $C223E0 / (-100, 0, 484) |
| stepped | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4A9D6 | $C4EBA2 / $C22598 / (-91, 0, 463) |
| stepped | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4A9DC | $C4EBBA / $C223F4 / (-98, 0, 475) |
| stepped | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4A9E2 | $C4EBD2 / $C223F4 / (-103, 0, 485) |
| stepped | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4A9E8 | $C4EBEA / $C223CC / (-110, 0, 497) |
| stepped | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4A9EE | $C4EC02 / $C223B8 / (-91, 0, 470) |
| stepped | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4A9F4 | $C4EC1A / $C223F4 / (-98, 0, 482) |
| stepped | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4A9FA | $C4EC32 / $C225C0 / (-104, 0, 494) |
| stepped | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4AA00 | $C4EC4A / $C225D4 / (-124, 0, 473) |
| stepped | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4AA06 | $C4EC62 / $C223A4 / (-110, 0, 480) |
| stepped | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4AA0C | $C4EC7A / $C22570 / (-99, 0, 487) |
| stepped | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4AA12 | $C4EC92 / $C223A4 / (-87, 0, 493) |
| stepped | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4AA18 | $C4ECAA / $C22570 / (-110, 0, 476) |
| stepped | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4AA1E | $C4ECC2 / $C22570 / (-100, 0, 481) |
| stepped | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4AA24 | $C4ECDA / $C22570 / (-90, 0, 487) |
| stepped | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4AA2A | $C4ECF2 / $C225AC / (-78, 0, 493) |
| stepped | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C4AF10 | $C4ED0A / $C22714 / (-384, 0, 440) |
| stepped | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C4AF16 | $C4ED22 / $C224A8 / (-355, 0, 416) |
| stepped | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4AF1C | $C4ED3A / $C22494 / (-315, 0, 388) |
| stepped | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C4AF70 | $C4ED52 / $C224BC / (-434, 0, 443) |
| stepped | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C4BAB0 | $C4ED6A / $C227B4 / (-684, 0, 48) |
| stepped | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C4BAB6 | $C4ED82 / $C2278C / (-679, 0, 69) |
| stepped | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C4BABC | $C4ED9A / $C227C8 / (-684, 0, 56) |
| stepped | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C4BAC2 | $C4EDB2 / $C227C8 / (-687, 0, 45) |
| stepped | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C4BAC8 | $C4EDCA / $C227DC / (-692, 0, 32) |
| stepped | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C4BACE | $C4EDE2 / $C227A0 / (-679, 0, 57) |
| stepped | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C4BAD4 | $C4EDFA / $C227C8 / (-684, 0, 44) |
| stepped | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C4BADA | $C4EE12 / $C227DC / (-688, 0, 31) |
| stepped | $C42951 | 66 | $52 -> $0052 | $0800 $0300 | $C4CE90 | not reached before trace end |
| stepped | $C42957 | 66 | $15 -> $0015 | $0400 $0E00 | $C4CE96 | not reached before trace end |
| stepped | $C4295D | 66 | $2D -> $002D | $0800 $0800 | $C4CB90 | $C4EE2A / $C2250C / (-320, 0, -448) |
| stepped | $C42963 | 66 | $4A -> $004A | $0800 $0800 | $C4D0D0 | not reached before trace end |
| stepped | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C48B70 | not reached before trace end |
| stepped | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C48BD0 | not reached before trace end |
| stepped | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C48BD6 | not reached before trace end |
| stepped | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C48C30 | not reached before trace end |
| stepped | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C48D50 | $C4F03A / $C22778 / (-84, 0, -1196) |
| stepped | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C48D56 | $C4F052 / $C22A84 / (-64, 0, -1248) |
| stepped | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C48ED0 | $C4F06A / $C22A98 / (-64, 0, -1344) |
| stepped | $C427B5 | 66 | $6C -> $006C | $0800 $0800 | $C49290 | not reached before trace end |
| stepped | $C427BB | 66 | $6D -> $006D | $0C00 $0400 | $C492F0 | not reached before trace end |
| stepped | $C427C1 | 66 | $6E -> $006E | $0800 $0800 | $C49470 | not reached before trace end |
| stepped | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4A850 | $C4F082 / $C224D0 / (-16, 0, 624) |
| stepped | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4A856 | $C4F09A / $C22A48 / (-64, 0, 576) |
| stepped | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4A9D0 | $C4F0B2 / $C22A5C / (-64, 0, 448) |
| stepped | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4A9D6 | $C4F0CA / $C224F8 / (-73, 0, 500) |
| stepped | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4A9DC | $C4F0E2 / $C224E4 / (-122, 0, 470) |
| stepped | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4AA30 | $C4F0FA / $C22A70 / (-160, 0, 416) |
| stepped | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4ABB0 | $C4F112 / $C22890 / (-192, 0, 320) |
| stepped | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C4AE50 | $C4F12A / $C2291C / (-576, 0, 576) |
| stepped | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C4AE56 | $C4F142 / $C22930 / (-608, 0, 512) |
| stepped | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C4AF70 | $C4F15A / $C22944 / (-512, 0, 480) |
| stepped | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C4AFD0 | $C4F172 / $C22958 / (-544, 0, 480) |
| stepped | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C4AFD6 | $C4F18A / $C2296C / (-640, 0, 480) |
| stepped | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C4B030 | not reached before trace end |
| stepped | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C4B090 | $C4F1A2 / $C2287C / (-288, 0, 320) |
| stepped | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C4B1B0 | $C4F1BA / $C22994 / (-704, 0, 320) |
| stepped | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C4B210 | $C4F1D2 / $C229A8 / (-352, 0, 224) |
| stepped | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C4B270 | $C4F1EA / $C22908 / (-448, 0, 160) |
| stepped | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C4BA50 | $C4F202 / $C228CC / (-576, 0, 64) |
| stepped | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C4BAB0 | $C4F21A / $C228E0 / (-704, 0, 0) |
| stepped | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C4BC30 | $C4F232 / $C228F4 / (-736, 0, -1) |
| stepped | $C429D1 | 67 | $56 -> $0056 | $0600 $0100 | $C49C50 | $C4F6FA / $C22840 / (-84, 0, -30) |
| stepped | $C429D7 | 67 | $4A -> $004A | $0C00 $0400 | $C49DD0 | $C4F6CA / $C22750 / (-72, 0, -56) |
| stepped | $C429DF | 67 | $2A -> $002A | $0800 $0800 | $C4AD90 | $C4F72A / $C224D0 / (-16, 0, 112) |
| stepped | $C429E5 | 67 | $4A -> $004A | $0C00 $0400 | $C4B2D0 | $C4F712 / $C22750 / (-72, 0, 8) |
| stepped | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4C170 | not reached before trace end |
| stepped | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4C176 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
