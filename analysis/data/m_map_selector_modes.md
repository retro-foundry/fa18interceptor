# M-map selector stride modes

Classification: **scenario-backed frame-local selector mode inventory**.

Mode is the signed word at -$3E(A6) read by the byte-exact selector. Zero chooses the 16-byte row stride and non-zero chooses the 64-byte stride. The paired snapshot supplies the frame-local word; this does not map either layout to global terrain coordinates or LOD.

104 `$C2ADCE` visits were decoded from paired trace/snapshot inputs.

| trace | base | mode | row stride | visits |
| --- | --- | ---: | ---: | ---: |
| `build\run003_m_map_appearance_trace\trace.jsonl` | `$C42CA8` | 0 | 16 | 12 |
| `build\run003_m_map_appearance_trace\trace.jsonl` | `$C42E6C` | 0 | 16 | 25 |
| `build\run035_m_map_appearance_13f_trace\trace.jsonl` | `$C42CA8` | 1 | 64 | 8 |
| `build\run035_m_map_appearance_13f_trace\trace.jsonl` | `$C42E6C` | 1 | 64 | 9 |
| `build\run035_m_map_stable_1f_trace\trace.jsonl` | `$C42E6C` | 1 | 64 | 9 |
| `build\run037_m_map_stable_13f_trace\trace.jsonl` | `$C42CA8` | 0 | 16 | 16 |
| `build\run037_m_map_stable_13f_trace\trace.jsonl` | `$C42E6C` | 0 | 16 | 25 |
