# Complete primary and alternate scene-placement traversal

2026-10-02. `port/game/scene_placements.c` implements the complete
$C1CB14/$C1CB26 pair, including their shared traversal through $C1CCBA.
The pair contains 99 unique original instructions, not two separate copies
of the shared loop. Both are registered game entries; coverage is 425/624
and 208 entries have source timing. Descriptor consumers remain explicit
child owners and are not counted as newly ported functions.

## Source contract

The two entries select the primary or alternate 24-byte placement list at
$C4E9AA/$C4F03A, seed its cursor from $C459AC/$C459AE and clear the source's
cell/position flags. The negated projection height supplies the signed byte
table comparison, except for the original signed-long saturation branch.

Each record supplies the header, descriptor and three signed coordinate
words. The parent preserves negative-descriptor rejection, the $C1ED48
height gate, packed coordinates, cached-distance refresh thresholds and the
two-phase refresh selection. It calls the existing readable `target_distance`
once when refresh is required and stores its word result back into the list.

The negative previous-result gate decrements the signed countdown byte;
when the countdown expires it reloads the source's shared skip byte. A
dispatched record decrements its visit byte and publishes the magnitude,
descriptor control/auxiliary streams and signed header kind. The child
callback receives the descriptor routine and parameter pointer. After it
returns, the parent rereads the selected list and shared offset, stores a
positive word result or -1 and advances the offset by 24. The negative
word terminator ends the traversal only when it is exactly $FFFF.

Two source distinctions matter:

- $C1ED3C bypasses both distance refresh and the negative-result/countdown
  gate. It still publishes and dispatches the descriptor.
- Decrementing -128 stores +127 but takes the signed negative branch:
  `SUBQ.B`/`BGE` uses the unwrapped signed subtraction through N/V. Testing
  only the wrapped stored byte would skip a required callback.

The domain exposes semantic observations and a required child consumer,
without CPU registers, CCR, instruction handlers or timing fees. Its CPU
adapter reconstructs all caller-live outputs in `port/game/glue`.
The distance/magnitude adapters can publish the already computed result
without calling the domain child or writing its globals again.

## Independent complete-call proof

`python tools/recomp/check_scene_placements.py` passes 8,192 structural
calls: 4,096 per entry. Original execution independently evaluates the
expected output. The comparison retains all sixteen registers, including
all data-register high words, PC, SR control bits and every Chip/Slow RAM
byte outside a 160-byte private child-stack region. Caller CCR is dead;
production liveness is not relaxed. The stack allowance covers the original
empty renderer's 152-byte LINK locals, saved A6 and nested return words.

Fixtures cover empty and one-to-five-record lists, nonzero offsets,
negative/positive/extreme projection heights, every header high byte and
shift, negative descriptors, distance thresholds, all refresh phases,
signed countdown edges and both positive/nonpositive callback results.
They use original descriptor children with terminated control streams,
including $C096BC/$C096CA/$C1ED3C/$C1ED48 and $C2F490. No original code bytes
are patched. These bounded child fixtures are not a proof of those renderer
parents; the recorded checks below exercise real scene consumers separately.

`python tools/recomp/check_whole_call_glue.py C1CB14 C1CB26` disables only
the selected timing steps in a temporary registry, retaining the original
normal caller masks. Returns $C0F0AC/$C0F0BA retain all sixteen registers
and all eight data-register high words; flags are dead. The handwritten
callback bridge dispatches independently owned source children and holds
their event deadline for this whole-call CPU/RAM comparison, matching the
reference's event hold. Live timing uses resumable steps instead.

| Native recording | Shadow matches | Shadow incomplete | Sandbox matches | Sandbox incomplete |
| --- | ---: | ---: | ---: | ---: |
| demo01 | 1,935 | 2,235 | 4,004 | 187 |
| qual_carrier_success | 2,967 | 327 | 6,740 | 0 |
| qual_fail_crashes | 699 | 3 | 702 | 0 |
| Total | 5,601 | 2,565 | 11,446 | 187 |

All completed comparisons match, with zero mismatches and zero hardware
classifications. Incomplete calls remain incomplete and are not counted as
passes. Sandbox replay may encounter a different number of calls; it does
not provide live timing proof.

## Source timing and integration

The resumable bridge owns only the parent's 99 source instructions. Runtime
dispatch owns distance and indirect descriptor children and event boundaries.
The local DMA oracle passes 99 instructions / 3,168 cases, comparing CPU,
full SR, PC, cycles and RAM. A fresh combined oracle passes 11,402
instructions / 364,864 DMA cases. GNU and MSVC Release builds pass.

The bounded 600-frame demo probe is exact for the pair. ALL still first
differs at frame 416 by 361 pixels. The minor Copper fade remains deferred
by the user; registering these complete functions advances readable source
coverage without claiming that combined visual timing is fixed.

Full registered and isolated live results are recorded in
`analysis/figures/native_scene_placements_checkpoint.json` and the current
handoff. The full 425-entry gate matches 653,694 shadow and 1,069,233 sandbox
calls with zero mismatches, sealed RAM exact and poison frames identical.
The isolated pair matches all 36,236 live frames and all three sealed final
RAM hashes. Aggregate completed calls can fall when parents absorb children;
full-registry parent totals also differ from the isolated registry's totals.
The checkpoint retains hardware and incomplete classifications separately.
Build storage is 0.283 GiB after automatic replay cleanup.

Historical capped UAE evidence in
`c1cb14_flight_update_stage.md` remains capped evidence; it is not reclassified
as a complete invocation by this new independent proof.
