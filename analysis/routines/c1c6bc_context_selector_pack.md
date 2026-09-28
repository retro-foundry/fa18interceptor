# `$C1C6BC-$C1C7F5` context selector pack

`context_selector_pack.{c,h}` ports the second mutable selector pack produced
inside the `$C1C63E` update stage. The port preserves both source routes:

- with `$C45785 == 0`, it classifies the active record (`+$56/+58/+5A/+6C`),
  copies `+$06/+08/+0A` to the pack, and derives `$C45850` from their low two
  bits;
- otherwise it invokes the caller-owned `$C29042` origin updater, consumes the
  resulting `$C45C3E/$C45C46` pair, and performs the source's coarse/fine
  selector comparisons and `$C45858` change-bit updates.

The state exposes the map-parent-relevant `$C45850` table selector, its
companion selector byte, two selector words, the `$C45858` accumulated change
byte, and the structural `$C458BC` magnitude class. It does not claim an
absolute coordinate meaning for the origin triple and does not schedule the
unported `$C29042` producer.

Its contract checks the record-copy route and an alternate-origin route,
including the `mode == 2 && depth <= -$1000` byte-change suppression.
