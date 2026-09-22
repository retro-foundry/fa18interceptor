# Partial run003 map-mode geometry export

Triples are immutable local transform inputs, not global world coordinates. Line records are bounded to the next control-walker entry; the final component's interval ends at the trace cap and can include later unrelated line work. Polygon route counts are bounded to each wrapper's observed return. This is a partial renderer-input export, not the complete terrain map, a global placement table, or LOD data.

| Transform input | Local triples | Control entry | Line records | Polygon line/span routes |
| --- | --- | --- | ---: | ---: |
| `$C35BDE` | `[[11136, 0, -9088], [6656, 0, -3840], [-13568, 0, 2944]]` | `$C35BF0` | 2 | 0/0 |
| `$C35BAA` | `[[0, 0, 3744], [0, 416, -3168]]` | `$C35BB8` | 1 | 0/0 |
| `$C35BC2` | `[[0, 0, -3744], [0, 416, 3168]]` | `$C35BD0` | 1 | 0/0 |
| `$C36220` | `[[4096, 172, -5248], [-2816, 172, 1152], [-8960, 0, 2816]]` | `$C36232` | 2 | 0/0 |
| `$C3B720` | `[[2112, 0, -896], [-2080, 0, -2048], [-1600, 0, 1280], [1088, 0, 1568], [-640, 1024, 0]]` | `$C3B73E` | 9 | 2/2 |

The `$C3B720` component is the only entry in this export with an independently
observed face-slot mapping, so it is also available as a four-triangle
[minimal OBJ](../exports/c3b720_trace_proven_partial_component.obj). The other
entries remain input/control/line exports rather than guessed surface meshes.

`$C35BDE -> $C35BF0` additionally has a decoded static offset-pair list, so
its two selected local edges (`0 -> 1 -> 2`) are documented in the
[polyline component contract](c35bde_c35bf0_map_polyline_component.md).

`$C36220 -> $C36232` has the same decoded two-edge local polyline topology;
see the [three-triple line component](c36220_c36232_map_line_component.md).
