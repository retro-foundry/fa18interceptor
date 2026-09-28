# `$C15DB4` display-page topology constraint

The source allocation at `$C15ED0-$C15FA8` creates one five-plane family. Its
five outputs populate `$C18272` and `$C456BE-$C456CE` with `$1F40` strides.
The following setup does **not** allocate a second independent family:

- `$C15FC0-$C15FDE` copies the new lower four planes to
  `$C456D2/$C456D6/$C456DA/$C456DE`.
- `$C15FE8-$C16018` copies those same four lower planes into the second
  display record rooted at `$C18266` before `$C2F4DE` builds renderer tables.

Thus the two View/ViewPort-related configurations built later in this
initializer cannot be treated as proof of two private five-plane render pages.
The separate `$012BC0-$01C7FF` family is a different runtime display target;
its relationship to the newly allocated `$04DB30-$05776F` family must be
recovered from the live selector/Copper timeline.

`flight_page_handoff` remains an unscheduled generic contract owner. Normal
runtime integration must begin with `renderer_page_setup`, then port the
source ViewPort/Copper configuration and page-selection timeline without
collapsing the two structures into an invented double buffer.
