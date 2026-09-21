# `toggle_hud_mode` at `$C1B264` (Hunk 5 +`$6604`)

Classification: **behavioural**. A run003 `H` press reaches the raw-key
dispatcher with `D0 = $25`; its equality branch at `$C1AEB8` enters this
slice. The isolated, no-future-input trace returns to `$C0F45C` after 109
instructions and stores the raw event through the common queue at `$C1C23C`.
Its raw P-code packet is `pcode/raw/run003_h_key_dispatch/`.

The handler increments `$C457A1`, wraps it to zero when it exceeds one, and
then enters the shared command fallback. Thus `$C457A1` is a two-state HUD
mode field for this observed flight state. The field's two visual modes have
not yet been separated frame-by-frame.
