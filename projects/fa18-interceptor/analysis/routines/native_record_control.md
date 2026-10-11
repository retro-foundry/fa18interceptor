# Native record control and control-stream owners

`port/native_record_control.c/.h` implements complete `$C23228` root control,
`$C233AA` secondary control, the shared `$C233D6` stream player, and actual
`$C23578` next-stream behavior against the live sixteen-record bank. The
scheduler now calls these owners directly. The authority is the sealed original
instruction graph, with `port/game/control_records.c` as readable corroboration.

The owner preserves word-width magnitude arithmetic and tone-eight gating,
mode-125 record cloning, classification preservation, sequential 41-long copy,
actual `$C091E0` fixed-point local placement, packed coordinate publication and
the full carried axis used by the later record-view owner. Caller and companion
may alias. Original byte/word flags and shared stream position remain distinct
from per-record controls. Streams are explicit immutable command windows,
empty values or code values; signed stream indices and signed command offsets
retain their original meaning. Metadata and messages use caller-bound field
windows, including the original adjacent rows. No guest addresses or CPU state
appear in the native interfaces.

The player preserves event, playback and record gates, scene initialization,
code-stream lifecycle, end/next routing, special index-zero handling, message 29
at the command limit, metadata-derived observer state and the altitude message.
Stream initialization may publish the carried axis. Tone, message and scene
initialization remain explicit lower owners (`C3316E`, `C25704`, `C28722`);
missing bindings fail at the actual route with earlier stores retained.

The append arm `C23354-C233A4` cannot execute from these complete entries.
Sealed `C23348` TST feeds adjacent `C2334E` BNE-return and `C23350` BEQ-player
without any flag writer. The audit checks the exact six-, two- and four-byte
instructions before excluding that fall-through arm. It does not claim to port
an independently invoked append entry. Malformed streams that endlessly
advance are not given an invented runtime iteration limit.

Validation: `python tools/recomp/check_native_record_control.py` passes 24,576
complete calls (8,192 per entry) at all 263/263 reachable original boundaries.
Actual next-stream and local-placement instructions execute in the source
oracle. All Chip/Slow RAM matches except the CPU ABI stack `C7FD00..C7FF00`;
typed aircraft, matrix and position owners are verified independently. The
full carried axis matches after every call. Fixtures cannot overwrite any
audited instruction bytes. They cover all sixteen callers, aliased companions,
signed indices/positions, wrapping, copy and packed arithmetic, empty/code/byte
streams, stream limits, metadata signs, end/next and initialization routes.

There are 5,361 controlled tone, 12,224 ordered message and 1,201 scene calls.
Contracts check program/message arguments and original return sites; scene
contracts vary the live record and full carried axis. These comparisons prove
parent behavior across those contracts, not the actual lower children. See
`analysis/figures/native_record_control_checkpoint.json`.

MSVC Release native game and affected contracts, strict GNU compilation of
the new owner/contracts and changed callers, twelve affected CTests and the
499-file native build guard pass. Composed tests exercise direct secondary
playback and reject split record, event, mode or carried-work bindings. The
bootstrap observation point now follows the direct control/view/range owners
at the root pose boundary. The subsequent `native_record_pose` batch makes
pose/history direct, leaving five scheduler children: periodic, primary/secondary
placement, dispatch and finish. Native main integration,
original runtime asset bindings and actual lower children remain open.
