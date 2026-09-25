# Run031 M-map selector-origin write: no map-pan effect

Classification: **controlled negative result**.

The Alcatraz-window M-map state was restored three times and run for 60
no-input frames. The treatment writes `$C45C3E` before the first frame, once
with `$129BE87C` and once with `$309BE87C`; the control writes nothing.

All three final video hashes are identical:

```text
4ba6d1c885227eee96c2bd5d43372bcaa0fa78ee40c65ff09d7f697d2b6c6cf8
```

Exact-colour comparisons between each treatment and the control have zero
translation and 100% agreement over 113,920 blue-water pixels. The attempted
`$C45C3E` write is therefore overwritten before this stable map renderer
draws. It cannot serve as a user-map pan control or create new map coverage
from this saved state.

This is consistent with `$C45C3E` being live terrain-selector origin state;
it does not identify its producer or eliminate other map-origin controls.

Authorities: ignored controls
`build/run031_frame13200_map_origin_{control_60,x_plus_probe,x_plus_0x20_probe}/`
and their exact-colour comparison JSON files.
