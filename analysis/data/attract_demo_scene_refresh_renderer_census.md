# Demonstration-flight renderer component census

Classification: **renderer/control address census, not placement or mesh ownership**.

The bounded flight trace reaches the renderer repeatedly. This report retains every distinct descriptor field, immutable transform source, control pointer, and submission context without assigning feature names or joining them to flight placement records.

## `descriptor_field_store`

Events: **3**

| address | count |
| --- | ---: |
| $C3B4F8 | 2 |
| $C44500 | 1 |

## `stream_cursor_publish`

Events: **6**

| address | count |
| --- | ---: |
| $C34A5E | 3 |
| $C3924C | 3 |

## `transform_source`

Events: **0**

| address | count |
| --- | ---: |

## `control_walker_entry`

Events: **6**

| address | count |
| --- | ---: |
| $C351E2 | 3 |
| $C39B7C | 3 |

## `control_stream_load`

Events: **6**

| address | count |
| --- | ---: |
| $C34A5E | 3 |
| $C3924C | 3 |

## `line_submit`

Events: **88**

| address | count |
| --- | ---: |
| $0001AA | 3 |
| $00026A | 3 |
| $C34A9A | 43 |
| $C392A8 | 36 |
| $C4BFD6 | 3 |

## `polygon_submit`

Events: **64**

| address | count |
| --- | ---: |
| $0001AA | 3 |
| $00026A | 3 |
| $C34A9A | 31 |
| $C392A8 | 3 |
| $C45BEA | 15 |
| $C4BFBE | 3 |
| $C4BFD6 | 3 |
| $C4BFE2 | 3 |

This is a renderer/control census of an ordinary demo-flight trace. It does not join a flight placement record to these addresses and therefore does not prove named buildings, landmarks, complete models, or per-instance primitive ownership.
