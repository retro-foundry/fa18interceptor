# Default scene to map-parent composition

`default_scene_map_pass.{c,h}` joins two already bounded source routes without
claiming their scheduler or page owner:

```
$C2DB18 -> $C1C54E/$C1C636 -> $C2AA9C -> $C2AB34...$C2AFF9
```

The default scene pass resolves the active `$C46184 + $C458DE.w` record,
publishes `$C45BD8`, and creates the projection packet whose full
`depth_metric` corresponds to `$C45A78`.  The map bridge then copies only
source-owned values into a caller-supplied map-parent template:

- active record `+$14`, `+$18`, and `+$1C` become the ordinary map coordinate
  components read at `$C2AB8C` when its directory-selector gate is clear;
- the default route's second matrix (`$C45BD8`) becomes the map packet
  transform matrix; and
- the projection packet becomes the map depth-stage input.

The parent-prepared `$C1C6BC` selector pack is also consumed directly:
`$C45785` selects the ordinary-root or alternate-origin coordinate triple,
`$C45C3E/$C45C42/$C45C46` supply the latter, and `$C45850` supplies the map
low-filter/column-table selector.

The template deliberately retains the static Hunk binding, detail fields,
filter row resolver, workspace header, and native page submission. Those are
live parent/page state not recovered by this local composition.

The contract uses a source-shaped active record whose `$C1C54E` seed yields
depth `-125`, reproducing the frame-382 `$C2AA9C` low-metric branch. It proves
that the composed wide pass reaches the display callback after transferring
the root/matrix/depth owners. It is not scheduled by `game.c`.
