; Byte-exact observed C1EE14 stream continuation $C1EDE8-$C1EDF5.
; A $FFFF sentinel returns to the preceding stream step; otherwise the shared
; cursor advances by four before the C1EE14 continuation.

                org     $C1EDE8

STREAM_CURSOR                   equ     $C45A36

advance_c1ee14_stream_cursor:
                cmpi.w  #-$1,d0
                beq.b   $C1EDE4
                addq.l  #4,STREAM_CURSOR.l
                bra.b   $C1EE14
