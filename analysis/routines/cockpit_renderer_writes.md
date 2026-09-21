# Cockpit renderer writes

## Bounded trace

`build/attract_cockpit_1800_tenframe_trace/` restores the sealed frame-1800 attract snapshot with no future input and instruction-steps ten chipset-frame boundaries. It records 100,895 68000 instructions and 2,387 Custom-chip writes. The stepping host's existing limitation applies: it is evidence for this no-input interval, not proof of held-input equivalence.

The Copper continues to display the four buffers in [cockpit_bitplane_assets.md](../cockpit_bitplane_assets.md). No CPU write changes a BPLx pointer in this trace. The CPU instead programs blitter destination pointers inside those ranges.

| Observed CPU PC | Observed destination examples | Evidence-backed role |
| --- | --- | --- |
| `$C2FD8C` packet (trigger sites `$C2FDF0`, `$C2FE3A`, `$C2FE90`, `$C2FEDA`) | `$0189A8`, `$016A68`, `$014B28`, `$012BE8` | One blit each into planes 4, 3, 2, and 1 in frame 4. Disassembly establishes the packet entry at `$C2FD8C`: it loads the four base pointers from `$C456B6`, adds `$28`, and submits a common blit whose `BLTSIZE` comes from `$C45984`. The observed caller is `$C0D730`, reached from `$C0F05C`; `$C0D730` selects this packet when bit 13 of `$C458D2` is clear. The higher-level display purpose remains unassigned. |
| `$C306AE` | `$013C50`, `$012C0F`, `$012BE8` | Frame-5 blits into plane 1. The routine has no current exact source slice. |
| `$C2FBE6`, `$C2FC4E`, `$C2FCB6`, `$C2FD1C` | Repeating destinations in the plane-1 through plane-4 ranges | The four enabled-plane write sites inside `submit_blitter_line_to_active_planes`. |
| `$C30F6A` | `$019E4A`, `$017F0A`, `$015FCA`, `$01408A` | Four plane-family destinations from an unassigned caller at `$C30E..`; they are outside the Copper-visible four planes during this capture. |

The exact `submit_blitter_line_to_active_planes` source reads its four base pointers from `$C456B6`, adds a prepared byte offset, waits for the blitter, and writes `BLTCPT` and `BLTDPT` before `BLTSIZE`. The trace confirms active display-plane destinations at its four write sites. This connects the previously identified projected-segment emitter to the cockpit renderer without assigning the line data to a particular cockpit object.

## Stable and variable regions

Full-frame deterministic captures at 1800, 1801, and 1802 have byte-identical active plane data. At 1810, only small regions differ from 1800:

| Plane | Changed bytes | Changed display rows |
| --- | ---: | --- |
| 1 | 203 | 102–106, 122, 124 |
| 2 | 163 | 103–106, 122, 124 |
| 3 | 19 | 115–117, 120, 122, 126–128, 134, 137, 159–161, 175 |
| 4 | 16 | 85–88, 122, 124, 183–187 |

This establishes that most cockpit pixels persist over the interval. It does not prove that every persistent pixel comes from a single static asset or identify the blits responsible for panel initialization.


### Active plane table order

Attract frame 1800 shows that `$C456B6` contains four pointers to renderer tables (`$C4566E`, `$C4568E`, `$04DB30`, `$04FA70`), rather than the Copper plane bases themselves. `$C2FD8C` dereferences the selected table before adding `$28`; the final destination values in the Custom trace establish the affected Copper-plane ranges. The exact table-to-plane mapping remains pending.


### `$C456B6` live value at packet execution

The `$C2FD8C` instruction rows at chipset frame 4 resolve `A2 = $C4567E`; its dereferenced values yield `$0189A8` then `$016A68` at the first two trigger sites. Thus `$C456B6` was dynamically updated before the packet. The Custom log and CPU register rows jointly prove active Copper-plane destinations; endpoint snapshots alone are insufficient for this mutable table.

### Selected cockpit plane-table contents

The pointer publisher's live `$C456B6 = $C4567E` selection identifies a four-longword table. Endpoint frame-1800 RAM shows `$C4567E` contains `$018980`, `$016A40`, `$014B00`, `$012BC0`, in the `$C2FD8C` read order. The packet adds `$28`, producing the observed `$0189A8`, `$016A68`, `$014B28`, and `$012BE8` destinations: Copper planes 4, 3, 2, and 1 respectively.

## Reconstruction boundary

`$C2FD8C` through its fourth `BLTSIZE` trigger at `$C2FEDA` is the measured four-plane packet. The next instruction at `$C2FEDE` begins a separate state-preservation and helper-call sequence, so it must not be merged into the packet source until a return-bounded trace proves the enclosing function boundary.
