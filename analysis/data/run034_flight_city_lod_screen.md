# run034 flight-to-city/retreat screen

Classification: **replay-backed visual anchors plus no-input renderer input
samples**. This is a bounded test for a distance-related static-family change;
it is not a terrain export, global-coordinate recovery, or an LOD proof.

## Authority

`captures/run034` is sealed with 538 recorded events through replay frame
10,477. The user created an Engine9000 GUI save at frame 4,331 after takeoff.
`scripts/extract_e9k_snapshot_state.py` extracts its serialized state without
modifying the capture; a one-frame restore accepted it and produced a flight
screen.

The ordinary replay contact sheet at
`build/run034_keyframes/contact_sheet.png` supplies these visual anchors:

| Replay frame | Visual anchor |
| ---: | --- |
| 4,200 / 4,331 | open water immediately before / at the saved checkpoint |
| 6,000 | broad coastline/land horizon |
| 8,000 | densest sampled coastline/land view |
| 9,000--10,477 | simpler coastline after the turn away |

At frames 4,200, 6,000, 8,000, and 9,500, `$C1C860` was replayed normally
to its first post-arm hit, then instruction-stepped only to `$C0F048` with
future human input suppressed. The four intervals were 606, 167, 854, and
216 instructions respectively. None executed `$C1D3F4`, `$C1D442`, `$C1D488`,
or `$C1DD36`. Therefore that immediate `$C1C860` return slice is not the
terrain page-load/copy window for these samples. This negative result limits
the test; it does not show that the city has no static terrain source.

## Renderer-input comparison

Full replay checkpoints at 4,200, 6,000, 8,000, and 9,500 were each restored
and advanced without further input to the next `$C0F090` update entry. The
first 30,000 instructions supplied the following `$C1F4AC/$C1F524`
matrix-input samples (the JSON files beside this report retain every row):

| Checkpoint | Distinct static input addresses |
| --- | --- |
| far 4,200 | `$C3AD0E`, `$C3B720`, `$C3B726`, `$C3BE4C` |
| city 6,000 | `$C35BAA`, `$C36220`, `$C36226`, `$C36410` |
| near 8,000 | `$C35BC2`, `$C363EC`, `$C3AD0E`, `$C3AD14`, `$C3B720`, `$C3B726` |
| retreat 9,500 | `$C35C4A`, `$C35C50`, `$C35DD6`, `$C35DDC`, `$C3BE4C`, `$C3BE52` |

The common `$C3513C` record prefix is also observed in every checkpoint
except that the near sample ends its 30,000-instruction window earlier in that
prefix. These input sets establish changing static renderer context across the
flight path. They do **not** establish LOD: distance, world position, heading,
and culling all change between checkpoints, and no traced selector has tied a
measured distance to a replacement mesh/face family.

## Grey-city overlay lead

The user's visual observation that grey city shapes can lie flat over the
ground is compatible with the established flat placement layer, but it does
not identify a static source by itself. A no-input trace bounded from the city
checkpoint's `$C36410` matrix entry reaches the next `$C1F4AC` entry after 708
instructions without entering `$C2469E`, `$C2FF48`, `$C212B0`, or `$C2FA7E`.
The existing source-bounded collectors independently record zero polygons and
zero lines for `$C36410`; `$C35BAA` likewise records zero direct primitives in
its interval. Consequently neither is assigned to the visible grey city
overlay. They remain renderer input/control candidates only.

The next city-specific test is to correlate the grey raster with a projected
primitive or span submission, then trace that primitive back to its static
source and placement record. That will distinguish a flat city footprint from
a co-visible 3D building/landmark component.

An initial replay-preserved `$C2FF48` census armed at frame 8,000 records four
finalized polygon tuples. One has static `A5=$C3B6B0`; the other three have
`$FFFFFFF4` or `$FFFFFFE8` mutable/negative contexts. Their stored screen-pair
bounds are respectively `x=109..133,y=79..100`, `x=18..31,y=51..69`,
`x=0..20,y=41..51`, and `x=28..260,y=69..179`. The tuples are useful
screen-space leads, but they are not a grey-city attribution: debugger
single-stepping advances subsequent host frames, the recorded workspaces are
mutable, and no tuple has yet been compared against an exact grey raster mask.

## Result

run034 is a valid and materially better LOD/map investigation capture: it
contains one same-flight approach and retreat with a durable after-takeoff
checkpoint. It adds no confirmation of LOD. The next decisive pass must hold
one landmark and heading stable while varying distance, then follow the
associated selector through its chosen static face/model family.
