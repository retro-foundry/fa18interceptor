# Static terrain-template selector lattice

Classification: **controlled static directory decode**. This exports the static
template streams selected by every combination of the two already-proven 5-bit
origin inputs for one 41-call update packet. It is a selector/page map, not a
global-coordinate terrain mesh, heightmap, or LOD table.

The decoder combines the group-axis group-record pointer resolved at `$C1D406` with
the row-axis live-key sequence, rejects negative group headers, and accepts only
exact static row-key matches. Its bin-16
combination exactly reproduces all 41 observed stream/no-stream choices in the
independent runtime inventory.

All 1,024 selector-bin cells have been decoded. Their selected-stream counts range from 4 to 18; 
the companion SVG/PNG visualizes these counts and the JSON retains every selected static stream address.

The lattice's group/row labels are directory-input axes only. It does not establish
their cardinal orientation, physical spacing, full map extent, or whether every
static template stream represents terrain rather than another scene element.
