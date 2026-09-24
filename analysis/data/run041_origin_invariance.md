# Run041 selector-origin invariance

Classification: **scenario-backed selector-origin invariance; not physical-distance LOD proof**.

The four saved run041 checkpoints span the measured red-Golden-Gate growth and the documented renderer source-family transition. Their sampled selector-origin state is identical. The two bounded trace windows nevertheless execute the downstream `$C1D10C-$C1DC08` template-selection region.

| Frame | $C45C3E | $C45C42 | $C45C46 | $C457B6 | $C458AE | $C458B2 | $C45785 | `$C29042-$C295D0` trace instructions | `$C1D10C-$C1DC08` trace instructions |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 5000 | 264187402 | 2928 | 286094979 | 6 | 0 | 0 | 0 | 0 | 561 |
| 5750 | 264187402 | 2928 | 286094979 | 6 | 0 | 0 | 0 | -- | -- |
| 6000 | 264187402 | 2928 | 286094979 | 6 | 0 | 0 | 0 | -- | -- |
| 6250 | 264187402 | 2928 | 286094979 | 6 | 0 | 0 | 0 | 0 | 425 |

## Result

The sampled `$C45C3E/$C45C42/$C45C46` selector-origin triple, `$C457B6` adjustment mode, `$C458AE` detail mode, `$C458B2` detail index, and `$C45785` enable byte are identical at frames 5,000, 5,750, 6,000, and 6,250. The producer range does not execute in either bounded trace, while the downstream template-selection region executes in both.

This rules out a change in these sampled selector-origin inputs as the cause of the run041 source-family change. It does **not** measure aircraft-to-landmark range, establish a renderer selector threshold, isolate all camera state, or prove a physical-distance LOD rule.

Authority: sealed `captures/run041` replay checkpoints and the listed ignored build artifacts. Reproduce with `python scripts/summarize_run041_origin_invariance.py`.
