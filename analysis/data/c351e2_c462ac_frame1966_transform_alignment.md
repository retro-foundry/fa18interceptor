# `$C351E2 -> $C462AC`: frame-1966 transform-alignment evidence

Classification: **related-coordinate numerical alignment; rejected as the direct producer path**.

The first replay-frame transition of the unresolved `$C462AC-$C46301` slots occurs at frame 1966: sampled words `$C462AC/$C462AE/$C462B0` move from zero to `FFF7/0000/FF48` together. A subsequent instruction trace locates the direct first store at `$C0D396`, where `A3=$C46228` and `movem.w d3-d5,$84(a3)` writes slot 22. See [the derived-tail routine](../routines/c0d384_derived_vertex_tail.md).

At the post-frame snapshot, applying the sampled `$C45BC6` nine-word matrix to the fourteen immutable `$C351E2-$C35234` triples with the same signed-16-bit product and `>> 8` convention used by `$C1F100/$C1F21C` reproduces twelve output triples within four units. The two exceptions are the symmetric source-order rows 30 and 31: both retain their X/Y fit but have a +40 Z residual. This establishes a related coordinate pattern, but the direct `$C0D384` trace rejects it as the actual producer input.

[Machine-readable residual report](c351e2_c462ac_frame1966_transform_alignment.json) records every source, expected triple, sampled output, and residual. Reproduce it with:

```powershell
python scripts\validate_static_transform_alignment.py --samples build\run031_frame1966_c462ac_and_matrix_samples.json --memory captures\baseline_menu\slow.bin --source 0xC351E2 --output 0xC462AC --matrix 0xC45BC6 --count 14 --tolerance 4 --report analysis\data\c351e2_c462ac_frame1966_transform_alignment.json
```

The sample JSON retains the explicit output/matrix word list. The result is a reproducible numerical comparison, not a source-to-output provenance claim.
