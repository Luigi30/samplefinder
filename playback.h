#ifndef PLAYBACK_H
#define PLAYBACK_H

#define NO  (0)
#define YES (1)

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>

#include <datatypes/datatypesclass.h>
#include <datatypes/soundclass.h>
#include <devices/audio.h>
#include <exec/memory.h>
#include <exec/types.h>

#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

/*  These are Paula periods, not freuqencies!
    Source: www.eblong.com/zarf/blorb/mod-spec.txt */
#define NOTE_C1 856
#define NOTE_C2 428
#define NOTE_C3 214

extern ULONG SIG_PLAYBACK_FINISHED; //Signal flag for when an 8SVX sound is done playing.
extern APTR PB_SoundObject;

void PB_PlaySound(struct Window *window, char *fileName, LONG frequency);
void PB_DisposeSound();
void PB_PlayPCM(char *fileName, LONG frequency);

#endif

