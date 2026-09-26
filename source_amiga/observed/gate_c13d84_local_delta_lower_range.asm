; Byte-exact observed C13D84 local-delta lower-range gate $C143E8-$C143FD.
; It subtracts signed local -$16 from signed local -$1C.  A difference below
; $1E0 takes the small adjustment continuation; the larger difference takes
; the following larger adjustment path.

                org     $C143E8

gate_c13d84_local_delta_lower_range:
                move.w  -$1C(a6),d0
                ext.l   d0
                move.w  -$16(a6),d1
                ext.l   d1
                sub.l   d1,d0
                cmpi.l  #$1E0,d0
                bge.b   $C14406
