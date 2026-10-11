# `$C3925A`: external-view entity controller trace

Classification: **static controller family with a trace-proven six-polygon output set**. It is the first model-view candidate suitable for a combined wireframe, not yet a named immutable mesh export.

The sealed run031 frame-7,500 external-camera checkpoint visibly contains the player's aircraft. In `build/run031_frame7500_c3925a_stream_trace_20k/selected_stream_trace.jsonl`, `$C1F6F8` enters the byte-stable Hunk-52 stream `$C3925A`. Before the next walker entry, the trace reaches `$C2FF48` six times with static contexts `$C3925C` (three) and `$C3925E` (three). Those contexts are therefore one controller-associated output family rather than two independently guessed fragments.

`scripts/plot_runtime_geometry.py` combines those six finalized polygons into [the controller-family orthographic wireframe](../plots/external_aircraft_c3925a_controller_family_orthographic.svg). It deliberately uses the captured projection-workspace coordinates and says so in the caption: the drawing is a coherent view-dependent renderer output, not an assertion that `$C48390` holds original local vertex data.

The visible external-camera aircraft is a scenario anchor for investigating this family. It does not, by itself, prove that every polygon belongs to the aircraft or identify the upstream immutable local-coordinate source. The next required step is to trace the first writer of the selected `$C48390` records before the controller runs, then join that source to this six-polygon topology.
