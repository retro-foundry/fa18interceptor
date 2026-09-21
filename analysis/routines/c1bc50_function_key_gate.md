# Function-key gate at `$C1BC50`

Classification: **structural and behavioural**. The byte-exact source
`route_function_key_level_input.asm` covers `$C1BC50-$C1BD03` and joins
`apply_function_key_throttle_level.asm` at `$C1BD04`.

The observed run014 F1 packet enters this gate with raw `$50`, passes the
state guards, reaches `$C1BD04`, and stores `$0C` in `$C45870`. The sealed
run003 F10 packet reaches the same path with raw `$59` and stores `$79`.

The gate also contains alternative state-controlled paths through `$C45791`,
`$C45787`, `$C4584B`, `$C458A6`, `$C45879`, and `$C457AE`. Their wider game
semantics have not been assigned; the source preserves the exact comparisons,
range checks, writes, and branch targets with neutral names.

Evidence:

- `pcode/raw/run014_f1_keyboard_poll/`
- `pcode/raw/run003_f10_exact_keyboard_poll/`
- `analysis/function_key_levels.md`
