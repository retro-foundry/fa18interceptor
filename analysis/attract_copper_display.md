# Attract Copper display list at `$00A400`

Authority: `build/attract_focus_600/chip.bin` and
`build/attract_focus_600/custom_writes.jsonl`, hardware frame 602. The CPU
writes COP1LC to `$0000A400` at VPOS 1. The Copper subsequently executes the
listed bitplane pointer commands at VPOS 41; its source PCs are Chip-RAM
addresses `$00A468-$00A484`.

| Copper list address | Register | Value | Observed role |
| ---: | --- | ---: | --- |
| `$00A444` | DIWSTRT | `$0581` | Display-window start programming. |
| `$00A448` | BPLCON0 | `$0200` | Initial bitplane-control value. |
| `$00A44C` | BPLCON2 | `$0024` | Bitplane-control value. |
| `$00A450` | DIWSTOP | `$40C1` | Display-window stop programming. |
| `$00A454/$00A458` | DDFSTRT/DDFSTOP | `$0038/$00D0` | Display-fetch bounds. |
| `$00A468/$00A46C` | BPL1PTH/PTL | `$0001/$2BC0` | Pointer `$00012BC0`. |
| `$00A470/$00A474` | BPL2PTH/PTL | `$0001/$4B00` | Pointer `$00014B00`. |
| `$00A478/$00A47C` | BPL3PTH/PTL | `$0001/$6A40` | Pointer `$00016A40`. |
| `$00A480/$00A484` | BPL4PTH/PTL | `$0001/$8980` | Pointer `$00018980`. |
| `$00A48C` | BPLCON0 | `$4200` | Later display-enable/control value. |
| `$00A494` | BPLCON0 | `$0200` | Later restore value. |
| `$00A498` | Copper end | `$FFFF,$FFFE` | End wait. |

The four pointers have an `$1F40` byte stride. This proves four Copper-loaded
display planes in this attract frame. It does not establish buffer ownership,
resolution, or whether the plane data is a full screen versus a subregion.

The list is deliberately not represented in `source_amiga/observed`: it is
mutable Chip-RAM data. Its palette bytes differ between the attract snapshot and
the baseline snapshot used by the static-source verifier. The hardware trace
and this table remain the authority for this frame.
