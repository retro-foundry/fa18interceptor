; Byte-exact C34900-C34948 shared postflight marker-table walker.
                org $C34900
XOFF equ $C45988
YOFF equ $C458D8
SUBMIT equ $C2F5F4
walk_postflight_marker_table:
                add.w XOFF.l,d0
                cmpi.w #10,d0
                blt.b done
                cmpi.w #$136,d0
                bgt.b done
                add.w YOFF.l,d1
                link.w a6,#-16
                move.w d0,-2(a6)
                move.w d1,-4(a6)
loop:           move.b (a0)+,d0
                move.b (a0)+,d1
                move.b d0,d2
                or.b d1,d2
                beq.b end
                ext.w d0
                ext.w d1
                add.w -2(a6),d0
                add.w -4(a6),d1
                move.l a0,-(a7)
                jsr SUBMIT.l
                movea.l (a7)+,a0
                bra.b loop
end:            unlk a6
done:           rts
