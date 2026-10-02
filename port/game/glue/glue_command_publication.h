#ifndef FA18_GLUE_COMMAND_PUBLICATION_H
#define FA18_GLUE_COMMAND_PUBLICATION_H
#include "command_publication.h"
const CommandPublicationHooks *glue_command_publication_hooks(void);
int glue_publish_command_event(void);
#endif
