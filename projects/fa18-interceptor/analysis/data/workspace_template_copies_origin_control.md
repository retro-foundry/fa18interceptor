# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_origin_control_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **106** observed static-entry copies.  **91** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| stepped | $C42B47 | 67 | $55 -> $0055 | $0040 $0560 | $C49A70 | not reached before trace end |
| stepped | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4A7F0 | $C4E9AA / $C2232C / (0, 0, 512) |
| stepped | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4A970 | $C4E9C2 / $C22408 / (76, 0, 428) |
| stepped | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4A976 | $C4E9DA / $C2241C / (76, 0, 404) |
| stepped | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4A97C | $C4E9F2 / $C22804 / (64, 0, 448) |
| stepped | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4A9D0 | $C4EA0A / $C22818 / (-64, 0, 448) |
| stepped | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4A9D6 | $C4EA22 / $C22318 / (-112, 0, 432) |
| stepped | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4A9DC | $C4EA3A / $C22584 / (-112, 0, 402) |
| stepped | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4A9E2 | $C4EA52 / $C22728 / (-112, 0, 402) |
| stepped | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4A9E8 | $C4EA6A / $C22430 / (-111, 0, 391) |
| stepped | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4A9EE | $C4EA82 / $C227F0 / (-16, 0, 432) |
| stepped | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4AA30 | $C4EA9A / $C22458 / (-148, 0, 416) |
| stepped | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4AA36 | $C4EAB2 / $C2246C / (-160, 0, 420) |
| stepped | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4AA3C | $C4EACA / $C22480 / (-162, 0, 421) |
| stepped | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4AA42 | $C4EAE2 / $C22444 / (-136, 0, 412) |
| stepped | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4AAF0 | $C4EAFA / $C22354 / (64, 0, 320) |
| stepped | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4AB50 | $C4EB12 / $C22368 / (-64, 0, 320) |
| stepped | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4AB56 | $C4EB2A / $C22AAC / (-68, 0, 348) |
| stepped | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4AB5C | $C4EB42 / $C22430 / (-78, 0, 357) |
| stepped | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4AB62 | $C4EB5A / $C22430 / (-89, 0, 368) |
| stepped | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4AB68 | $C4EB72 / $C22430 / (-100, 0, 379) |
| stepped | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4AC70 | $C4EB8A / $C2237C / (64, 0, 192) |
| stepped | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4ACD0 | $C4EBA2 / $C22390 / (-64, 0, 192) |
| stepped | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C4AF10 | $C4EBBA / $C22700 / (-272, 0, 432) |
| stepped | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C4AF70 | $C4EBD2 / $C2232C / (-400, 0, 432) |
| stepped | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C4B090 | $C4EBEA / $C2232C / (-352, 0, 368) |
| stepped | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C4B096 | $C4EC02 / $C227F0 / (-320, 0, 280) |
| stepped | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C4B210 | $C4EC1A / $C225FC / (-382, 0, 208) |
| stepped | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C4B216 | $C4EC32 / $C225E8 / (-362, 0, 175) |
| stepped | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4B21C | $C4EC4A / $C22624 / (-381, 0, 159) |
| stepped | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C4B222 | $C4EC62 / $C226C4 / (-369, 0, 169) |
| stepped | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C4B228 | $C4EC7A / $C226C4 / (-357, 0, 179) |
| stepped | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4B22E | $C4EC92 / $C22610 / (-345, 0, 189) |
| stepped | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C4B234 | $C4ECAA / $C22688 / (-369, 0, 234) |
| stepped | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4B23A | $C4ECC2 / $C226EC / (-370, 0, 228) |
| stepped | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C4B240 | $C4ECDA / $C2269C / (-372, 0, 223) |
| stepped | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C4B246 | $C4ECF2 / $C2264C / (-369, 0, 210) |
| stepped | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4B24C | $C4ED0A / $C22638 / (-378, 0, 213) |
| stepped | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C4B252 | $C4ED22 / $C226D8 / (-380, 0, 206) |
| stepped | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C4B270 | $C4ED3A / $C22660 / (-391, 0, 202) |
| stepped | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C4B276 | $C4ED52 / $C22674 / (-393, 0, 208) |
| stepped | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C4BB10 | $C4ED6A / $C22714 / (-384, 0, -72) |
| stepped | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C4BB16 | $C4ED82 / $C224A8 / (-355, 0, -96) |
| stepped | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4BB1C | $C4ED9A / $C22494 / (-315, 0, -124) |
| stepped | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C4BB70 | $C4EDB2 / $C224BC / (-434, 0, -69) |
| stepped | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C4CCB0 | not reached before trace end |
| stepped | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C4CCB6 | not reached before trace end |
| stepped | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C4CCBC | not reached before trace end |
| stepped | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C4CCC2 | not reached before trace end |
| stepped | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C4CCC8 | not reached before trace end |
| stepped | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C4CCCE | not reached before trace end |
| stepped | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C4CCD4 | not reached before trace end |
| stepped | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C4CCDA | not reached before trace end |
| stepped | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4D3D0 | $C4EDCA / $C223E0 / (-100, 0, -28) |
| stepped | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4D3D6 | $C4EDE2 / $C22598 / (-91, 0, -49) |
| stepped | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4D3DC | $C4EDFA / $C223F4 / (-98, 0, -37) |
| stepped | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4D3E2 | $C4EE12 / $C223F4 / (-103, 0, -27) |
| stepped | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4D3E8 | $C4EE2A / $C223CC / (-110, 0, -15) |
| stepped | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4D3EE | $C4EE42 / $C223B8 / (-91, 0, -42) |
| stepped | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4D3F4 | $C4EE5A / $C223F4 / (-98, 0, -30) |
| stepped | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4D3FA | $C4EE72 / $C225C0 / (-104, 0, -18) |
| stepped | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4D400 | $C4EE8A / $C225D4 / (-124, 0, -39) |
| stepped | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4D406 | $C4EEA2 / $C223A4 / (-110, 0, -32) |
| stepped | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4D40C | $C4EEBA / $C22570 / (-99, 0, -25) |
| stepped | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4D412 | $C4EED2 / $C223A4 / (-87, 0, -19) |
| stepped | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4D418 | $C4EEEA / $C22570 / (-110, 0, -36) |
| stepped | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4D41E | $C4EF02 / $C22570 / (-100, 0, -31) |
| stepped | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4D424 | $C4EF1A / $C22570 / (-90, 0, -25) |
| stepped | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4D42A | $C4EF32 / $C225AC / (-78, 0, -19) |
| stepped | $C42B3F | 67 | $58 -> $0058 | $0E00 $0480 | $C49AD0 | not reached before trace end |
| stepped | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4A970 | $C4F03A / $C22700 / (88, 0, 456) |
| stepped | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4A9D0 | $C4F052 / $C22700 / (-118, 0, 400) |
| stepped | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4A9D6 | $C4F06A / $C22764 / (-51, 0, 384) |
| stepped | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4AA30 | $C4F082 / $C22A34 / (-224, 0, 416) |
| stepped | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4AB50 | $C4F09A / $C22778 / (-84, 0, 340) |
| stepped | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4AB56 | $C4F0B2 / $C22A84 / (-64, 0, 288) |
| stepped | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4ACD0 | $C4F0CA / $C22A98 / (-64, 0, 192) |
| stepped | $C427B5 | 66 | $6C -> $006C | $0800 $0800 | $C4B090 | $C4F0E2 / $C229F8 / (-320, 0, 320) |
| stepped | $C427BB | 66 | $6D -> $006D | $0C00 $0400 | $C4B0F0 | $C4F0FA / $C22A0C / (-416, 0, 288) |
| stepped | $C427C1 | 66 | $6E -> $006E | $0800 $0800 | $C4B270 | $C4F112 / $C22A20 / (-448, 0, 192) |
| stepped | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C4BA50 | $C4F12A / $C2291C / (-576, 0, 64) |
| stepped | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C4BA56 | $C4F142 / $C22930 / (-608, 0, 0) |
| stepped | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C4BB70 | $C4F15A / $C22944 / (-512, 0, -32) |
| stepped | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C4BBD0 | $C4F172 / $C22958 / (-544, 0, -32) |
| stepped | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C4BBD6 | $C4F18A / $C2296C / (-640, 0, -32) |
| stepped | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C4BC30 | $C4F1A2 / $C22980 / (-672, 0, -96) |
| stepped | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C4BC90 | $C4F1BA / $C2287C / (-288, 0, -192) |
| stepped | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C4BDB0 | $C4F1D2 / $C22994 / (-704, 0, -192) |
| stepped | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C4BE10 | $C4F1EA / $C229A8 / (-352, 0, -288) |
| stepped | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C4BE70 | $C4F202 / $C22908 / (-448, 0, -352) |
| stepped | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C4CC50 | not reached before trace end |
| stepped | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C4CCB0 | not reached before trace end |
| stepped | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C4CE30 | not reached before trace end |
| stepped | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4D250 | $C4F292 / $C224D0 / (-16, 0, 112) |
| stepped | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4D256 | $C4F2AA / $C22A48 / (-64, 0, 64) |
| stepped | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4D3D0 | $C4F24A / $C22A5C / (-64, 0, -64) |
| stepped | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4D3D6 | $C4F262 / $C224F8 / (-73, 0, -12) |
| stepped | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4D3DC | $C4F27A / $C224E4 / (-122, 0, -42) |
| stepped | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4D430 | $C4F232 / $C22A70 / (-160, 0, -96) |
| stepped | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4D5B0 | $C4F21A / $C22890 / (-192, 0, -192) |
| stepped | $C429D1 | 67 | $56 -> $0056 | $0600 $0100 | $C49C50 | $C4F6FA / $C22840 / (-84, 0, -30) |
| stepped | $C429D7 | 67 | $4A -> $004A | $0C00 $0400 | $C49DD0 | $C4F6CA / $C22750 / (-72, 0, -56) |
| stepped | $C429DF | 67 | $2A -> $002A | $0800 $0800 | $C4AD90 | $C4F72A / $C224D0 / (-16, 0, 112) |
| stepped | $C429E5 | 67 | $4A -> $004A | $0C00 $0400 | $C4B2D0 | $C4F712 / $C22750 / (-72, 0, 8) |
| stepped | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4C170 | not reached before trace end |
| stepped | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4C176 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
