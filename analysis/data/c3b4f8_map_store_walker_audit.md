# `$C3B4F8` map descriptor-store / walker audit

Classification: **repeated bounded negative transition evidence**.

Across the listed sealed map traces, `$C1CC70` reads `$C3B4F8` from descriptor field `$C22708`, but no subsequent observed `$C1F6F8` entry receives `$C3B4F8`. A missing walker is retained as a trace boundary, not a rejection claim. This does not prove the target can never reach the walker.

| Trace | frame | store index | next walker |
| --- | ---: | ---: | --- |
| `build/run003_m_map_appearance_trace/trace.jsonl` | 9 | 89009 | frame 9, `$C3B73E` |
| `build/run003_m_map_appearance_trace/trace.jsonl` | 10 | 96691 | trace ends |
| `build/run003_m_map_appearance_trace/trace.jsonl` | 10 | 96858 | trace ends |
| `build/run035_m_map_stable_1f_trace/trace.jsonl` | 1 | 8217 | trace ends |
| `build/run035_m_map_stable_20f_trace/trace.jsonl` | 2 | 12289 | frame 15, `$C35BF0` |
| `build/run035_m_map_stable_20f_trace/trace.jsonl` | 2 | 12479 | frame 15, `$C35BF0` |
| `build/run035_m_map_stable_20f_trace/trace.jsonl` | 15 | 147941 | frame 15, `$C35BF0` |
| `build/run037_c3b4f8_descriptor_target_trace/trace.jsonl` | no chipset-frame field | 0 | frame not recorded, `$C3B73E` |
| `build/run037_c3b4f8_descriptor_target_trace/trace.jsonl` | no chipset-frame field | 8164 | trace ends |
| `build/run037_c3b4f8_descriptor_target_trace/trace.jsonl` | no chipset-frame field | 8331 | trace ends |
