# Attract projection-depth metric oracle

Classification: **same-replay producer-to-consumer dataflow**.  This proves a renderer projection component handoff, not physical distance, object identity, or LOD.

Authority: `build/attract_cockpit_1800_tenframe_trace/trace.jsonl`. Regenerate with `python scripts/extract_attract_projection_metric_oracle.py`.

At trace index 38158 (frame 4), `$C1C636` stores `D1=-125` to `$C45A78`. At index 45022 (frame 5), `$C2AAD2` reads that longword; the immediately following trace row has `D0=-125`. The 19 intervening accesses are reads, and no intervening `$C45A78` write occurs.

Immediately before the publication tail, `A2=$C46184`; the direct arm reads its `+$14/+$18/+$1C` fields. This is the established mutable control-record-bank root, not immutable terrain-template data.

Therefore this replay proves `$C1C636 -> $C45A78 -> $C2AAD2` with value parity. The producer's selected control-record role, camera context, and any physical interpretation remain open.
