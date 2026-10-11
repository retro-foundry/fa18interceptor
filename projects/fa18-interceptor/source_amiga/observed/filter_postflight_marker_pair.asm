; Byte-exact C34876-C348B1 marker-pair filter, submit, and returns.
                org $C34876
SUBMIT_MARKER_PAIR equ $C2F66E
filter_postflight_marker_pair:
                ext.w d0
                ext.w d1
                add.w -2(a6),d0
                add.w -4(a6),d1
                cmpi.w #2,d0
                ble.b marker_done
                cmpi.w #$13E,d0
                bge.b marker_done
                cmp.w -6(a6),d0
                ble.b marker_done
                cmp.w -8(a6),d0
                bge.b marker_done
                cmp.w -10(a6),d1
                ble.b marker_done
                cmp.w -12(a6),d1
                bge.b marker_done
                jsr SUBMIT_MARKER_PAIR.l
marker_done:     unlk a6
                rts
marker_return_stub:
                rts
