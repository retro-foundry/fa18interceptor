# run037 M-map control-component repeat

Classification: **second-position repeat of bounded immutable map-display
components**. These are renderer-control components, not identified terrain
cells, coastline ownership, or a 3D terrain mesh.

From the run037 stable-map checkpoint, the template-derived descriptor field
`$C35BD0` is observed at `$C1CC70` and leads into the map control sequence.
A source-bounded matrix capture then reaches three static inputs at `$C1F4AC`:

| static source | source triples | line context | bounded output |
| --- | --- | --- | --- |
| `$C35BAA` | `(0,0,3744)`, `(0,416,-3168)` | `$C3559A` | one transformed line |
| `$C35BC2` | `(0,0,-3744)`, `(0,416,3168)` | `$C355D2` | one transformed line |
| `$C36220` | three static triples | `$C36216` | two transformed lines |

Each source uses matrix `$C45BD8`, writes through the mutable `$C48390`
transform workspace, and reaches `$C212B0` followed by `$C2FA7E`. The first
two are the same `$C35BAA/$C35BC2` two-triple control components already
observed during run003 map preparation, and `$C36220` is the previously
identified map line component. Their recurrence after a different ordinary
flight position establishes reusable map-display control data, not spatial
terrain payload.

Authority: sealed `captures/run037`; source-bounded capture
`build/run037_m_map_control_component_geometry/instance_geometry.json` from
the stable map checkpoint. The collector keeps only geometry reached between
consecutive `$C1F4AC` entries; its line triples are mutable renderer
workspaces and are not exported as immutable vertices.
