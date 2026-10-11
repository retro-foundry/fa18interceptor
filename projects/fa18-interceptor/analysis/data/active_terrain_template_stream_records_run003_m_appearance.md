# Active terrain-template stream records

Classification: **scenario-backed static stream-to-template-record inventory**. Each row joins a selected `$C1D3F4` stream to the exact `$C1D488` source records that `$C1D442-$C1D4C2` copied into a mutable workspace. It is not a complete global map or raw vertex export.

Authority: `build/run003_m_map_appearance_trace/trace.jsonl` and its `slow.bin`. The stream address is captured at `$C1D442`; each listed source is captured before the `(A5)+` header read at `$C1D488`.

The bounded control window has **6** selected streams that copy **71** template records.

| workspace band | group selector / row key | static stream | copied records | source records (`header`, `word_1`, `word_2`) |
| --- | --- | --- | ---: | --- |
| $C48990 | $000F / $000F | $C42ADA | 8 | $C42ADB ($4F, $0A80, $0600); $C42AE1 ($4D, $0B38, $08A8); $C42AE7 ($50, $0A9D, $0700); $C42AED ($50, $0A21, $05AC); $C42AF3 ($51, $0990, $041F); $C42AF9 ($4E, $0B2F, $0730); $C42AFF ($50, $0A94, $0588); $C42B05 ($51, $0A02, $03F8) |
| $C48F90 | $000F / $000E | $C42956 | 3 | $C42957 ($15, $0400, $0E00); $C4295D ($2D, $0800, $0800); $C42963 ($4A, $0800, $0800) |
| $C4A190 | $0010 / $0010 | $C42BD4 | 16 | $C42BD5 ($1E, $0380, $0C80); $C42BDB ($34, $04A0, $09E0); $C42BE1 ($1F, $03D2, $0B64); $C42BE7 ($1F, $0327, $0CA6); $C42BED ($1D, $025E, $0E21); $C42BF3 ($1C, $04AA, $0ACE); $C42BF9 ($1F, $03D8, $0C5A); $C42BFF ($36, $030F, $0DD4); $C42C05 ($37, $0090, $0B35); $C42C0B ($1B, $0244, $0C1C); $C42C11 ($32, $03BF, $0CE5); $C42C17 ($1B, $0538, $0DAD); $C42C1D ($32, $0240, $0B92); $C42C23 ($32, $0386, $0C3F); $C42C29 ($32, $04C8, $0CEA); $C42C2F ($35, $0650, $0DBA) |
| $C4A790 | $0010 / $000F | $C42706 | 4 | $C42707 ($47, $0000, $0700); $C4270D ($28, $03A0, $0400); $C42713 ($27, $08B0, $0090); $C42719 ($29, $09C0, $0760) |
| $C4B390 | $0011 / $0010 | $C42646 | 22 | $C42647 ($15, $0000, $0000); $C4264D ($20, $0980, $0580); $C42653 ($21, $0980, $0280); $C42659 ($53, $0800, $0800); $C4265F ($54, $0800, $0800); $C42665 ($14, $0200, $0600); $C4266B ($33, $0200, $0240); $C42671 ($48, $0200, $0240); $C42677 ($22, $0220, $00E0); $C4267D ($52, $0E00, $0600); $C42683 ($24, $0D98, $0414); $C42689 ($25, $0C00, $0498); $C4268F ($26, $0BD8, $04A8); $C42695 ($23, $0F00, $0380); $C4269B ($17, $0800, $0800); $C426A1 ($18, $0800, $0800); $C426A7 ($75, $0780, $0B80); $C426AD ($22, $0658, $0CA8); $C426B3 ($22, $04F0, $0E10); $C426B9 ($22, $0388, $0F78); $C426BF ($19, $0800, $0800); $C426C5 ($1A, $0800, $0800) |
| $C4D190 | $0011 / $000F | $C42B66 | 18 | $C42B67 ($46, $0E00, $0600); $C42B6D ($15, $0E00, $0600); $C42B73 ($15, $0400, $0E00); $C42B79 ($52, $0800, $0300); $C42B7F ($39, $0040, $0A00); $C42B85 ($38, $02D0, $05F0); $C42B8B ($3B, $0070, $03F0); $C42B91 ($43, $01F6, $0538); $C42B97 ($43, $036E, $0675); $C42B9D ($3A, $04E5, $07B1); $C42BA3 ($40, $01FE, $0D4B); $C42BA9 ($45, $01C2, $0C99); $C42BAF ($41, $018C, $0BE8); $C42BB5 ($3D, $01FB, $0A4C); $C42BBB ($3C, $00DD, $0AB1); $C42BC1 ($44, $0097, $09CA); $C42BC7 ($3E, $0F33, $0949); $C42BCD ($3F, $0EF8, $0A00) |

The same static stream can be selected for different workspace bands, and the template words remain inputs to the placement builder rather than direct global coordinates. The inventory identifies cell-content candidates only for this sealed origin window; it must not be extrapolated into a complete terrain export.
