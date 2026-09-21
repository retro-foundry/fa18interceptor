# `$C315C0` second postflight record normalization

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes the complete arithmetic span.

`source_amiga/observed/normalize_postflight_record_delta_second.asm`
reproduces `$C315C0-$C31611` (82 bytes). It obtains a shift count from byte
`$63` of the selected `$C46184`-relative record, rounds two arithmetic right
shifts, negates both results, and rejects values outside fixed signed bounds.

The fixed-point units and record role are unproven; only the decoded arithmetic
and reject edges are claimed.
