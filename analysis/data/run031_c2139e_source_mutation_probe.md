# Run031 `$C2139E` source-table mutation probe

Classification: **controlled runtime mutation, negative visual result**.

An ordinary full replay reached `$C2139E` at frame 12,001 with selector
`$C45954=$000C`. Before the handler executed, the experiment changed the
first word of `$C48390` through `e9k_debug_write_memory`:

| Address | Original | Mutated |
| --- | --- | --- |
| `$C48390` | `$0185` (389) | `$0285` (645) |

The remainder of the first two table triples was unchanged. The mutation ran
only in a fresh emulator instance; no disk, recording, or saved state changed.

## Result

The mutated frame-12,001 screenshot is pixel-identical to the independently
replayed unmodified frame: the RGB difference image has no bounding box and
zero changed pixels.

This rejects a direct visible contribution from this *specific* table word in
this frame. It does not prove `$C2139E` is unrelated to the Golden Gate scene:
the mutated component may be clipped, overwritten later in the frame, or may
not participate in the accepted polygon path.

## Evidence

- Baseline: `build/run031_frame12001_baseline/frame_12001.png`
- Mutation: `build/run031_frame12000_c2139e_source_mutation_probe/mutated_frame12001.png`
- Runtime details: `build/run031_frame12000_c2139e_source_mutation_probe/probe.json`
