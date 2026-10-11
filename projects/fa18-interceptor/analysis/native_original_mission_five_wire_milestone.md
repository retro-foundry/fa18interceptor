# Original mission-five wire approaches — 2026-10-10

Three complete original recordings reproduce every trace byte, final RAM export
and consumed key in separate runs without the input controller. Each preserves
the independently earned 47,814-observation qualification/patrol/escort prefix.
None earns mission five; they remain failed or incomplete landing evidence.

| Input route | Observations | Observed result |
| --- | ---: | --- |
| Live wire target, preceding controller | 80,000 | Standoff orbit at altitude 9,890.828125; height gate never reached |
| Descend during the standoff turn | 74,804 | Height gate reached; wire crossed too high; stopped on water |
| Same descent, level-height final | 80,000 | Wire crossed too high; continued flying beyond the carrier |

Mission-five flight begins at observation 61,797 in each recording. These are
pre-input observations, not newly established executed-body counts. Actual
qualification and preceding grades remain earned, completion count stays two,
mission-five grade stays zero, and final mode/phase remain five/one.

The original carrier's logged wire target is `(1153000, 119.03125, 1070584)`;
119.03125 is the observed aircraft touchdown height, while the wire geometry is
at 112. The preceding controller suppresses descent during large yaw errors,
leaving the first route orbiting above its requested 819.03125 standoff height.
The validation controller now allows its existing patrol-style descent command
during the mode-five wire standoff turn. The original owns all resulting motion.

Both later recordings reach the height gate at observation 74,369, altitude
819, horizontal distance 1,490.371481 from the standoff. The curved final crosses
wire Z between observations 74,558 and 74,559 at altitudes 188.359375 and
185.46875. The level-height final crosses between the same observation numbers
at 228.875 and 227.828125. Both are above the observed aircraft touchdown height.
The first later stops at `(1152994.390625, 2.9296875, 1077875.046875)`, water
contact `8080`, speed zero. The second continues beyond the carrier; its final
RAM position is `(1153007.53125, 111.03125, 1393522.375)`, contact `8000`.
Neither result establishes an arrestor-alignment fault or a native game fault.

The optional `--level-final` input borrows the existing native escort validation
controller's temporary height-goal adjustment. It changes ordinary keyboard
choices only, restores the controller's home height after each call, and is
restricted to mode-five wire input. Invalid combinations reject before recording;
reuse rejects a changed profile without changing the retained report.

The failed-water diagnostic needed a correction before these recordings. Its
previous broad grounded/zero-speed condition also matched a valid carrier arrest
before the original awarded its grade. Actual successful escort evidence reaches
contact `C482`, speed zero, at observation 46,934 / PAL frame 94,778, then earns
its grade at PAL frame 94,852. The corrected diagnostic stops only at observed
water contact `8080`, preserving that 74-frame original result sequence.
Compiled probes using both actual contact cases expose the premature carrier
stop in the preceding predicate; nine invalid external driver configurations
and three invalid/mismatched level profiles also reject.
The interrupted capture made with the preceding predicate remains retained,
explicitly unaccepted, without a complete replay or final RAM claim.

The [checkpoint](figures/native_original_mission_five_wire_checkpoint.json)
binds compressed recordings, reports, controls, source identities, independent
replay digests, crossing observations and the guard results. Captured controller
sources are retained compressed with their original hashes, including preceding
committed driver sources reconstructed with verified byte identity. Existing
failed-landing evidence remains unchanged. Passing raw replay copies are removed
and the 4 GiB pruner remains enabled.

The exercised path is `fa18_original_mission_pilot -> original keyboard IRQ
queue -> original game`. This removes a validation input limitation and fixes
diagnostic termination; it removes no playable runtime dependency. Native game
code, clocks, original RNG, grades and preallocation are unchanged. The playable
Release executable remains SHA-256
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.

A successful original mission-five carrier return, further independent native
full-flight comparisons and original sound timing remain open. Campaign
continuity remains waived; named-state cleanup remains outside this goal.

To verify the retained level-height recording against another unmodified replay:

```powershell
python tools/native/check_original_mission_recording.py --mode 5 --source-prefix build/native-flight/original-escort-wire-target --repeat-steering --wait-for-approach-height wire --level-final --reuse-driver --out build/native-flight/original-mission-five-wire-level-final
```

A zero verifier exit establishes exact replay, not mission completion.
