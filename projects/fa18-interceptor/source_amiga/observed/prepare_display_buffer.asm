; Byte-exact $C0D730-$C0D749 display-buffer preparation gate.
                org     $C0D730

DISPLAY_PREPARATION_FLAGS equ $C458D2
DISPLAY_PREPARATION_SKIP  equ $2000
PREPARE_ACTIVE_PLANES      equ $C2FD8C
PREPARE_ALTERNATE_PATH     equ $C0DA38

prepare_display_buffer:
                move.w  DISPLAY_PREPARATION_FLAGS.l,d0
                andi.w  #DISPLAY_PREPARATION_SKIP,d0
                bne.b   .alternate_path
                jsr     PREPARE_ACTIVE_PLANES.l
                rts
.alternate_path:
                bsr.w   PREPARE_ALTERNATE_PATH
                rts
