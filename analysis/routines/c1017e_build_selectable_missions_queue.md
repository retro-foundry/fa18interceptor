# `$C1017E`: build selectable-missions text queue

Classification: **scenario-backed queue construction**.

The byte-exact entry `$C1017E-$C101FB` clears the display through `$C2FD22`,
then starts `$C4574A` with selector 64. It iterates indices 3 through 8,
testing byte `$C1AB74 + $12 + index`; for each nonzero conditional byte it copies the next
word from `$C3ED00` into the selector queue.  It always appends `$806E`, a zero
terminator, and restores callback `$C0FCB4`.

Authority: `build/run029_key6_submenu_queue_writer/`, a bounded no-future-
input trace after controlled top-level digit 6.  It enters at frame 270 and
returns after 32,264 instructions (the first 32,174 are the display clear).
The six tested addresses `$C06AAD-$C06AB2` are zero in this run, so no optional
word is copied.  The final explicit writes are:

```text
$C4574A := $0040
$C4574C := $806E
$C4574E := $0000
```

This proves the producer of the visible selectable-missions heading and its
Esc instruction record for this initial state.  The bytes' wider mission-
availability/persistence meaning remains unknown.

At the `$C1017E` entry in this same snapshot, `$C1AB74` resolves to
`$C06A98`; bytes `$C06A88-$C06AB7` are all zero. Thus this is observed as a
live, pointer-selected conditional-state block in this scenario, not as a
static mission-record table. Its allocator, writer, and persistent ownership
remain unknown.

The same six bytes are also zero in both independent final frame-14,298
run029 replay snapshots.  This excludes them as a demonstrated
qualification-success indicator for that capture; it does not otherwise name
their ownership or persistence semantics.

## Controlled conditional-byte probe

`build/run029_key6_all_conditionals_queue_probe/` is the same sealed-input
key-6 path, paused at the `$C1017E` entry. Only the six tested bytes
`$C06AAD-$C06AB2` were debugger-written to one before stepping; no input was
delivered during the stepped interval. The six live copy instructions are:

| Conditional index | Tested byte | Read word | Queue destination |
|---:|---|---|---|
| 3 | `$C06AAD` | `$C3ED00 = $004D` (77) | `$C4574C` |
| 4 | `$C06AAE` | `$C3ED02 = $004E` (78) | `$C4574E` |
| 5 | `$C06AAF` | `$C3ED04 = $004F` (79) | `$C45750` |
| 6 | `$C06AB0` | `$C3ED06 = $0050` (80) | `$C45752` |
| 7 | `$C06AB1` | `$C3ED08 = $0051` (81) | `$C45754` |
| 8 | `$C06AB2` | `$C3ED0A = $0424` | `$C45756` |

The first five values are the static table's F2--F6 selector codes. The sixth
is instead the first signed relative offset of the selector-record table.
This is a direct producer/table-boundary result, not a valid
availability-state experiment: forcing all six bytes produces the
out-of-inventory selector word `$0424` before `$806E`. It establishes that
the visible F1 label cannot be inferred as the first member of this
conditionally appended queue.

The saved post-step state was resumed for 180 normal no-input frames. The
visible result contains F2--F6 as well as the already-independent F1 label;
the exact queue and screenshot are documented in
`analysis/run029_key6_all_conditionals_display_probe.md`. This promotes the
77--81 links from queue-only dataflow to a controlled text-display contract,
while leaving the condition bytes' real gameplay ownership unknown.
