# `$C1B27E` control-record stream advance

Classification: **structural with one bounded branch trace**. For control-mode
values 1–3 and a zero activity word, this route advances a byte stream and a
word stream, publishes their observed values, wraps the word pointer by its
count, and detects three consecutive `$FF` bytes or the byte-stream end.

The sealed frame-5 trace establishes the other entry outcome: mode zero takes
the direct branch to `$C1B340`. It does not exercise this stream-advance path.
The pointer and stream labels describe only the byte-exact data movement.
