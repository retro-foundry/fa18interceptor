; Byte-exact message-layout route to the auxiliary-message return gate
; $C32E58-$C32E5B.

                org     $C32E58

route_message_layout_to_auxiliary_gate:
                dc.w    $6000,$008A             ; bra.w $C32EE6
