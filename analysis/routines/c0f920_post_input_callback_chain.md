# `$C0F920-$C0F991`: post-input callback chain

## Evidence

Static 68000 disassembly, reconstructed byte-for-byte in
`source_amiga/observed/run_post_input_callback_chain.asm`. These are callback
addresses explicitly installed by the static `$C0F5F8` post-input tick.

## Static transitions

`$C0F920` calls `$C08F26`, clears `$C458AC`, `$C458A6`, and `$C4584B`, then
installs `$C0FBE0` in `$C1820C`.

`$C0F946` waits for a negative `$C45AD6` and equal bytes `$C458A0/$C458A1`,
then stores two and installs `$C0F974`.

`$C0F974` waits for a negative `$C45AD6`, sets `$C45795`, and installs
`$C0F992`. The runtime conditions and gameplay purpose of these transitions
remain unobserved.
