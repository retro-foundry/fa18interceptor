# Traced static-template workspace copies

Classification: **scenario-backed producer-to-consumer inventory**. The rows describe static bytes copied into mutable workspace cells; they are not an extracted terrain mesh, global position table, or LOD table.

Authority: `build/run033_placement_bulk_404_trace/trace.jsonl` and its frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; `$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.

The bounded trace has **37** observed static-entry copies.  **22** are later read at `$C1DD36` before the trace ends.  An absent later read means only that this bounded trace did not reach one; it is not rejection evidence.

`$C1D48C-$C1D496` transforms the source header byte rather than copying it directly: source bit 7 becomes destination word bit 15, while source bits 0..6 remain the low seven bits.  The two displayed source words are copied to the next four workspace bytes by `$C1D4BC`.

| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | $C42B3F | 67 | $58 -> $0058 | $0E00 $0480 | $C49AD0 | not reached before trace end |
| 1 | $C427C9 | 66 | $46 -> $0046 | $0B00 $0900 | $C4A970 | $C4F03A / $C22700 / (88, 0, 456) |
| 1 | $C427CF | 66 | $46 -> $0046 | $0150 $0200 | $C4A9D0 | $C4F052 / $C22700 / (-118, 0, 400) |
| 1 | $C427D5 | 66 | $4B -> $004B | $09A0 $0000 | $C4A9D6 | $C4F06A / $C22764 / (-51, 0, 384) |
| 1 | $C427DB | 66 | $6F -> $006F | $0400 $0400 | $C4AA30 | $C4F082 / $C22A34 / (-224, 0, 416) |
| 1 | $C427E1 | 66 | $4C -> $004C | $0588 $0A80 | $C4AB50 | $C4F09A / $C22778 / (-84, 0, 340) |
| 1 | $C427E7 | 66 | $73 -> $0073 | $0800 $0400 | $C4AB56 | $C4F0B2 / $C22A84 / (-64, 0, 288) |
| 1 | $C427ED | 66 | $74 -> $0074 | $0800 $0800 | $C4ACD0 | $C4F0CA / $C22A98 / (-64, 0, 192) |
| 1 | $C427B5 | 66 | $6C -> $006C | $0800 $0800 | $C4B090 | $C4F0E2 / $C229F8 / (-320, 0, 320) |
| 1 | $C427BB | 66 | $6D -> $006D | $0C00 $0400 | $C4B0F0 | $C4F0FA / $C22A0C / (-416, 0, 288) |
| 1 | $C427C1 | 66 | $6E -> $006E | $0800 $0800 | $C4B270 | $C4F112 / $C22A20 / (-448, 0, 192) |
| 1 | $C4274B | 66 | $61 -> $0061 | $0800 $0800 | $C4BA50 | $C4F12A / $C2291C / (-576, 0, 64) |
| 1 | $C42751 | 66 | $62 -> $0062 | $0400 $0000 | $C4BA56 | $C4F142 / $C22930 / (-608, 0, 0) |
| 1 | $C42757 | 66 | $63 -> $0063 | $0000 $0C00 | $C4BB70 | $C4F15A / $C22944 / (-512, 0, -32) |
| 1 | $C4275D | 66 | $64 -> $0064 | $0C00 $0C00 | $C4BBD0 | $C4F172 / $C22958 / (-544, 0, -32) |
| 1 | $C42763 | 66 | $65 -> $0065 | $0000 $0C00 | $C4BBD6 | $C4F18A / $C2296C / (-640, 0, -32) |
| 1 | $C42769 | 66 | $66 -> $0066 | $0C00 $0400 | $C4BC30 | $C4F1A2 / $C22980 / (-672, 0, -96) |
| 1 | $C4276F | 66 | $59 -> $0059 | $0C00 $0800 | $C4BC90 | $C4F1BA / $C2287C / (-288, 0, -192) |
| 1 | $C42775 | 66 | $67 -> $0067 | $0800 $0800 | $C4BDB0 | $C4F1D2 / $C22994 / (-704, 0, -192) |
| 1 | $C4277B | 66 | $68 -> $0068 | $0400 $0C00 | $C4BE10 | $C4F1EA / $C229A8 / (-352, 0, -288) |
| 1 | $C42781 | 66 | $60 -> $0060 | $0800 $0400 | $C4BE70 | $C4F202 / $C22908 / (-448, 0, -352) |
| 1 | $C4272F | 66 | $5D -> $005D | $0800 $0800 | $C4CC50 | not reached before trace end |
| 1 | $C42735 | 66 | $5E -> $005E | $0800 $0000 | $C4CCB0 | not reached before trace end |
| 1 | $C4273B | 66 | $5F -> $005F | $0400 $0FFF | $C4CE30 | not reached before trace end |
| 1 | $C42789 | 66 | $2A -> $002A | $0E00 $0E00 | $C4D250 | $C4F292 / $C224D0 / (-16, 0, 112) |
| 1 | $C4278F | 66 | $70 -> $0070 | $0800 $0800 | $C4D256 | $C4F2AA / $C22A48 / (-64, 0, 64) |
| 2 | $C42795 | 66 | $71 -> $0071 | $0800 $0800 | $C4D3D0 | not reached before trace end |
| 2 | $C4279B | 66 | $2C -> $002C | $06F6 $0E9A | $C4D3D6 | not reached before trace end |
| 2 | $C427A1 | 66 | $2B -> $002B | $00C4 $0AC8 | $C4D3DC | not reached before trace end |
| 2 | $C427A7 | 66 | $72 -> $0072 | $0C00 $0400 | $C4D430 | not reached before trace end |
| 2 | $C427AD | 66 | $5A -> $005A | $0800 $0800 | $C4D5B0 | not reached before trace end |
| 3 | $C429D1 | 67 | $56 -> $0056 | $0600 $0100 | $C49C50 | not reached before trace end |
| 3 | $C429D7 | 67 | $4A -> $004A | $0C00 $0400 | $C49DD0 | not reached before trace end |
| 3 | $C429DF | 67 | $2A -> $002A | $0800 $0800 | $C4AD90 | not reached before trace end |
| 3 | $C429E5 | 67 | $4A -> $004A | $0C00 $0400 | $C4B2D0 | not reached before trace end |
| 3 | $C4298B | 66 | $2D -> $002D | $0800 $0800 | $C4C170 | not reached before trace end |
| 3 | $C42991 | 66 | $2D -> $002D | $0700 $0740 | $C4C176 | not reached before trace end |

Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at `$C429D0-$C42C9F`.  The inventory records only source addresses reached by the captured path.  It does not claim that either entire segment is terrain data.

For every emitted row, the final tuple is the runtime record address, descriptor, and three signed emitted placement words.  These words are generated by the builder, not copied verbatim from the source words.  A zero middle word in this small joined sample is consistent with the wider runtime-placement diagnostic, but does not prove a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) plots only these 22 source-to-placement paths.

See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) for the instruction-level `$C427C1 -> $C4B270` example and [the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) for the downstream coordinate calculation.
