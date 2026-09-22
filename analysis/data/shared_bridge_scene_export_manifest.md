# Shared bridge-scene component export manifest

[`shared_bridge_scene_export_manifest.json`](shared_bridge_scene_export_manifest.json)
is the machine-readable handoff for the two local components proven in the
run033 red Golden Gate window.  Each entry names only its static triple payload
and renderer-observed topology file.

The manifest intentionally excludes `$C45BEA`, `$C48390-$C4E76B`, `$C4BF90`,
and `$C4E910`: those are mutable display, transform, or projection workspaces.
It also does not promote the components into a complete Golden Gate mesh—the
scene can reuse them across bridge instances and no unseen faces are inferred.
