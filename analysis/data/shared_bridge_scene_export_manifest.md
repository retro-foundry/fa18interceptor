# Shared scene-component export manifest

[`shared_bridge_scene_export_manifest.json`](shared_bridge_scene_export_manifest.json)
is the machine-readable handoff for one local scene component proven in
the run033 red Golden Gate window.  Each entry names only its static triple payload
and renderer-observed topology file.

The manifest intentionally excludes `$C45BEA`, `$C48390-$C4E76B`, `$C4BF90`,
and `$C4E910`: those are mutable display, transform, or projection workspaces.
It does not promote the component into a Golden Gate mesh: it can be reused
across scenes and no unseen faces are inferred. `$C3B720` is deliberately
excluded after visual identification as a terrain / mountain candidate; the
same possibility remains open for `$C3B588`.
