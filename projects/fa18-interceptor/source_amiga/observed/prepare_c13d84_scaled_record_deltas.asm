; Byte-exact observed $C13D84 continuation $C14B16-$C14B7D.
; It adjusts a signed frame word, optionally adds record +$76, then prepares
; three longword-scaled local deltas.  Field semantics remain unassigned.

                org     $C14B16

CURRENT_CONTROL_RECORD          equ     $C18210
CONTROL_ADJUST_FLAGS            equ     $C4579F
CONTROL_SCALE_MODE              equ     $C457A0
RECORD_OFFSET_WORD_76           equ     $76
SIGNED_WORD_ADJUST_HELPER       equ     $C15138

prepare_c13d84_scaled_record_deltas:
                move.w  -$4(a6),d0
                ext.l   d0
                move.w  -$a(a6),d1
                ext.l   d1
                move.l  d1,-(sp)
                move.l  d0,-(sp)
                bsr.w   SIGNED_WORD_ADJUST_HELPER
                addq.l  #8,sp
                move.b  CONTROL_ADJUST_FLAGS.l,d1
                move.w  d0,-$4(a6)
                btst    #0,d1
                bne.b   $C14B4A
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  RECORD_OFFSET_WORD_76(a0),d1
                add.w   d1,-$4(a6)
                move.w  -$2(a6),d0
                ext.l   d0
                asl.l   #2,d0
                move.w  -$4(a6),d1
                ext.l   d1
                asl.l   #2,d1
                move.w  -$6(a6),d2
                ext.l   d2
                asl.l   #2,d2
                move.l  #$6000,-$1c(a6)
                move.l  d0,-$10(a6)
                move.l  d1,-$14(a6)
                move.l  d2,-$18(a6)
                tst.b   CONTROL_SCALE_MODE.l
                beq.b   $C14BB0
