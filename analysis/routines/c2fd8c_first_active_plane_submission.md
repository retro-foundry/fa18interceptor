# `$C2FD8C`: first active cockpit-plane submission

The byte-exact slice `$C2FD8C-$C2FDF3` selects the current four-plane table through `$C456B6`, reads the first longword, adds `$28`, waits until `DMACONR` bit 6 is clear, and starts a blit by writing `BLTSIZE`.

In the bounded attract cockpit trace, `$C456B6` resolves to `$C4567E`; its first entry is `$018980`, so this trigger uses `$0189A8` as both `BLTCPT` and `BLTDPT`. Copper displays the containing range as plane 4. The source establishes a plane-4 blit submission, not the semantic meaning of the pixels or the caller's larger operation.
