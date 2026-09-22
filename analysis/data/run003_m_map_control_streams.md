# `M` map-mode static control-stream entries

Classification: **scenario-backed map-mode renderer-control activity**.

From the sealed five-frame post-`M` checkpoint, a no-input collector stops at
the established `$C1F6F8` control-stream walker six times while the map
transition advances. The entered `$C45A36` values are immutable scene-control
addresses:

| stepped host frame | `$C45A36` stream | static family |
| ---: | --- | --- |
| 10 | `$C35BD2` | segment 42 (`$C35568-$C361FF`) |
| 11 | `$C35598` | segment 42 |
| 12 | `$C355D0` | segment 42 |
| 13 | `$C36214` | segment 43 (`$C36208-$C36A1B`) |
| 14 | `$C36210` | segment 43 |
| 15 | `$C3B6AE` | static scene-family candidate |

The first words of each stream are retained in the ignored raw collector
output. `$C1F6F8` is the proven projection-path control walker, so this
observation joins the map-mode scenario to immutable 3D scene-control inputs,
in addition to its already traced static-template-to-flat-placement chain.

This does not identify any stream as the full map, Golden Gate, coastline, or
terrain mesh. The collector single-steps through a full host frame after each
entry, so its host-frame ordering is debugger collection timing, not a claim
that a single uninterrupted map video frame submitted exactly these six
streams. Nor does a control stream by itself prove an individual bitplane
pixel or immutable vertex list.

Authority: sealed `captures/run003`; `build/run003_m_visual_5/state.bin`; and
ignored `build/run003_m_map_control_stream_entries/control_stream_entries.json`.

Reproduce:

```text
python scripts/collect_control_stream_entries.py \
  --restore build/run003_m_visual_5/state.bin \
  --config captures/run003/config.uae \
  --frames 25 --max-entries 128 \
  --output build/run003_m_map_control_stream_entries
```
