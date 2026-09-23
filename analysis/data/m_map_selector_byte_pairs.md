# Observed M-map selector byte pairs

Classification: **scenario-backed selector-input subtable**.

Each pair is read by the byte-exact C2AD80 selector and added to live row/column minima before bounded relative-offset lookup. This is an observed subtable only; it does not establish the complete C29F00 table, global coordinates, or terrain identity.

`$C2AD80` observes 25 selector values at `$C29F00`; the JSON retains every hit.
Their distinct pairs are the complete `{-2..2} × {-2..2}` lattice: True.

| Selector | table bytes | signed pair | observations |
| ---: | --- | --- | ---: |
| 0 | `$C29F00` | (1, 1) | 5 |
| 1 | `$C29F02` | (0, 1) | 5 |
| 2 | `$C29F04` | (-1, 1) | 3 |
| 3 | `$C29F06` | (1, 0) | 5 |
| 4 | `$C29F08` | (0, 0) | 9 |
| 5 | `$C29F0A` | (-1, 0) | 6 |
| 6 | `$C29F0C` | (1, -1) | 4 |
| 7 | `$C29F0E` | (0, -1) | 6 |
| 8 | `$C29F10` | (-1, -1) | 6 |
| 9 | `$C29F12` | (2, 2) | 2 |
| 10 | `$C29F14` | (1, 2) | 2 |
| 11 | `$C29F16` | (0, 2) | 2 |
| 12 | `$C29F18` | (-1, 2) | 2 |
| 13 | `$C29F1A` | (-2, 2) | 2 |
| 14 | `$C29F1C` | (2, 1) | 4 |
| 15 | `$C29F1E` | (-2, 1) | 2 |
| 16 | `$C29F20` | (2, 0) | 5 |
| 17 | `$C29F22` | (-2, 0) | 3 |
| 18 | `$C29F24` | (2, -1) | 5 |
| 19 | `$C29F26` | (-2, -1) | 3 |
| 20 | `$C29F28` | (2, -2) | 5 |
| 21 | `$C29F2A` | (1, -2) | 5 |
| 22 | `$C29F2C` | (0, -2) | 5 |
| 23 | `$C29F2E` | (-1, -2) | 5 |
| 24 | `$C29F30` | (-2, -2) | 3 |
