# M-map directory initializer sequence

Classification: **scenario-backed initializer ordering**.

An adjacent normal/wide event pair proves ordering only within the bounded trace. It establishes sequential renderer preparation passes, not terrain ownership, physical map extent, or LOD.

Observed adjacent normal-to-wide initializer pairs: 3.

| Trace | ordered initializer events |
| --- | --- |
| `build\run003_m_map_appearance_trace\trace.jsonl` | wide (frame 2) |
| `build\run035_m_map_appearance_13f_trace\trace.jsonl` | normal (frame 8859), wide (frame 8860), normal (frame 8864), wide (frame 8865) |
| `build\run035_m_map_stable_1f_trace\trace.jsonl` |  |
| `build\run037_m_map_stable_13f_trace\trace.jsonl` | normal (frame 5690), wide (frame 5691) |
