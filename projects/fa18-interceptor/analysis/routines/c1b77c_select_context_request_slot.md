# `$C1B77C` context request-slot selectors

Classification: **structural with byte-exact source**. Ten entries select
values 0–9 for `$C458B2`; selected entries set one request bit first. The
common tail writes `$FF` at `$C45858` and, when needed, `$C458B0`, then joins
the common command queue.

The source exposes the exact selector mechanics but does not attach gameplay
names to values or request bits without bounded raw-key traces.
