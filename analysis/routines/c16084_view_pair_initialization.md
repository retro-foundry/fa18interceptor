# `$C16084-$C16126`: View/ViewPort pair construction and publication

Classification: **bounded display-pair initialization**.

Authority is the cold-boot replay `captures/cold_boot_menu_init/`.  Its
initial state has all sixteen bytes at `$C182BA-$C182C9` zero.  During ordinary
replay, slot zero first becomes nonzero at frame 5,926 and slot one first
becomes nonzero at frame 7,699:

| Slot | View table write | Display-instruction table write | Resulting observed pair |
| --- | --- | --- | --- |
| 0 | `$C160BE`: `$C1821C -> $C182BA` | `$C160C8`: `$C18232 -> $C182C2` | `$C074D8`, `$C07F00` |
| 1 | `$C16110`: `$C1821C -> $C182BE` | `$C1611A`: `$C18232 -> $C182C6` | `$C01268`, `$C01488` |

The slot-zero witness was captured with:

```text
python scripts/trace_instruction_memory_writes.py \
  --restore captures/cold_boot_menu_init/initial_state.bin \
  --playback captures/cold_boot_menu_init/playback.e9k \
  --breakpoint 0xFE5A70 --arm-frame 5926 --frames 5930 \
  --watch-address 0xC182B0 --watch-size 0x20 \
  --max-instructions 30000 --context-instructions 16 \
  --output build/cold_boot_c182ba_fe5a70_write_trace
```

The analogous slot-one trace is
`build/cold_boot_c182be_fe5a70_write_trace/`.  Both table writes occur after
the pair-specific graphics-library calls, not in `$C1612C`; that child only
selects and republishes an already-built table entry.

The source leaves are structurally parallel:

```text
$C16084: copy first pair's source field; set ViewPort construction fields
          call graphics wrapper `$C53F18` with `$C18218`
          call graphics wrapper `$C53F04` with `$C18218`
$C160BE: copy live `$C1821C/$C18232` into table slot 0

$C160D6: clear live `$C1821C/$C18232`; copy second pair's source field
          call the same `$C53F18`, then `$C53F04` wrappers
$C16110: copy live `$C1821C/$C18232` into table slot 1
```

An instruction-level write trace over `$C18218-$C18267`, starting at the
slot-zero frame boundary, proves the two calls' distinct live outputs:

- `$FCC6E4: MOVE.L D2,$08(A2)` writes `$C07F00` at `$C18232`;
- `$FCA67C: MOVE.L D3,$04(A2)` writes `$C074D8` at `$C1821C`.

The concrete library-vector names are not assigned here.  The native port
must therefore preserve the proven ownership boundary: a page/view constructor
creates native View and display-instruction identities, then the initializer
publishes their pair into slot zero or one.  It must not import the observed
Amiga addresses as native identities, and it must not make `$C1612C` a pair
creator.
