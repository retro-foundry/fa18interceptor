; Byte-exact observed C1E540 record-transform continuation $C1E5E0-$C1E5EF.
; It moves the three prepared address results into the working address
; registers, initializes D7's observed fixed limit, and tests the D5 count.

                org     $C1E5E0

initialize_c1e540_record_transform_stream:
                movea.l d2,a3
                movea.l d3,a4
                movea.l d4,a5
                move.l  #$c00,d7
                subq.w  #1,d5
                blt.b   $C1E640
