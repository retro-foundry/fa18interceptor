; Byte-exact observed context-refresh stage prefix $C1E328-$C1E371.
; It selects an indexed six-byte descriptor from C459CE, establishes a working
; output base, and bounds the descriptor count before the subsequent loop.

                org     $C1E328

REFRESH_STAGE_ENABLE            equ     $C457A5
REFRESH_DESCRIPTOR_BASE         equ     $C459CE
REFRESH_DESCRIPTOR_INDEX        equ     $C4585D
REFRESH_OUTPUT_BASE             equ     $C4E778

initialize_c1e328_refresh_stage:
                movem.l d0-d7/a0-a5,-(sp)
                tst.b   REFRESH_STAGE_ENABLE.l
                beq.w   $C1E4A0
                lea     REFRESH_DESCRIPTOR_BASE.l,a0
                move.b  REFRESH_DESCRIPTOR_INDEX.l,d0
                subq.b  #1,d0
                blt.w   $C1E484
                ext.w   d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d0,d0
                add.w   d1,d1
                add.w   d1,d0
                adda.w  d0,a0
                tst.w   (a0)
                blt.w   $C1E4A0
                lea     REFRESH_OUTPUT_BASE.l,a3
                movea.l (a0)+,a1
                lea     (a1),a2
                move.w  (a0)+,d0
                ble.b   $C1E3AC
                move.w  d0,d7
                cmpi.w  #$16,d7
                ble.b   $C1E376
