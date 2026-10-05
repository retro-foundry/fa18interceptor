# Native context refresh

`port/native_context_refresh.c/.h` implements complete
`$C1C860-$C1CA2C` and direct `$C1CA82` record flagging against the shared
native record, workspace and view owners. Its behavioral authority is
`port/game/context_refresh.c` and the sealed proof in
`native_c_scene_bootstrap.md`.

The owner applies the signed `$F8000000` position guard, samples the live
request byte, marks all sixteen record/workspace pairs, and validates the
original low nibble. Invalid kinds publish error `$27` and continue because
the source fault hook returns. The selected route arithmetic-shifts the viewed
record's signed words by two. The alternate route masks each origin long,
swaps its halves and shifts by eight. Both publish directly to the shared
condition keys.

Bit zero is selected from the original request sample. Bits one, two and three
are reread from the live update mask after each template child, preserving
child consumption. The parent then runs sort, cache and the current condition
route, writes the caller-owned frame gate in source order, optionally publishes
the prepared line style/colour and clears the prepared byte after rendering.

Template, sort, cache, condition and render implementations remain explicit
native children. The focused contract covers both selector routes, all three
template calls, a child consuming later requests, invalid-kind continuation,
early guard return, render publication and failed-sort partial state. The
native bootstrap now invokes this owner directly and has no top-level child
callbacks. The historical parent oracle still contracts this boundary and is
sequencing evidence rather than a differential proof of the direct owner.
