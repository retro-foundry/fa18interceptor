# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run003_m_map_appearance_25f_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **107** observed static-entry copies.  **103** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| 13 | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C48AB0 | $C4E9AA / $C227B4 / (-172, 0, -592) |
| 13 | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C48AB6 | $C4E9C2 / $C2278C / (-167, 0, -571) |
| 13 | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C48ABC | $C4E9DA / $C227C8 / (-172, 0, -584) |
| 13 | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C48AC2 | $C4E9F2 / $C227C8 / (-175, 0, -595) |
| 13 | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C48AC8 | $C4EA0A / $C227DC / (-180, 0, -608) |
| 13 | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C48ACE | $C4EA22 / $C227A0 / (-167, 0, -583) |
| 13 | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C48AD4 | $C4EA3A / $C227C8 / (-172, 0, -596) |
| 13 | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C48ADA | $C4EA52 / $C227DC / (-176, 0, -609) |
| 13 | $C42957 | 66 | $15 -> $0015 | $0400 $0E00 | $C49290 | not reached before trace end |
| 13 | $C4295D | 66 | $2D -> $002D | $0800 $0800 | $C48F90 | $C4EA6A / $C2250C / (-320, 0, -576) |
| 13 | $C42963 | 66 | $4A -> $004A | $0800 $0800 | $C494D0 | not reached before trace end |
| 13 | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4A3D0 | $C4EA82 / $C223E0 / (412, 0, -156) |
| 13 | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4A3D6 | $C4EA9A / $C22598 / (421, 0, -177) |
| 13 | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4A3DC | $C4EAB2 / $C223F4 / (414, 0, -165) |
| 13 | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4A3E2 | $C4EACA / $C223F4 / (409, 0, -155) |
| 13 | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4A3E8 | $C4EAE2 / $C223CC / (402, 0, -143) |
| 13 | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4A3EE | $C4EAFA / $C223B8 / (421, 0, -170) |
| 13 | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4A3F4 | $C4EB12 / $C223F4 / (414, 0, -158) |
| 13 | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4A3FA | $C4EB2A / $C225C0 / (408, 0, -146) |
| 13 | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4A400 | $C4EB42 / $C225D4 / (388, 0, -167) |
| 13 | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4A406 | $C4EB5A / $C223A4 / (402, 0, -160) |
| 13 | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4A40C | $C4EB72 / $C22570 / (413, 0, -153) |
| 13 | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4A412 | $C4EB8A / $C223A4 / (425, 0, -147) |
| 13 | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4A418 | $C4EBA2 / $C22570 / (402, 0, -164) |
| 13 | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4A41E | $C4EBBA / $C22570 / (412, 0, -159) |
| 13 | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4A424 | $C4EBD2 / $C22570 / (422, 0, -153) |
| 13 | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4A42A | $C4EBEA / $C225AC / (434, 0, -147) |
| 13 | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C4A910 | $C4EC02 / $C22714 / (128, 0, -200) |
| 13 | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C4A916 | $C4EC1A / $C224A8 / (157, 0, -224) |
| 13 | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4A91C | $C4EC32 / $C22494 / (197, 0, -252) |
| 13 | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C4A970 | $C4EC4A / $C224BC / (78, 0, -197) |
| 13 | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4B3F0 | $C4EE5A / $C2232C / (512, 0, 384) |
| 13 | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4B570 | $C4ED22 / $C22408 / (588, 0, 300) |
| 13 | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4B576 | $C4ED3A / $C2241C / (588, 0, 276) |
| 13 | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4B57C | $C4ED52 / $C22804 / (576, 0, 320) |
| 13 | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4B5D0 | $C4ED6A / $C22818 / (448, 0, 320) |
| 13 | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4B5D6 | $C4ED82 / $C22318 / (400, 0, 304) |
| 13 | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4B5DC | $C4ED9A / $C22584 / (400, 0, 274) |
| 13 | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4B5E2 | $C4EDB2 / $C22728 / (400, 0, 274) |
| 13 | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4B5E8 | $C4EDCA / $C22430 / (401, 0, 263) |
| 13 | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4B5EE | $C4EDE2 / $C227F0 / (496, 0, 304) |
| 13 | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4B630 | $C4EDFA / $C22458 / (364, 0, 288) |
| 13 | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4B636 | $C4EE12 / $C2246C / (352, 0, 292) |
| 13 | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4B63C | $C4EE2A / $C22480 / (350, 0, 293) |
| 13 | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4B642 | $C4EE42 / $C22444 / (376, 0, 284) |
| 13 | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4B6F0 | $C4EC92 / $C22354 / (576, 0, 192) |
| 13 | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4B750 | $C4ECAA / $C22368 / (448, 0, 192) |
| 13 | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4B756 | $C4ECC2 / $C22AAC / (444, 0, 220) |
| 13 | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4B75C | $C4ECDA / $C22430 / (434, 0, 229) |
| 13 | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4B762 | $C4ECF2 / $C22430 / (423, 0, 240) |
| 13 | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4B768 | $C4ED0A / $C22430 / (412, 0, 251) |
| 13 | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4B870 | $C4EC62 / $C2237C / (576, 0, 64) |
| 13 | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4B8D0 | $C4EC7A / $C22390 / (448, 0, 64) |
| 13 | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C4D310 | $C4EE8A / $C22700 / (240, 0, 304) |
| 13 | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C4D370 | $C4EE72 / $C2232C / (112, 0, 304) |
| 13 | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C4D490 | $C4EEA2 / $C2232C / (160, 0, 240) |
| 13 | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C4D496 | $C4EEBA / $C227F0 / (192, 0, 152) |
| 13 | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C4D610 | $C4EED2 / $C225FC / (130, 0, 80) |
| 13 | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C4D616 | $C4EEEA / $C225E8 / (150, 0, 47) |
| 13 | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4D61C | $C4EF02 / $C22624 / (131, 0, 31) |
| 13 | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C4D622 | $C4EF1A / $C226C4 / (143, 0, 41) |
| 13 | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C4D628 | $C4EF32 / $C226C4 / (155, 0, 51) |
| 13 | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4D62E | $C4EF4A / $C22610 / (167, 0, 61) |
| 13 | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C4D634 | $C4EF62 / $C22688 / (143, 0, 106) |
| 13 | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4D63A | $C4EF7A / $C226EC / (142, 0, 100) |
| 13 | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C4D640 | $C4EF92 / $C2269C / (140, 0, 95) |
| 13 | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C4D646 | $C4EFAA / $C2264C / (143, 0, 82) |
| 13 | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4D64C | $C4EFC2 / $C22638 / (134, 0, 85) |
| 13 | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C4D652 | $C4EFDA / $C226D8 / (132, 0, 78) |
| 13 | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C4D670 | $C4EFF2 / $C22660 / (121, 0, 74) |
| 13 | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C4D676 | $C4F00A / $C22674 / (119, 0, 80) |
| 15 | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C48A50 | $C4F03A / $C228CC / (-64, 0, -576) |
| 15 | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C48AB0 | $C4F052 / $C228E0 / (-192, 0, -640) |
| 15 | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C48C30 | not reached before trace end |
| 15 | $C42721 | 66 | $6A -> $006A | $0800 $0800 | $C48F90 | $C4F06A / $C229D0 / (-320, 0, -576) |
| 15 | $C42727 | 66 | $6B -> $006B | $0800 $0C00 | $C49110 | not reached before trace end |
| 15 | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4A250 | $C4F0FA / $C224D0 / (496, 0, -16) |
| 15 | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4A256 | $C4F112 / $C22A48 / (448, 0, -64) |
| 15 | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4A3D0 | $C4F09A / $C22A5C / (448, 0, -192) |
| 15 | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4A3D6 | $C4F0B2 / $C224F8 / (439, 0, -140) |
| 15 | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4A3DC | $C4F0CA / $C224E4 / (390, 0, -170) |
| 15 | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4A430 | $C4F0E2 / $C22A70 / (352, 0, -224) |
| 15 | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4A5B0 | $C4F082 / $C22890 / (320, 0, -320) |
| 15 | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C4A850 | $C4F1EA / $C2291C / (-64, 0, -64) |
| 15 | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C4A856 | $C4F202 / $C22930 / (-96, 0, -128) |
| 15 | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C4A970 | $C4F18A / $C22944 / (0, 0, -160) |
| 15 | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C4A9D0 | $C4F1A2 / $C22958 / (-32, 0, -160) |
| 15 | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C4A9D6 | $C4F1BA / $C2296C / (-128, 0, -160) |
| 15 | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C4AA30 | $C4F1D2 / $C22980 / (-160, 0, -224) |
| 15 | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C4AA90 | $C4F15A / $C2287C / (224, 0, -320) |
| 15 | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C4ABB0 | $C4F172 / $C22994 / (-192, 0, -320) |
| 15 | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C4AC10 | $C4F12A / $C229A8 / (160, 0, -416) |
| 15 | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C4AC70 | $C4F142 / $C22908 / (64, 0, -480) |
| 15 | $C42743 | 66 | $69 -> $0069 | $0800 $0800 | $C4B210 | $C4F21A / $C229BC / (-320, 0, -448) |
| 15 | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4B570 | $C4F27A / $C22700 / (600, 0, 328) |
| 15 | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4B5D0 | $C4F292 / $C22700 / (394, 0, 272) |
| 15 | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4B5D6 | $C4F2AA / $C22764 / (461, 0, 256) |
| 15 | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4B630 | $C4F2C2 / $C22A34 / (288, 0, 288) |
| 15 | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4B750 | $C4F24A / $C22778 / (428, 0, 212) |
| 15 | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4B756 | $C4F262 / $C22A84 / (448, 0, 160) |
| 15 | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4B8D0 | $C4F232 / $C22A98 / (448, 0, 64) |
| 15 | $C427B5 | 66 | $6C -> $006C | $0800 $0800 | $C4D490 | $C4F2DA / $C229F8 / (192, 0, 192) |
| 15 | $C427BB | 66 | $6D -> $006D | $0C00 $0400 | $C4D4F0 | $C4F2F2 / $C22A0C / (96, 0, 160) |
| 15 | $C427C1 | 66 | $6E -> $006E | $0800 $0800 | $C4D670 | $C4F30A / $C22A20 / (64, 0, 64) |
| 16 | $C429ED | 67 | $56 -> $0056 | $0050 $0880 | $C4B1B0 | $C4F6CA / $C22840 / (128, 0, 49) |
| 16 | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4B570 | $C4F6E2 / $C2250C / (-48, 0, 80) |
| 16 | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4B576 | $C4F6FA / $C2250C / (-50, 0, 78) |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
