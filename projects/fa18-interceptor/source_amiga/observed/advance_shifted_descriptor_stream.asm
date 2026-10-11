; Byte-exact observed descriptor-stream continuation $C09708-$C0972F.
; It retains D2 locally, then advances the shared descriptor cursor while the
; shifted signed descriptor word remains below D2.  The surrounding caller
; supplies the resulting condition-code path at $C09730/$C09732.

                org     $C09708

DESCRIPTOR_STREAM_CURSOR        equ     $C45A36
DESCRIPTOR_STREAM_SHIFT         equ     $C45AB8

advance_shifted_descriptor_stream:
                move.w  d2,-$28(a6)
.next_descriptor:
                movea.l DESCRIPTOR_STREAM_CURSOR.l,a2
                move.w  (a2)+,d0
                blt.b   $C09732
                move.w  DESCRIPTOR_STREAM_SHIFT.l,d1
                asr.w   d1,d0
                cmp.w   d2,d0
                bge.b   $C09730
                move.l  (a2),DESCRIPTOR_STREAM_CURSOR.l
                bge.b   .next_descriptor
                moveq   #0,d0
                unlk    a6
                rts
