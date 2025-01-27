#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include "ComputerSystem.h"
// #include "Hooks.h"
#include <stdio.h>


#define SUCCESS 1
#define PROGRAMDOESNOTEXIST -1
#define PROGRAMNOTVALID -2

// In this version, every process occupies a 60 positions main memory chunk 
// so we can use 60 positions for OS code and the system stack
#define MAINMEMORYSECTIONSIZE (MAINMEMORYSIZE / (PROCESSTABLEMAXSIZE+1))

#define NOFREEENTRY -3
#define TOOBIGPROCESS -4

#define NOPROCESS -1

// V2 Ej 5c
#define SLEEPINGQUEUE

// Contains the possible type of programs
enum ProgramTypes { USERPROGRAM, DAEMONPROGRAM }; 

// Enumerated type containing all the possible process states
enum ProcessStates { NEW, READY, EXECUTING, BLOCKED, EXIT};

// Enumerated type containing the list of system calls and their numeric identifiers
//V1 Ej 12: adding SYSCALL_YIELD
enum SystemCallIdentifiers { SYSCALL_END=3, SYSCALL_YIELD=4, SYSCALL_PRINTEXECPID=5};

// Data structures for multiple queuing
// V1 Ej 11
#define NUMBEROFQUEUES 2
enum TypeOfReadyToRunProcessQueues { USERPROCESSQUEUE, DAEMONSQUEUE};

//V2 ej 1 simulacro
#define NUMBEROFTIMES 3 //Tiempo de respuesta, retorno y espera
#define RESPONSETIME 0
#define RETURNTIME 1
#define WAITINGTIME 2
int processesTimesQueue [PROCESSTABLEMAXSIZE][NUMBEROFTIMES];
int notExecutedYet [PROCESSTABLEMAXSIZE]; //0=not executed yet; 1=already executed


// A PCB contains all of the information about a process that is needed by the OS
typedef struct {
	int busy;
	int initialPhysicalAddress;
	int processSize;
	int state;
	int priority;
	int copyOfPCRegister;
	int copyOfAccumulatorRegister;
	unsigned int copyOfPSWRegister;
	int programListIndex;
	int queueID;
	int whenToWakeUp;  // Exercise 5-a of V2
} PCB;

// These "extern" declaration enables other source code files to gain access
// to the variable listed
extern PCB processTable[];
extern int OS_address_base;
extern int sipID;

// Functions prototypes
void OperatingSystem_Initialize();
// void HOOKEABLE(OperatingSystem_InterruptLogic)(int);
void OperatingSystem_InterruptLogic(int);
// void HOOKEABLE(OperatingSystem_PrepareDaemons)();
// void OperatingSystem_PrepareDaemons();
int OperatingSystem_PrepareStudentsDaemons(int);
void OperatingSystem_UpdateProcessesTimes();

#endif
