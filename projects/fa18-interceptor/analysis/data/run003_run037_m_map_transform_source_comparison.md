# M-map transform-source trace comparison

An unchanged source set only characterizes these bounded stable-map windows. It does not establish whole-map coverage, terrain identity, or LOD behavior.

Baseline: `build\run003_m_map_appearance_trace\trace.jsonl` — 135,244 instructions, 6 `$C1F4AC` entries.
Extended: `build\run037_m_map_stable_13f_trace\trace.jsonl` — 133,001 instructions, 6 `$C1F4AC` entries.

| Immutable source | baseline entries | extended entries |
| --- | ---: | ---: |
| `$C35BAA` | 1 | 1 |
| `$C35BC2` | 1 | 1 |
| `$C35BDE` | 1 | 1 |
| `$C36220` | 2 | 2 |
| `$C3B720` | 1 | 1 |

New sources in extended window: none.
