; Byte-exact relative-record component test $C1FB8C-$C1FBD3.
; Component ownership remains structural; return is D7/condition codes.

                org     $C1FB8C

CONTROL_BASE_LOCAL              equ -44
STREAM_STAGE_SHIFT               equ $C45AB8
RECORD_COMPONENT_X              equ $C45B2A
RECORD_COMPONENT_Z              equ $C45B2E

test_c1fb8c_relative_record_components:
                andi.w  #$3000,d7
                beq.b   $C1FBD4
                movea.l CONTROL_BASE_LOCAL(a6),a0
                adda.w  (a2)+,a0
                movem.w (a0),d0-d5
                move.w  STREAM_STAGE_SHIFT.l,d7
                asr.w   d7,d0
                asr.w   d7,d1
                asr.w   d7,d2
                add.w   RECORD_COMPONENT_X.l,d0
                add.w   RECORD_COMPONENT_Z.l,d2
                sub.w   -38(a6),d0
                sub.w   -36(a6),d1
                sub.w   -34(a6),d2
                muls.w  d3,d0
                muls.w  d4,d1
                muls.w  d5,d2
                add.l   d0,d2
                add.l   d1,d2
                blt.b   $C1FBD0
                moveq   #1,d7
                rts
                clr.w   d7
                rts
