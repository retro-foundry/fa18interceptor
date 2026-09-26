; Byte-exact observed C23116 record-descriptor continuation $C23160-$C23169.
; It initializes D0 to zero on one route; the adjacent route loops until the
; byte in D0 equals the observed class value three.

                org     $C23160

loop_c23116_record_descriptor_class:
                moveq   #0,d0
                bra.b   $C23170
                cmpi.b  #3,d0
                bne.b   $C23160
