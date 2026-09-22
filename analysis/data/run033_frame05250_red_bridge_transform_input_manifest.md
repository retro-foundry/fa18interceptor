# Run033 frame-5,250 red Golden Gate transform inputs

This is the portable immutable-input inventory for the sealed run033
frame-5,250 checkpoint, where the user identifies the red on-screen structure
as the Golden Gate bridge.  The machine-readable payload is
[the JSON manifest](run033_frame05250_red_bridge_transform_input_manifest.json).

It records only direct six-byte source triples bounded by the next `$C1F6F8`
walker pointer.  It deliberately excludes `$C48390` and all other transformed
workspace addresses.  Each `next_pointer` is code/control data, not another
vertex.

The inventory is a **data-location and separation** artifact, not a claim that
every active packet is bridge geometry.  In particular `$C3B720` is the
separately traced terrain/mountain component and is explicitly excluded from a
Golden Gate export.  The red image establishes the scene context; source to
renderer-context traces establish mesh membership.

Boundary authorities are collected in
[the Golden Gate source packet inventory](c1f4ac_golden_gate_source_packet_inventory.md).
