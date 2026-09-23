# M-map transform-source trace comparison

An unchanged source set only characterizes these bounded stable-map windows. It does not establish whole-map coverage, terrain identity, or LOD behavior.

Baseline: `build\run035_m_map_stable_20f_trace\trace.jsonl` — 201,858 instructions, 12 `$C1F4AC` entries.
Extended: `build\run037_m_map_stable_20f_trace\trace.jsonl` — 198,208 instructions, 6 `$C1F4AC` entries.

| Immutable source | baseline entries | extended entries |
| --- | ---: | ---: |
| `$C35BAA` | 2 | 1 |
| `$C35BC2` | 2 | 1 |
| `$C35BDE` | 2 | 1 |
| `$C36220` | 4 | 2 |
| `$C3B720` | 2 | 1 |

New sources in extended window: none.
