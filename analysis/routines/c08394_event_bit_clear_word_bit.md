# `$C08394` event-bit / command-word-bit sibling

Classification: **structural state effect**.

The `$00E8` branch of `$C16EAE` directly calls this complete 18-byte leaf. It
sets bit 3 of `$C4599A`, clears bit 3 of `$C458C6` via mask `$FFF7`, and
returns. Its adjacency to the `$C0833E-$C08393` Space-command helper does not
prove shared game semantics; only these field effects are assigned.
