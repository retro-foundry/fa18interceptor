# Demonstration-flight renderer component census

Classification: **renderer/control address census, not placement or mesh ownership**.

The bounded flight trace reaches the renderer repeatedly. This report retains every distinct descriptor field, immutable transform source, control pointer, and submission context without assigning feature names or joining them to flight placement records.

## `descriptor_field_store`

Events: **5**

| address | count |
| --- | ---: |
| $C3B5A6 | 4 |
| $C44640 | 1 |

## `stream_cursor_publish`

Events: **12**

| address | count |
| --- | ---: |
| $C34A56 | 4 |
| $C3925A | 4 |
| $C3B5B4 | 4 |

## `transform_source`

Events: **4**

| address | count |
| --- | ---: |
| $C3B62E | 4 |

## `control_walker_entry`

Events: **12**

| address | count |
| --- | ---: |
| $C3515A | 4 |
| $C39E2C | 4 |
| $C3B646 | 4 |

## `control_stream_load`

Events: **12**

| address | count |
| --- | ---: |
| $C34A56 | 4 |
| $C3925A | 4 |
| $C3B5B4 | 4 |

## `line_submit`

Events: **32**

| address | count |
| --- | ---: |
| $000005 | 16 |
| $FFBD28 | 16 |

## `polygon_submit`

Events: **29**

| address | count |
| --- | ---: |
| $C3B5B6 | 8 |
| $C4BFA6 | 4 |
| $C4BFAC | 4 |
| $C4BFBE | 4 |
| $C4BFE2 | 4 |
| $C4BFE8 | 5 |

This is a renderer/control census of an ordinary demo-flight trace. It does not join a flight placement record to these addresses and therefore does not prove named buildings, landmarks, complete models, or per-instance primitive ownership.
