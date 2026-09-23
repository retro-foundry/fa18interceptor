# Complete wide M-map packet directory

Classification: **byte-decoded 32×32 static relative-offset directory**.

The dimensions and base are set explicitly by byte-exact C2AB34. A positive relative offset passes C2ADCE's immediate offset test; later target-word and packet-stage branches depend on live state, so this is a complete static directory decode, not a complete rendered terrain map, global coordinate grid, or LOD table.

`$C42E6C-$C4366B` contains 1024 cells.

| cell class | count |
| --- | ---: |
| negative_target_word_guard | 526 |
| nonnegative_target_word | 498 |

The JSON companion retains all 1024 coordinates, table words, and resolved targets. It is intentionally not rendered as a world map because selector x/y are local directory indices, not established world axes.
