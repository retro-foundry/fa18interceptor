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
`$C45650`, and `$C45654` are compared with exact sentinel values. The last
differing zero-extended word is selected, including zero. A nonzero selected
value is formatted with `$C0F56A` and `$C1820C` is changed to `$C113E4`.
A zero selected value invokes `$C17B96` menu audio with `$32`, even if an earlier
word differed. See `native_postflight_text.md` for the complete native owner,
signed-width formatter proof and explicit remaining bootstrap dependency.

This establishes a static caller relationship for `$C0F56A`; the display
purpose of the copied words, sentinel values, and subsequent callback targets
needs a bounded callback trace.
