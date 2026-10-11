# `$C1B9CC` context command selection publisher

Classification: **structural with byte-exact source**. The common continuation
stores the selected index, invokes `$C08324`, resets observed latches, reads a
record type nibble, and either queues immediately or selects output words using
the index and the byte lookup table at `$C1BAD4`. It invokes `$C082B8` on the
lookup path before publishing `$C45984/$C45986/$C45988`.

The output-field labels describe exact stores and lookup mechanics. Their UI or
gameplay meanings require frame-bounded evidence.
