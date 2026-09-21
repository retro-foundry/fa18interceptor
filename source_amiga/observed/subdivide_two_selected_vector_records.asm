; Byte-exact $C21C2E-$C21C85 vector-record subdivision helper.
; Invoked by the run031 external-aircraft-frame control packet.

                org     $C21C2E

VECTOR_RECORDS_BASE             equ     $C46184
VECTOR_RECORDS_SELECTOR         equ     $C459B6
PROJECTED_VERTEX_TABLE          equ     $C48390

subdivide_two_selected_vector_records:
                lea     VECTOR_RECORDS_BASE.l,a3
                move.w  VECTOR_RECORDS_SELECTOR.l,d0
                addi.w  #$00A4,d0
                adda.w  d0,a3
                bsr.w   subdivide_vector_record
                lea     PROJECTED_VERTEX_TABLE.l,a3
                adda.w  (a2)+,a3
subdivide_vector_record:
                movem.w $0.w(a3),d0-d5
                sub.w   d0,d3
                sub.w   d1,d4
                sub.w   d2,d5
                asr.w   #1,d3
                asr.w   #1,d4
                asr.w   #1,d5
                add.w   d3,d0
                add.w   d4,d1
                add.w   d5,d2
                movem.w d0-d2,$1E(a3)
                sub.w   d3,d0
                sub.w   d4,d1
                sub.w   d5,d2
                asr.w   #1,d3
                asr.w   #1,d4
                asr.w   #1,d5
                add.w   d3,d0
                add.w   d4,d1
                add.w   d5,d2
                movem.w d0-d2,$24(a3)
                moveq   #0,d0
                rts
