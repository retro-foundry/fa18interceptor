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

The bounded run035 and run037 traces show the normal sibling executing first,
then this wide initializer in the next frame. They are therefore sequential
map-render preparation passes in those scenarios, not mutually exclusive map
modes. Their caller/context remains neither a physical-distance nor a
terrain-cell claim. See the [initializer sequence](../data/m_map_directory_initializer_sequence.md).

After either initializer, shared setup at `$C2ABD8` branches on the mode.
For the wide pass, `$C2AC1A` chooses a control-stream base from the depth
metric at `-$28(A6)`: values above `$10000` choose `$C2A0C2`, values in
`($9000,$10000]` choose `$C2A072`, and values at or below `$9000` enter the
further record-filter path at `$C2AC3E`. The run035 appearance trace takes
the latter low-metric branch twice. This proves a depth-threshold control
stream selector within the wide pass, but only one threshold branch is
scenario-observed here; it is not a physical-world distance or mesh-LOD claim.

Authority: byte-exact [wide initializer](../../source_amiga/observed/initialize_wide_map_packet_directory.asm), byte-exact [normal initializer](../../source_amiga/observed/initialize_normal_map_packet_directory.asm), and [selector stride samples](../data/m_map_selector_modes.md).
