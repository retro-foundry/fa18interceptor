# Throttle-mode tail at `$C1B5DC`

Classification: **structural continuation**. The byte-exact slice
`finish_throttle_mode_update.asm` covers `$C1B5DC-$C1B601`. It follows the
existing `$C1B5B8` selector and calls the adjacent `$C1B602` reset helper.

It then calls `$C33186`, sets one status bit, writes mode value three, toggles
bit `$0800` in `$C46184`, and joins the shared command fallback at `$C1C23C`.
The names are structural; the wider ownership of these fields remains open.
