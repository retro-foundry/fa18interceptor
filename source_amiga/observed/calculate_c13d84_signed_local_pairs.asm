; Byte-exact observed C13D84 continuation $C14ABE-$C14B09.
; It applies the bounded signed-word adjustment helper twice, then derives a
; local word from current-record +$42 under a mode gate.

                org     $C14ABE

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_OFFSET_LONG_42           equ     $42
CONTROL_SCALE_MODE              equ     $C457A0
SIGNED_WORD_ADJUST_HELPER       equ     $C15138

calculate_c13d84_signed_local_pairs:
                move.w  -$2(a6),d0
                ext.l   d0
                move.w  -$8(a6),d1
                ext.l   d1
                move.l  d1,-(sp)
                move.l  d0,-(sp)
                bsr.w   SIGNED_WORD_ADJUST_HELPER
                addq.l  #8,sp
                move.w  -$6(a6),d1
                ext.l   d1
                move.w  -$c(a6),d2
                ext.l   d2
                move.l  d2,-(sp)
                move.l  d1,-(sp)
                move.w  d0,-$2(a6)
                bsr.w   SIGNED_WORD_ADJUST_HELPER
                addq.l  #8,sp
                move.w  d0,-$6(a6)
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  RECORD_OFFSET_LONG_42(a0),d0
                asr.l   #2,d0
                move.w  d0,-$a(a6)
                tst.b   CONTROL_SCALE_MODE.l
                beq.b   $C14B16
