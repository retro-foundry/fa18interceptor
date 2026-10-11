; Byte-exact return block for the $C20A52 record-projection branch.

                org     $C20C10

return_c20a52_record_projection:
                subq.w  #1,-$3a(a6)
                bgt.w   $C20B92
                movem.l (sp)+,a1-a2/a5
                move.w  -$7e(a6),d0
                rts
