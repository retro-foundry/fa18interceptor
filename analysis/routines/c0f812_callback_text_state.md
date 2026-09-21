# `$C0F812`: callback text-state initializer

## Evidence

Static disassembly of `$C0F812-$C0F91F`, reconstructed byte-for-byte in
`source_amiga/observed/initialize_callback_text_state.asm`. `$C0F4D8` stores
this address in `$C1820C`, making it the initial callback for the bounded
post-input tick.

## Static behavior

The routine copies 32 words from `$C08510` to the pointer held at `$C45660`.
It clears `$C458A6`, sets `$C457AD` and `$C45857` to `$FF`, sets `$C45AD6` to
three, and installs `$C11446` into `$C1820C`.

It then builds a small field at `$C3F055`. Three input words at `$C4564C`,
`$C45650`, and `$C45654` are compared with exact sentinel values. If at least
one differs, the last differing zero-extended word is formatted with the
verified `$C0F56A` fixed-width hexadecimal formatter and `$C1820C` is changed
to `$C113E4`. If all match, it invokes `$C17B96` with `$32`.

This establishes a static caller relationship for `$C0F56A`; the display
purpose of the copied words, sentinel values, and subsequent callback targets
needs a bounded callback trace.
