# Segment-65 workspace-band selector stream

Classification: **scenario-backed static-control to group-selector dataflow**. The byte stream controls which static template groups are considered for mutable workspace bands; it is not a decoded world grid, world coordinate table, or LOD table.

Authority: `build/run033_placement_bulk_404_trace/trace.jsonl`.  `$C1D338` reads a byte from the byte-stable, relocation-free segment-65 control stream.  Its local path maps through `$C411F0` and `$C1D764`, applies the observed live bounds, and on success calls `$C1D3F4` at `$C1D3B4`.  The call-time `D0` is the static-group selector index later used with `$C42390`; `D1` remains a live row term.

The bounded trace reads **29** control items; **27** reach the group selector.

| Frame | static control byte | byte value | workspace band | reaches group selector | selector index | live row term |
| ---: | --- | --- | --- | --- | --- | --- |
| 1 | $C412EC | $09 | $C48390 | True | $0012 | $0011 |
| 1 | $C412F0 | $0A | $C48990 | True | $0012 | $0010 |
| 1 | $C412F9 | $0B | $C48F90 | True | $0012 | $000F |
| 1 | $C412FC | $0C | $C49590 | True | $0011 | $0012 |
| 1 | $C412FF | $0E | $C49B90 | True | $0010 | $0012 |
| 1 | $C41303 | $00 | $C4A190 | True | $0011 | $0011 |
| 1 | $C41314 | $01 | $C4A790 | True | $0011 | $0010 |
| 1 | $C41326 | $02 | $C4AD90 | True | $0011 | $000F |
| 1 | $C41336 | $03 | $C4B390 | True | $0010 | $0011 |
| 1 | $C41348 | $05 | $C4B990 | True | $0010 | $000F |
| 1 | $C41359 | $06 | $C4BF90 | True | $000F | $0011 |
| 1 | $C41360 | $07 | $C4C590 | True | $000F | $0010 |
| 1 | $C4136D | $08 | $C4CB90 | True | $000F | $000F |
| 1 | $C41372 | $04 | $C4D190 | True | $0010 | $0010 |
| 2 | $C41384 | $FF | $C4D790 | False |  |  |
| 3 | $C41256 | $09 | $C48390 | True | $0041 | $0041 |
| 3 | $C4125C | $0A | $C48990 | True | $0041 | $0042 |
| 3 | $C41264 | $0C | $C48F90 | True | $0042 | $0040 |
| 3 | $C4126A | $0E | $C49590 | True | $0043 | $0040 |
| 3 | $C41272 | $00 | $C49B90 | True | $0042 | $0041 |
| 3 | $C41284 | $01 | $C4A190 | True | $0042 | $0042 |
| 3 | $C41296 | $02 | $C4A790 | True | $0042 | $0043 |
| 3 | $C412A1 | $03 | $C4AD90 | True | $0043 | $0041 |
| 3 | $C412B3 | $05 | $C4B390 | True | $0043 | $0043 |
| 3 | $C412C0 | $06 | $C4B990 | True | $0044 | $0041 |
| 3 | $C412CA | $07 | $C4BF90 | True | $0044 | $0042 |
| 3 | $C412D6 | $08 | $C4C590 | True | $0044 | $0043 |
| 3 | $C412D9 | $04 | $C4CB90 | True | $0043 | $0042 |
| 3 | $C412EB | $FF | $C4D190 | False |  |  |

The same raw control-byte range yields different selector-index ranges in the two observed update phases, while the per-call row term is live state.  That establishes a static-control plus live-state selection stage, not a distance formula or a map-cell coordinate system.  See [static selector groups](static_template_selector_groups.md) for the next static-table lookup.
