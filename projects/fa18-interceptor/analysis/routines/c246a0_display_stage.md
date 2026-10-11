# Display-stage child at `$C246A0` (Hunk 19 +`$18`)

Classification: **structural display-stage packet**. This complete child is
called by the human-flight `$C21060` dispatcher target. It has no assigned
object, geometry, or camera meaning.

## Runtime packet

- Direct edge `$C210CE -> $C246A0 -> $C210D4`, observed four times in the
  enclosing `$C21060` packet.
- The bounded invocation runs 7,903 instructions and returns at `$C210D4`.
- P-code: `pcode/raw/run001_c246a0_display_child/`, 1,588 starts / 11,314
  operations, all mapped to resolved Hunks.

The packet calls `$C247C0` 23 times, `$C248B2` 23 times, `$C24996` 20 times,
and re-enters `$C246A0` through `$C2AFE2` twice. It reaches the known `$C2FA7E`
line emitter twice through the Hunk-36 renderer path. Those observed calls
prove display-path participation only.
