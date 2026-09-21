# Record-stage gate at `$C25A6A`

Classification: **behavioural early-return leaf**. `$C1B3E2` calls `$C25A6A`
in the sealed run003 frame-6,000 control-record packet. `$C45790` is zero, so
it returns through `$C25A68` to `$C1B3E8` in three instructions. Canonical
P-code is `pcode/raw/run003_6000_c25a6a/`. The complete 10-byte gate is
byte-exact source in `source_amiga/observed/check_record_stage_enable.asm`.
