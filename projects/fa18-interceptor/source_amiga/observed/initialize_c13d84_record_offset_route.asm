; Byte-exact observed C13D84 record-offset route prefix $C14466-$C1447B.
; It initializes the offset local, compares the paired signed locals, and
; routes the positive paired-local case to the adjacent negative-offset path.

                org     $C14466

initialize_c13d84_record_offset_route:
                moveq   #0,d0
                move.w  d0,-$22(a6)
                move.w  -$1C(a6),d0
                cmp.w   -$16(a6),d0
                bge.b   $C144BC
                tst.w   -$18(a6)
                ble.b   $C14484
