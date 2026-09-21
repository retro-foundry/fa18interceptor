# `$C2FF58` renderer lane stage

Classification: **runtime-backed dataflow/control flow**. This complete
`$C2FF58-$C30037` stage is executed in `pcode/raw/run001_c2ff48_renderer_stage/`.

`source_amiga/observed/run_renderer_lane_stage.asm` is byte-exact for all 224
bytes. A non-negative `$C456E8` with non-zero `$C456EC` takes a prelude at
`$C3040C`. Otherwise, each enable bit in `$C456E7` selects a lane helper call
at `$C30466` using lane selectors `$C,$8,$4,$0`; enabled lanes derive shifted
`D3/D4` values from `$C456E8/$C456EA`, while disabled lanes shift
`$C45956`.

After the lane paths, it calls `$C304B2` and writes `$0400` to `DMACON`.
This establishes renderer control/dataflow and the DMA write, but does not
assign a visual meaning to the lanes or input words.
