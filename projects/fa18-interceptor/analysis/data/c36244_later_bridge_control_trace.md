# `$C36244`: later-bridge live control-stream trace

Classification: **runtime-backed static control data feeding a renderer handler**. This is not a source-mesh extraction.

The sealed frame-14,500 checkpoint reaches `$C1F6F8` with `$C45A36=$C36244` (segment 43, payload offset `$3C`). `build/run031_frame14500_c36244_stream_trace/selected_stream_trace.jsonl` single-steps 600 instructions from that exact entry.

Its first record is consumed as follows:

1. The walker reads control word `$0000` and class word `$2000` from `$C36244`.
2. Bit `$2000` selects the six-word handler route at `$C1F770`.
3. The following six static words are loaded into `D0-D5` and `$C1FB9C` is called.

The trace therefore proves a direct static-stream-to-renderer-handler path before the later mutable projection workspaces. It does **not** prove that the six words are vertices, nor that this one control record accounts for the whole visible bridge. The 24-byte `$C36244` prefix remains separately registered in the runtime region manifest as data inside original `HUNK_CODE`.
