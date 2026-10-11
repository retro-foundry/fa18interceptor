# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run037_m_map_template_to_placement_3f_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **110** observed static-entry copies.  **100** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted placement / descriptor `+8` field |
| ---: | --- | ---: | --- | --- | --- | --- |
| 5717 | $C42ADB | 67 | $4F -> $004F | $0A80 $0600 | $C48AB0 | $C4E9AA / $C227B4 / $C0DD30 / (-300, 0, -720) |
| 5717 | $C42AE1 | 67 | $4D -> $004D | $0B38 $08A8 | $C48AB6 | $C4E9C2 / $C2278C / $C0DB50 / (-295, 0, -699) |
| 5717 | $C42AE7 | 67 | $50 -> $0050 | $0A9D $0700 | $C48ABC | $C4E9DA / $C227C8 / $C08758 / (-300, 0, -712) |
| 5717 | $C42AED | 67 | $50 -> $0050 | $0A21 $05AC | $C48AC2 | $C4E9F2 / $C227C8 / $C08758 / (-303, 0, -723) |
| 5717 | $C42AF3 | 67 | $51 -> $0051 | $0990 $041F | $C48AC8 | $C4EA0A / $C227DC / $C37854 / (-308, 0, -736) |
| 5717 | $C42AF9 | 67 | $4E -> $004E | $0B2F $0730 | $C48ACE | $C4EA22 / $C227A0 / $C0DB7C / (-295, 0, -711) |
| 5717 | $C42AFF | 67 | $50 -> $0050 | $0A94 $0588 | $C48AD4 | $C4EA3A / $C227C8 / $C08758 / (-300, 0, -724) |
| 5717 | $C42B05 | 67 | $51 -> $0051 | $0A02 $03F8 | $C48ADA | $C4EA52 / $C227DC / $C37854 / (-304, 0, -737) |
| 5717 | $C42957 | 66 | $15 -> $0015 | $0400 $0E00 | $C49290 | not reached before trace end |
| 5717 | $C4295D | 66 | $2D -> $002D | $0800 $0800 | $C48F90 | $C4EA6A / $C2250C / $C3A858 / (-448, 0, -704) |
| 5717 | $C42963 | 66 | $4A -> $004A | $0800 $0800 | $C494D0 | not reached before trace end |
| 5717 | $C42BD5 | 67 | $1E -> $001E | $0380 $0C80 | $C4A3D0 | $C4EA82 / $C223E0 / $C08928 / (284, 0, -284) |
| 5717 | $C42BDB | 67 | $34 -> $0034 | $04A0 $09E0 | $C4A3D6 | $C4EA9A / $C22598 / $C3723E / (293, 0, -305) |
| 5717 | $C42BE1 | 67 | $1F -> $001F | $03D2 $0B64 | $C4A3DC | $C4EAB2 / $C223F4 / $C08718 / (286, 0, -293) |
| 5717 | $C42BE7 | 67 | $1F -> $001F | $0327 $0CA6 | $C4A3E2 | $C4EACA / $C223F4 / $C08718 / (281, 0, -283) |
| 5717 | $C42BED | 67 | $1D -> $001D | $025E $0E21 | $C4A3E8 | $C4EAE2 / $C223CC / $C381FC / (274, 0, -271) |
| 5717 | $C42BF3 | 67 | $1C -> $001C | $04AA $0ACE | $C4A3EE | $C4EAFA / $C223B8 / $C37218 / (293, 0, -298) |
| 5717 | $C42BF9 | 67 | $1F -> $001F | $03D8 $0C5A | $C4A3F4 | $C4EB12 / $C223F4 / $C08718 / (286, 0, -286) |
| 5717 | $C42BFF | 67 | $36 -> $0036 | $030F $0DD4 | $C4A3FA | $C4EB2A / $C225C0 / $C38228 / (280, 0, -274) |
| 5717 | $C42C05 | 67 | $37 -> $0037 | $0090 $0B35 | $C4A400 | $C4EB42 / $C225D4 / $C376AC / (260, 0, -295) |
| 5717 | $C42C0B | 67 | $1B -> $001B | $0244 $0C1C | $C4A406 | $C4EB5A / $C223A4 / $C087AC / (274, 0, -288) |
| 5717 | $C42C11 | 67 | $32 -> $0032 | $03BF $0CE5 | $C4A40C | $C4EB72 / $C22570 / $C08798 / (285, 0, -281) |
| 5717 | $C42C17 | 67 | $1B -> $001B | $0538 $0DAD | $C4A412 | $C4EB8A / $C223A4 / $C087AC / (297, 0, -275) |
| 5717 | $C42C1D | 67 | $32 -> $0032 | $0240 $0B92 | $C4A418 | $C4EBA2 / $C22570 / $C08798 / (274, 0, -292) |
| 5717 | $C42C23 | 67 | $32 -> $0032 | $0386 $0C3F | $C4A41E | $C4EBBA / $C22570 / $C08798 / (284, 0, -287) |
| 5717 | $C42C29 | 67 | $32 -> $0032 | $04C8 $0CEA | $C4A424 | $C4EBD2 / $C22570 / $C08798 / (294, 0, -281) |
| 5717 | $C42C2F | 67 | $35 -> $0035 | $0650 $0DBA | $C4A42A | $C4EBEA / $C225AC / $C373A2 / (306, 0, -275) |
| 5717 | $C42707 | 66 | $47 -> $0047 | $0000 $0700 | $C4A910 | $C4EC02 / $C22714 / $C35BD0 / (0, 0, -328) |
| 5717 | $C4270D | 66 | $28 -> $0028 | $03A0 $0400 | $C4A916 | $C4EC1A / $C224A8 / $C35D88 / (29, 0, -352) |
| 5717 | $C42713 | 66 | $27 -> $0027 | $08B0 $0090 | $C4A91C | $C4EC32 / $C22494 / $C35BFC / (69, 0, -380) |
| 5717 | $C42719 | 66 | $29 -> $0029 | $09C0 $0760 | $C4A970 | $C4EC4A / $C224BC / $C35E9E / (-50, 0, -325) |
| 5717 | $C42647 | 66 | $15 -> $0015 | $0000 $0000 | $C4B3F0 | $C4EE5A / $C2232C / $C445DC / (384, 0, 256) |
| 5717 | $C4264D | 66 | $20 -> $0020 | $0980 $0580 | $C4B570 | $C4ED22 / $C22408 / $C35568 / (460, 0, 172) |
| 5717 | $C42653 | 66 | $21 -> $0021 | $0980 $0280 | $C4B576 | $C4ED3A / $C2241C / $C355A0 / (460, 0, 148) |
| 5717 | $C42659 | 66 | $53 -> $0053 | $0800 $0800 | $C4B57C | $C4ED52 / $C22804 / $C44732 / (448, 0, 192) |
| 5717 | $C4265F | 66 | $54 -> $0054 | $0800 $0800 | $C4B5D0 | $C4ED6A / $C22818 / $C447C6 / (320, 0, 192) |
| 5717 | $C42665 | 66 | $14 -> $0014 | $0200 $0600 | $C4B5D6 | $C4ED82 / $C22318 / $C445A2 / (272, 0, 176) |
| 5717 | $C4266B | 66 | $33 -> $0033 | $0200 $0240 | $C4B5DC | $C4ED9A / $C22584 / $C36212 / (272, 0, 146) |
| 5717 | $C42671 | 66 | $48 -> $0048 | $0200 $0240 | $C4B5E2 | $C4EDB2 / $C22728 / $C36208 / (272, 0, 146) |
| 5717 | $C42677 | 66 | $22 -> $0022 | $0220 $00E0 | $C4B5E8 | $C4EDCA / $C22430 / $C3623E / (273, 0, 135) |
| 5717 | $C4267D | 66 | $52 -> $0052 | $0E00 $0600 | $C4B5EE | $C4EDE2 / $C227F0 / $C44612 / (368, 0, 176) |
| 5717 | $C42683 | 66 | $24 -> $0024 | $0D98 $0414 | $C4B630 | $C4EDFA / $C22458 / $C36E6A / (236, 0, 160) |
| 5717 | $C42689 | 66 | $25 -> $0025 | $0C00 $0498 | $C4B636 | $C4EE12 / $C2246C / $C366E6 / (224, 0, 164) |
| 5717 | $C4268F | 66 | $26 -> $0026 | $0BD8 $04A8 | $C4B63C | $C4EE2A / $C22480 / $C368D0 / (222, 0, 165) |
| 5717 | $C42695 | 66 | $23 -> $0023 | $0F00 $0380 | $C4B642 | $C4EE42 / $C22444 / $C36A28 / (248, 0, 156) |
| 5717 | $C4269B | 66 | $17 -> $0017 | $0800 $0800 | $C4B6F0 | $C4EC92 / $C22354 / $C4466E / (448, 0, 64) |
| 5717 | $C426A1 | 66 | $18 -> $0018 | $0800 $0800 | $C4B750 | $C4ECAA / $C22368 / $C4477C / (320, 0, 64) |
| 5717 | $C426A7 | 66 | $75 -> $0075 | $0780 $0B80 | $C4B756 | $C4ECC2 / $C22AAC / $C3B960 / (316, 0, 92) |
| 5717 | $C426AD | 66 | $22 -> $0022 | $0658 $0CA8 | $C4B75C | $C4ECDA / $C22430 / $C3623E / (306, 0, 101) |
| 5717 | $C426B3 | 66 | $22 -> $0022 | $04F0 $0E10 | $C4B762 | $C4ECF2 / $C22430 / $C3623E / (295, 0, 112) |
| 5717 | $C426B9 | 66 | $22 -> $0022 | $0388 $0F78 | $C4B768 | $C4ED0A / $C22430 / $C3623E / (284, 0, 123) |
| 5717 | $C426BF | 66 | $19 -> $0019 | $0800 $0800 | $C4B870 | $C4EC62 / $C2237C / $C44812 / (448, 0, -64) |
| 5717 | $C426C5 | 66 | $1A -> $001A | $0800 $0800 | $C4B8D0 | $C4EC7A / $C22390 / $C4483C / (320, 0, -64) |
| 5717 | $C42B67 | 67 | $46 -> $0046 | $0E00 $0600 | $C4D310 | $C4EE8A / $C22700 / $C3B4F8 / (112, 0, 176) |
| 5717 | $C42B6D | 67 | $15 -> $0015 | $0E00 $0600 | $C4D370 | $C4EE72 / $C2232C / $C445DC / (-16, 0, 176) |
| 5717 | $C42B73 | 67 | $15 -> $0015 | $0400 $0E00 | $C4D490 | $C4EEA2 / $C2232C / $C445DC / (32, 0, 112) |
| 5717 | $C42B79 | 67 | $52 -> $0052 | $0800 $0300 | $C4D496 | $C4EEBA / $C227F0 / $C44612 / (64, 0, 24) |
| 5717 | $C42B7F | 67 | $39 -> $0039 | $0040 $0A00 | $C4D610 | $C4EED2 / $C225FC / $C37EA6 / (2, 0, -48) |
| 5717 | $C42B85 | 67 | $38 -> $0038 | $02D0 $05F0 | $C4D616 | $C4EEEA / $C225E8 / $C37E56 / (22, 0, -81) |
| 5717 | $C42B8B | 67 | $3B -> $003B | $0070 $03F0 | $C4D61C | $C4EF02 / $C22624 / $C37F80 / (3, 0, -97) |
| 5717 | $C42B91 | 67 | $43 -> $0043 | $01F6 $0538 | $C4D622 | $C4EF1A / $C226C4 / $C08892 / (15, 0, -87) |
| 5717 | $C42B97 | 67 | $43 -> $0043 | $036E $0675 | $C4D628 | $C4EF32 / $C226C4 / $C08892 / (27, 0, -77) |
| 5717 | $C42B9D | 67 | $3A -> $003A | $04E5 $07B1 | $C4D62E | $C4EF4A / $C22610 / $C380D8 / (39, 0, -67) |
| 5717 | $C42BA3 | 67 | $40 -> $0040 | $01FE $0D4B | $C4D634 | $C4EF62 / $C22688 / $C37CF8 / (15, 0, -22) |
| 5717 | $C42BA9 | 67 | $45 -> $0045 | $01C2 $0C99 | $C4D63A | $C4EF7A / $C226EC / $C0883E / (14, 0, -28) |
| 5717 | $C42BAF | 67 | $41 -> $0041 | $018C $0BE8 | $C4D640 | $C4EF92 / $C2269C / $C37D94 / (12, 0, -33) |
| 5717 | $C42BB5 | 67 | $3D -> $003D | $01FB $0A4C | $C4D646 | $C4EFAA / $C2264C / $C37B78 / (15, 0, -46) |
| 5717 | $C42BBB | 67 | $3C -> $003C | $00DD $0AB1 | $C4D64C | $C4EFC2 / $C22638 / $C37B4C / (6, 0, -43) |
| 5717 | $C42BC1 | 67 | $44 -> $0044 | $0097 $09CA | $C4D652 | $C4EFDA / $C226D8 / $C087FE / (4, 0, -50) |
| 5717 | $C42BC7 | 67 | $3E -> $003E | $0F33 $0949 | $C4D670 | $C4EFF2 / $C22660 / $C37990 / (-7, 0, -54) |
| 5717 | $C42BCD | 67 | $3F -> $003F | $0EF8 $0A00 | $C4D676 | $C4F00A / $C22674 / $C379BC / (-9, 0, -48) |
| 5718 | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C48A50 | $C4F03A / $C228CC / $C44A3A / (-192, 0, -704) |
| 5718 | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C48AB0 | $C4F052 / $C228E0 / $C44AA4 / (-320, 0, -768) |
| 5718 | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C48C30 | not reached before trace end |
| 5718 | $C42721 | 66 | $6A -> $006A | $0800 $0800 | $C48F90 | $C4F06A / $C229D0 / $C45026 / (-448, 0, -704) |
| 5718 | $C42727 | 66 | $6B -> $006B | $0800 $0C00 | $C49110 | not reached before trace end |
| 5718 | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4A250 | $C4F0FA / $C224D0 / $C3B6A6 / (368, 0, -144) |
| 5718 | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4A256 | $C4F112 / $C22A48 / $C4532C / (320, 0, -192) |
| 5718 | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4A3D0 | $C4F09A / $C22A5C / $C453C4 / (320, 0, -320) |
| 5718 | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4A3D6 | $C4F0B2 / $C224F8 / $C3737C / (311, 0, -268) |
| 5718 | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4A3DC | $C4F0CA / $C224E4 / $C37680 / (262, 0, -298) |
| 5718 | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4A430 | $C4F0E2 / $C22A70 / $C4545C / (224, 0, -352) |
| 5718 | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4A5B0 | $C4F082 / $C22890 / $C44970 / (192, 0, -448) |
| 5718 | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C4A850 | $C4F1EA / $C2291C / $C44B78 / (-192, 0, -192) |
| 5718 | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C4A856 | $C4F202 / $C22930 / $C44C10 / (-224, 0, -256) |
| 5718 | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C4A970 | $C4F18A / $C22944 / $C44C7A / (-128, 0, -288) |
| 5718 | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C4A9D0 | $C4F1A2 / $C22958 / $C44CE4 / (-160, 0, -288) |
| 5718 | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C4A9D6 | $C4F1BA / $C2296C / $C44D4E / (-256, 0, -288) |
| 5718 | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C4AA30 | $C4F1D2 / $C22980 / $C44DB8 / (-288, 0, -352) |
| 5718 | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C4AA90 | $C4F15A / $C2287C / $C448B6 / (96, 0, -448) |
| 5718 | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C4ABB0 | $C4F172 / $C22994 / $C44E22 / (-320, 0, -448) |
| 5718 | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C4AC10 | $C4F12A / $C229A8 / $C44E8C / (32, 0, -544) |
| 5718 | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C4AC70 | $C4F142 / $C22908 / $C44EF6 / (-64, 0, -608) |
| 5718 | $C42743 | 66 | $69 -> $0069 | $0800 $0800 | $C4B210 | $C4F21A / $C229BC / $C44F8E / (-448, 0, -576) |
| 5718 | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4B570 | $C4F27A / $C22700 / $C3B4F8 / (472, 0, 200) |
| 5718 | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4B5D0 | $C4F292 / $C22700 / $C3B4F8 / (266, 0, 144) |
| 5718 | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4B5D6 | $C4F2AA / $C22764 / $C44500 / (333, 0, 128) |
| 5718 | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4B630 | $C4F2C2 / $C22A34 / $C45294 / (160, 0, 160) |
| 5718 | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4B750 | $C4F24A / $C22778 / $C4455A / (300, 0, 84) |
| 5718 | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4B756 | $C4F262 / $C22A84 / $C454F4 / (320, 0, 32) |
| 5718 | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4B8D0 | $C4F232 / $C22A98 / $C4558C / (320, 0, -64) |
| 5718 | $C427B5 | 66 | $6C -> $006C | $0800 $0800 | $C4D490 | $C4F2DA / $C229F8 / $C45128 / (64, 0, 64) |
| 5718 | $C427BB | 66 | $6D -> $006D | $0C00 $0400 | $C4D4F0 | $C4F2F2 / $C22A0C / $C45192 / (-32, 0, 32) |
| 5718 | $C427C1 | 66 | $6E -> $006E | $0800 $0800 | $C4D670 | $C4F30A / $C22A20 / $C451FC / (-64, 0, -64) |
| 5719 | $C42C37 | 67 | $30 -> $0030 | $0980 $0980 | $C49890 | not reached before trace end |
| 5719 | $C42C3D | 67 | $30 -> $0030 | $0580 $0640 | $C49896 | not reached before trace end |
| 5719 | $C42C43 | 67 | $4A -> $004A | $0780 $07C0 | $C4989C | not reached before trace end |
| 5719 | $C42C49 | 67 | $30 -> $0030 | $0E00 $0200 | $C49A70 | not reached before trace end |
| 5719 | $C42C4F | 67 | $46 -> $0046 | $0540 $0800 | $C49B30 | not reached before trace end |
| 5719 | $C42C55 | 67 | $22 -> $0022 | $0880 $0380 | $C49B36 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, descriptor `+8` field, and three signed emitted placement words. The generic route uses that field as `$C45A36`, but type-specific gates can bypass it. These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
