# Active terrain-template stream records

Classification: **scenario-backed static stream-to-template-record inventory**. Each row joins a selected `$C1D3F4` stream to the exact `$C1D488` source records that `$C1D442-$C1D4C2` copied into a mutable workspace. It is not a complete global map or raw vertex export.

Authority: `build/run037_flight_pre_map_selector_trace/trace.jsonl` and its `slow.bin`. The stream address is captured at `$C1D442`; each listed source is captured before the `(A5)+` header read at `$C1D488`.

The bounded control window has **0** selected streams that copy **0** template records.

| workspace band | group selector / row key | static stream | copied records | source records (`header`, `word_1`, `word_2`) |
| --- | --- | --- | ---: | --- |

The same static stream can be selected for different workspace bands, and the template words remain inputs to the placement builder rather than direct global coordinates. The inventory identifies cell-content candidates only for this sealed origin window; it must not be extrapolated into a complete terrain export.
