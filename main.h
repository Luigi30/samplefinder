#ifndef MAIN_H
#define MAIN_H

/* includes */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <datatypes/datatypesclass.h>
#include <dos/dosasl.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>

#include <proto/asl.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#include "gadgets.h"
#include "playback.h"

/* prototypes */

struct SearchResult
{
    struct Node sr_Node;
    UWORD       sr_Size;
    char        sr_Format[5];
};

extern int main(int argc, char **argv);
char *strlwr(char *);
struct Node *GetListIndex(struct List *list, int index);
void SetupGadgets();
void DoSampleSearch(struct List *, UBYTE *, UBYTE *);
void Event_ProcessGadgetUp(struct IntuiMessage *);

#endif

