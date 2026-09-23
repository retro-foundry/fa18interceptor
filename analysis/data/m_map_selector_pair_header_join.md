# M-map local selector pair to packet-header join

Classification: **scenario-backed local selection join**.

The join searches only until the next C2AD80 selector entry. A missing packet header means this bounded trace returned/rejected before C2AEFC; it is not a claim that the local offset has no map content. Pair components remain local selector offsets, not global axes.

77 of 104 `$C2AD80` entries reach `$C2AEFC` before the next selector entry.

| Selector | signed pair | packet headers | no-header entries |
| ---: | --- | --- | ---: |
| 0 | (1, 1) | `$C42E3A` (1), `$C43EC6` (2), `$C43F5A` (2) | 0 |
| 1 | (0, 1) | `$C43E20` (2), `$C43EC6` (2) | 1 |
| 2 | (-1, 1) | `$C43D80` (2) | 1 |
| 3 | (1, 0) | `$C42D24` (1), `$C43BA8` (2), `$C43CBA` (2) | 0 |
| 4 | (0, 0) | `$C42D24` (2), `$C42DF8` (2), `$C43B30` (2), `$C43BA8` (3) | 0 |
| 5 | (-1, 0) | `$C42DF8` (2) | 4 |
| 6 | (1, -1) | `$C42E16` (2), `$C43A74` (2) | 0 |
| 7 | (0, -1) | `$C42DC0` (2), `$C42E16` (2), `$C439E4` (2) | 0 |
| 8 | (-1, -1) | `$C42DC0` (2) | 4 |
| 9 | (2, 2) | none | 2 |
| 10 | (1, 2) | `$C4409A` (2) | 0 |
| 11 | (0, 2) | `$C44078` (2) | 0 |
| 12 | (-1, 2) | none | 2 |
| 13 | (-2, 2) | `$C44062` (2) | 0 |
| 14 | (2, 1) | `$C42E4E` (2), `$C43F5A` (2) | 0 |
| 15 | (-2, 1) | `$C43D30` (2) | 0 |
| 16 | (2, 0) | `$C42E4E` (2), `$C43CBA` (2), `$C43D16` (1) | 0 |
| 17 | (-2, 0) | none | 3 |
| 18 | (2, -1) | `$C42E4E` (2), `$C43B16` (3) | 0 |
| 19 | (-2, -1) | none | 3 |
| 20 | (2, -2) | `$C42E4E` (2), `$C439C8` (3) | 0 |
| 21 | (1, -2) | `$C42E4E` (2), `$C439A4` (2), `$C439C8` (1) | 0 |
| 22 | (0, -2) | `$C42E4E` (2), `$C43990` (2), `$C439A4` (1) | 0 |
| 23 | (-1, -2) | `$C43990` (1) | 4 |
| 24 | (-2, -2) | none | 3 |
