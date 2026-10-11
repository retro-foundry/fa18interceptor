# `$C351xx`: shared dual-lane flight-object source transform

Classification: **trace-proven shared static template input table for two separately transformed C34C and C34A renderer layers**.

The frame-1966 bounded lead trace reaches `$C1F100` with `A1=$C3515E`, `A0=$C46228`, and `A3=$C48390`. Its first vector is transformed and written to `$C46228` through the `$C45BC6` matrix. Without advancing the source cursor, `$C1F160-$C1F1B4` transforms that same result through `$C45BD8` and writes a second three-word lane at `$C48390`.

The next loop entry `$C1F21C` consumes the next `$C351xx` triple and repeats the same dual output. Later in the same pass, `$C1F2CE` writes `$C483B8`, a record directly consumed by the `$C34A9A` pre-clip faces. The C34C records consume the companion `$C46228` lane.

This is stronger than simple co-rendering: the two face families share the static template-transform pass, but use distinct static face records and matrices. Their frame-12000 output coordinates are not spatially comparable because the lanes use different transforms. Export must preserve the face layers and transform paths rather than flattening their mutable outputs into one guessed mesh. See [the coordinate-frame qualification](c34a_c34c_frame12000_instance_separation.md).

Evidence boundary: the trace establishes the shared `$C351xx` input stream and its two output lanes. The C34A external-camera oracle supports an F/A-18-like aircraft template. C34C's five-face layer may be a reduced-detail alternate layer. The larger C34A family has 22 distinct observed records, submitted through two heavily overlapping contexts; a shared world-instance relation, distance/LOD selection rule, and any missile identity remain unproven.
