; Byte-exact record-table dispatcher continuation $C1F94E-$C1F999.
; Dispatch targets remain individually bounded.

                org     $C1F94E

STREAM_STAGE_SHIFT               equ $C45AB8
RECORD_DISPATCH_TABLE            equ $C1FCE8
RECORD_COUNT_LOCAL               equ -106
RECORD_STATUS_LOCAL              equ -124

continue_c1f94e_record_table_dispatch:
                cmpi.w  #$7FFF,d0
                beq.b   $C1F966
                move.w  STREAM_STAGE_SHIFT.l,d1
                asr.w   d1,d0
                cmp.w   -40(a6),d0
                blt.b   $C1F8F0
                tst.w   (a2)+
                bge.b   $C1F8EC
                move.w  (a2)+,d0
                ble.b   $C1F970
                move.w  (a2)+,d7
                bsr.w   $C1F99A
                move.w  (a2)+,d0
                move.w  d0,d1
                andi.w  #$4000,d1
                beq.b   $C1F97E
                addq.w  #1,RECORD_COUNT_LOCAL(a6)
                andi.w  #$3FFF,d0
                lea     RECORD_DISPATCH_TABLE.l,a0
                movea.l (a0,d0.w),a0
                jsr     (a0)
                blt.w   $C1F7FA
                or.w    d0,RECORD_STATUS_LOCAL(a6)
                bra.w   $C1F90A
