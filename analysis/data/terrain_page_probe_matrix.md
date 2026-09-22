# Terrain page-selector probe matrix

Classification: **controlled selector-causality summary**. Each probe restores the same state and changes only named origin-bin values at the same breakpoint. This is not a complete world-map extent or LOD table.

| bins | perturbation | selector calls | selected streams | group records | static copies | later builder reads |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| (16,16) | unchanged | 41 | 16 | 9 | 106 | 91 |
| (17,16) | first component +1 | 41 | 10 | 7 | 60 | 53 |
| (16,17) | second component +1 | 41 | 14 | 7 | 95 | 79 |
| (17,17) | both components +1 | 41 | 10 | 7 | 60 | 35 |
| (0,16) | first component zero | 41 | 4 | 3 | 6 | 4 |
| (16,0) | second component zero | 41 | 4 | 3 | 6 | 4 |
| (16,8) | second component 8 | 41 | 4 | 3 | 6 | 4 |
| (16,12) | second component 12 | 41 | 5 | 4 | 10 | 4 |
| (16,14) | second component 14 | 41 | 11 | 8 | 58 | 10 |
| (8,16) | first component 8 | 41 | 6 | 5 | 13 | 7 |
| (12,16) | first component 12 | 41 | 8 | 7 | 18 | 14 |
| (255,16) | first component 255 | 41 | 4 | 3 | 6 | 4 |

The control window selects 16 streams. The independent +1 probes demonstrate the two directory axes, while all three sampled outer bins retain the same four-stream subset. This supports a bounded active page window but cannot establish a global map edge, coordinate scale, or LOD scheme without more origin bins and a distance-controlled renderer experiment.

Per-probe record and group inventories remain the authority; see [the origin mutation probe](origin_selector_mutation_probe.md).
