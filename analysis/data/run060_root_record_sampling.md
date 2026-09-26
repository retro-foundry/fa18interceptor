# Run060 root control-record sampling

Classification: **sealed full-replay negative evidence**, with one later
field-level physical identification. This sampling tests whether the first
512-byte control-table record at `$C46184` can be called the complete moving
player position/orientation record. It cannot.

The complete deterministic run060 replay was sampled every 250 GUI frames for
the root record's leading fields, `+$10`, `+$14/$18/$1C`, `+$66/$6E`, and
several later words. A second complete sampling pass captured the active
selector fields every 100 frames. Generated authority artifacts are:

- `build/run060_root_record_samples.json`
- `build/run060_record_selector_samples.json`

The root record changes during the early run (for example, its `+$14/$18/$1C`
words differ between GUI frames 1,001 and 2,001), but from sampled frame 2,251
through 10,001 those fields are constant. Recorded joystick/mouse flight input
continues well after that point.

Meanwhile `$C459B4` continues to select values including `$0000` and `$000E`
(with sampled `$0001`, `$0003`, and `$0058` intervals), consistent with a
multi-record update table. `$C458DC` remains zero in this sampled scenario;
the two selectors have different observed roles.

Therefore `$C46184` is a useful selected-record base and the run062
negative-candidate record, but its root slot is **not** established as the
complete moving player position/orientation record. A later bounded run060
trace does show that `$C13E10` selects this root slot in one real
input-control update; see `run060_c13e10_root_control_record.md`. A separate
live formatter packet proves root `+$18` is the selected cockpit-altitude
source (145 FT in this interval); see `run060_root_altitude_formatter.md`.
Those promotions establish an input-controlled record with an aircraft
altitude field, not horizontal position or orientation. The next player-state
experiment must trace the remaining fields into world placement or camera
output rather than infer them from selection alone.
