#ifndef OPERATINGSYSTEMBASE_H
#define OPERATINGSYSTEMBASE_H

#include "ComputerSystem.h"
#include "OperatingSystem.h"
#include "Heap.h"
// #include "Hooks.h"
#include <stdio.h>

/*******************************************************************************
 * HOOKS
 ***/

// void HOOK(OperatingSystem_InterruptLogic)(int);
// void HOOK(OperatingSystem_PrepareDaemons)(int);
/******************************************************************************/

// Prototypes of OS functions that students should not change
int OperatingSystem_ObtainAnEntryInTheProcessTable();
int OperatingSystem_ObtainProgramSize(FILE *);
int OperatingSystem_ObtainPriority(FILE *);
int OperatingSystem_LoadProgram(FILE *, int, int);
void OperatingSystem_ReadyToShutdown();
void OperatingSystem_TerminatingSIP();
void OperatingSystem_PrepareDaemons(int);
int OperatingSystem_PrepareTeachersDaemons(int);

#endif
