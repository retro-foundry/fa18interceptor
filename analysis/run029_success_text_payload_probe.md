# Run029 success-text payload probe

The exact static success payload is `$C3FC3C-$C3FCD3`: `LANDING SUCCESSFUL`,
`YOU ARE NOW QUALIFIED FOR MISSIONS`, and the adjacent qualification-return
lines. Two normal-replay monitors tested its established text presentation
routes:

| Replay interval | Probe | Result |
|---|---|---|
| 1--5,000 | `$C32D24` selector 74 | Not invoked; five other selectors are recorded in `run029_message_selector_monitor.md`. |
| 5,001--14,202 | `$C32D24` selector 74 | No `$C32D24` invocation. |
| 1--5,000 | `$C32FCE` glyph cursor in `$C3FC3C-$C3FCE0` | 188 glyph-reader hits, none in the payload range. |
| 5,001--14,202 | `$C32FCE` glyph cursor in `$C3FC3C-$C3FCE0` | No glyph-reader hits. |

This rules out the bounded selector and static-glyph paths as the observed
presentation route in this recording. Together with the final cockpit frame,
it means run029 cannot currently support a successful qualification claim. It
does not prove that its flight failed; the actual successful recording must be
identified separately. The qualification status writer remains unassigned.
