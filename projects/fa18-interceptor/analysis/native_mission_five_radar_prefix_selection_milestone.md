# Mission Five radar selection within a frame — 2026-10-10

The complete head-up drawing run reached its terminal radar controls, then
rejected the assumption that one selection applies to both radar tails. Its
report was not written. No earlier drawing-history assertion failed, but this
run is **not accepted as a complete-flight drawing result**. Its original
inputs, native bodies and reference executables remain retained.

The exact first offending original input is observation **62,005**, paired
through verified earned-continuation mapping with native update **61,302**.
Both execute two complete radar tails. Selection starts at record offset
**4,096**, changes to **5,120** after the first tail's scan, then remains there.
The second tail correctly blinks record 5,120. The old checker treated it as
an ordinary contact using the initial selection and rejected its phase probe.

The connected native caller remains `native_hud_draw ->
draw_postflight_renderer_dispatch`. The original C31226 dispatcher runs each
variant's complete tail, including `scan_postflight_variant_records`, before
starting the next variant. This is original selection behavior, not a native
drawing fault. Production gameplay is unchanged.

The external radar observer now records each prefix's actual selection, each
submitted point's prefix and selection, and the final selection. On all four
actual source/native normal/flipped inputs, the new and previous observers
produce identical complete RAM and identical previous events. Each C owner
matches original instructions in every non-stack byte. The actual native body
matches original execution, and both ordinary outputs match the recorded
owner-return pages. All four phase mutations are rejected with the corrected
per-prefix rules. No phase is fitted and no page byte is masked.

The verifier keeps historical fixtures using their original stable-selection
contract. New evidence requires actual prefix identities, selection and phase
for every point; ordinary contact coordinates stay strict within each prefix.
Fifteen radar tests and fourteen complete-history tests pass, including the
actual changing-selection fixture and rejection of mislabeled prefix,
selection, phase, ordinary contact coordinates and replacement of the second
selection by the initial one. Syntax and diff checks pass.

The complete drawing tool now writes a compressed progress journal as histories
and owner checks finish. Before terminal controls, it retains their complete
inputs. A control assertion produces an explicit failing report rather than
discarding completed evidence. The new retention test confirms that completed
history and radar inputs survive a counter-control rejection. These facilities
do not authorize resuming from a later convenient page or accepting an
incomplete run.

The [checkpoint](figures/native_mission_five_radar_prefix_selection_checkpoint.json)
binds the actual paired input/output fixtures, body identity, all four control
rejections and old/new compressed executable identities. Passing temporary raw
RAM is removed and normal pruning runs. The complete drawing gate needs a
new run with the corrected observer and durable reporting. Broader independent
flights and original sound acceptance remain open; campaign continuity stays
waived and named-state migration stays separate.
