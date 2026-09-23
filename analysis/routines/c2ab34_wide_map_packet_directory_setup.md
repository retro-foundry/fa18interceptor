# `$C2AB34` wide M-map packet-directory setup

Classification: **byte-exact static layout initializer with scenario-backed downstream use**.

`$C2AB34-$C2AB59` writes the selector frame fields consumed by the byte-exact
`$C2AD80` lookup:

| Frame field | Value | Effect at `$C2AD80` |
| --- | ---: | --- |
| `-$3E(A6)` | `1` | selects the 64-byte row stride |
| `-$34(A6)` | `$C42E6C` | static relative-offset directory base |
| `-$40(A6)` | `8` | coordinate/bin shift used by shared setup |
| `-$30/-$2E(A6)` | `31/31` | selector row/column maxima |

It clears `-$44(A6)` and branches to the shared coordinate/bin setup at
`$C2AB7C`. Its adjacent sibling `$C2AB5A` instead writes mode zero, base
`$C42CA8`, shift `12`, and maxima `7/7`, establishing the normal 16-byte
layout. Thus the two observed directories are intentional layouts initialized
by code, rather than inferred from packet offsets.

The captured run035 map traces execute the wide layout; run003 and run037
also execute the normal sibling layout. Which caller/context selects a layout
is not yet a physical-distance or terrain-cell claim.

Authority: byte-exact [wide initializer](../../source_amiga/observed/initialize_wide_map_packet_directory.asm), the adjacent verified runtime bytes at `$C2AB5A`, and [selector stride samples](../data/m_map_selector_modes.md).
