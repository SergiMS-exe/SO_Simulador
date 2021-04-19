#include "OperatingSystem.h"
#include "OperatingSystemBase.h"
#include "MMU.h"
#include "Processor.h"
#include "Buses.h"
#include "Heap.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

// Functions prototypes
void OperatingSystem_PCBInitialization(int, int, int, int, int);
void OperatingSystem_MoveToTheREADYState(int);
void OperatingSystem_Dispatch(int);
void OperatingSystem_RestoreContext(int);
void OperatingSystem_SaveContext(int);
void OperatingSystem_TerminateProcess();
int OperatingSystem_LongTermScheduler();
void OperatingSystem_PreemptRunningProcess();
int OperatingSystem_CreateProcess(int);
int OperatingSystem_ObtainMainMemory(int, int);
int OperatingSystem_ShortTermScheduler();
int OperatingSystem_ExtractFromReadyToRun();
void OperatingSystem_HandleException();
void OperatingSystem_HandleSystemCall();
void OperatingSystem_PrintReadyToRunQueue();
int OperatingSystem_GiveControl();
void OperatingSystem_HandleClockInterrupt();
void OperatingSystem_MoveToTheBLOCKEDState();


// The process table
PCB processTable[PROCESSTABLEMAXSIZE];

// Address base for OS code in this version
int OS_address_base = PROCESSTABLEMAXSIZE * MAINMEMORYSECTIONSIZE;

// Identifier of the current executing process
int executingProcessID=NOPROCESS;

// Variable containing the number of interruption clocks (V2 Ej 4)
int numberOfClockInterrupts = 0; 

// Identifier of the System Idle Process
int sipID;

// Initial PID for assignation
int initialPID=PROCESSTABLEMAXSIZE-1;

// Begin indes for daemons in programList
int baseDaemonsInProgramList; 

// Variable containing the number of not terminated user processes
int numberOfNotTerminatedUserProcesses=0;

// Array that contains the processes' states
// V1 Ej10
char * statesNames [5]={"NEW","READY","EXECUTING","BLOCKED","EXIT"}; 

// Data structures for multiple queuing
// V1 Ej 11
heapItem readyToRunQueue [NUMBEROFQUEUES][PROCESSTABLEMAXSIZE];
int numberOfReadyToRunProcesses[NUMBEROFQUEUES]={0,0};
char * queueNames [NUMBEROFQUEUES]={"USER","DAEMONS"};

// Exercise 5-b of V2 
// Heap with blocked processes sort by when to wakeup 
heapItem sleepingProcessesQueue[PROCESSTABLEMAXSIZE]; 
int numberOfSleepingProcesses=0;

// Initial set of tasks of the OS
void OperatingSystem_Initialize(int daemonsIndex) {
	int i, selectedProcess;
	FILE *programFile; // For load Operating System Code
	programFile=fopen("OperatingSystemCode", "r");
	if (programFile==NULL){
		OperatingSystem_ShowTime(SHUTDOWN);
		// Show red message "FATAL ERROR: Missing Operating System!\n"
		ComputerSystem_DebugMessage(99,SHUTDOWN,"FATAL ERROR: Missing Operating System!\n");
		exit(1);		
	}

	// Obtain the memory requirements of the program
	int processSize=OperatingSystem_ObtainProgramSize(programFile);

	// Load Operating System Code
	OperatingSystem_LoadProgram(programFile, OS_address_base, processSize);
	
	// Process table initialization (all entries are free)
	for (i=0; i<PROCESSTABLEMAXSIZE;i++){
		processTable[i].busy=0;
	}
	// Initialization of the interrupt vector table of the processor
	Processor_InitializeInterruptVectorTable(OS_address_base+2);
		
	// Include in program list  all system daemon processes
	OperatingSystem_PrepareDaemons(daemonsIndex);
	
	ComputerSystem_FillInArrivalTimeQueue(); //V3 Ej 0c

	OperatingSystem_PrintStatus(); //V3 Ej 0d

	// Create all user processes from the information given in the command line
	int createdProcesses = OperatingSystem_LongTermScheduler();
	//V1 Ej 15
	if (createdProcesses==0){
		OperatingSystem_ReadyToShutdown();
		return; 
	}

		
	if (strcmp(programList[processTable[sipID].programListIndex]->executableName,"SystemIdleProcess")) {
		OperatingSystem_ShowTime(SHUTDOWN);
		// Show red message "FATAL ERROR: Missing SIP program!\n"
		ComputerSystem_DebugMessage(99,SHUTDOWN,"FATAL ERROR: Missing SIP program!\n");
		exit(1);		
	}

	// At least, one user process has been created
	// Select the first process that is going to use the processor
	selectedProcess=OperatingSystem_ShortTermScheduler();

	// Assign the processor to the selected process
	OperatingSystem_Dispatch(selectedProcess);

	// Initial operation for Operating System
	Processor_SetPC(OS_address_base);
	
}

// Daemon processes are system processes, that is, they work together with the OS.
// The System Idle Process uses the CPU whenever a user process is able to use it
// void HOOKEABLE(OperatingSystem_PrepareDaemons)(int programListDaemonsBase) {
int OperatingSystem_PrepareStudentsDaemons(int programListDaemonsBase) {

	// Prepare aditionals daemons here
	// index for aditionals daemons program in programList
	// programList[programListDaemonsBase]=(PROGRAMS_DATA *) malloc(sizeof(PROGRAMS_DATA));
	// programList[programListDaemonsBase]->executableName="studentsDaemonNameProgram";
	// programList[programListDaemonsBase]->arrivalTime=0;
	// programList[programListDaemonsBase]->type=DAEMONPROGRAM; // daemon program
	// programListDaemonsBase++

	return programListDaemonsBase;
};


// The LTS is responsible of the admission of new processes in the system.
// Initially, it creates a process from each program specified in the 
// 			command lineand daemons programs
int OperatingSystem_LongTermScheduler() {
  
	int PID, i,
		numberOfSuccessfullyCreatedProcesses=0, newProgram;
	
	newProgram=OperatingSystem_IsThereANewProgram();

	while (newProgram==YES){
		i=Heap_poll(arrivalTimeQueue,QUEUE_ARRIVAL,&numberOfProgramsInArrivalTimeQueue);
		PID=OperatingSystem_CreateProcess(i);
		switch (PID) {
			case NOFREEENTRY:
				OperatingSystem_ShowTime(ERROR);
				ComputerSystem_DebugMessage(103, ERROR, programList[i]->executableName);
				break;
			case PROGRAMDOESNOTEXIST:
				OperatingSystem_ShowTime(ERROR);
				ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "it does not exists");
				break;
			case PROGRAMNOTVALID:
				OperatingSystem_ShowTime(ERROR);
				ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "invalid priority or size");
				break;
			case TOOBIGPROCESS:
				OperatingSystem_ShowTime(ERROR);
				ComputerSystem_DebugMessage(105, ERROR, programList[i]->executableName);
				break;
			default:
				numberOfSuccessfullyCreatedProcesses++;
				if (programList[i]->type==USERPROGRAM) 
					numberOfNotTerminatedUserProcesses++;
				// Move process to the ready state
				OperatingSystem_MoveToTheREADYState(PID);
				break;
		}
		newProgram=OperatingSystem_IsThereANewProgram();
	}
	if (numberOfSuccessfullyCreatedProcesses==0 && newProgram==EMPTYQUEUE)
		OperatingSystem_ReadyToShutdown();
	
	else
		//V2 Ej 7d
		OperatingSystem_PrintStatus();
	
	// Return the number of succesfully created processes
	return numberOfSuccessfullyCreatedProcesses;
}


// This function creates a process from an executable program
int OperatingSystem_CreateProcess(int indexOfExecutableProgram) {
  
	int PID;
	int processSize;
	int loadingPhysicalAddress;
	int priority;
	FILE *programFile;
	PROGRAMS_DATA *executableProgram=programList[indexOfExecutableProgram];

	// Obtain a process ID
	PID=OperatingSystem_ObtainAnEntryInTheProcessTable();
	if (PID==NOFREEENTRY)
		return NOFREEENTRY;

	// Check if programFile exists
	programFile=fopen(executableProgram->executableName, "r");
	if (programFile==NULL){
		return PROGRAMDOESNOTEXIST;
	}

	// Obtain the memory requirements of the program
	processSize=OperatingSystem_ObtainProgramSize(programFile);	

	// Obtain the priority for the process
	priority=OperatingSystem_ObtainPriority(programFile);
	
	if (processSize==PROGRAMNOTVALID || priority==PROGRAMNOTVALID)
		return PROGRAMNOTVALID;

	// Obtain enough memory space
 	loadingPhysicalAddress=OperatingSystem_ObtainMainMemory(processSize, PID);

	if (loadingPhysicalAddress==TOOBIGPROCESS)
		return TOOBIGPROCESS;

	// Load program in the allocated memory
	int programLoaded = OperatingSystem_LoadProgram(programFile, loadingPhysicalAddress, processSize);

	if (programLoaded==TOOBIGPROCESS)
		return TOOBIGPROCESS;
	
	// PCB initialization
	OperatingSystem_PCBInitialization(PID, loadingPhysicalAddress, processSize, priority, indexOfExecutableProgram);
	
	OperatingSystem_ShowTime(INIT);
	// Show message "Process [PID] created from program [executableName]\n"
	ComputerSystem_DebugMessage(70,INIT,PID,executableProgram->executableName);
	
	return PID;
}


// Main memory is assigned in chunks. All chunks are the same size. A process
// always obtains the chunk whose position in memory is equal to the processor identifier
int OperatingSystem_ObtainMainMemory(int processSize, int PID) {

 	if (processSize>MAINMEMORYSECTIONSIZE)
		return TOOBIGPROCESS;
	
 	return PID*MAINMEMORYSECTIONSIZE;
}


// Assign initial values to all fields inside the PCB
void OperatingSystem_PCBInitialization(int PID, int initialPhysicalAddress, int processSize, int priority, int processPLIndex) {

	processTable[PID].busy=1;
	processTable[PID].initialPhysicalAddress=initialPhysicalAddress;
	processTable[PID].processSize=processSize;
	processTable[PID].state=NEW;
	processTable[PID].priority=priority;
	processTable[PID].programListIndex=processPLIndex;
	processTable[PID].queueID=programList[processPLIndex]->type;
	processTable[PID].copyOfAccumulatorRegister = 0; // V1-ex13
	
	OperatingSystem_ShowTime(SYSPROC);
	//V1 Ej 10 Printing moving state message
	ComputerSystem_DebugMessage(110, SYSPROC, PID, programList[processTable[PID].programListIndex]->executableName);
	// Daemons run in protected mode and MMU use real address
	if (programList[processPLIndex]->type == DAEMONPROGRAM) {
		processTable[PID].copyOfPCRegister=initialPhysicalAddress;
		processTable[PID].copyOfPSWRegister= ((unsigned int) 1) << EXECUTION_MODE_BIT;
	} 
	else {
		processTable[PID].copyOfPCRegister=0;
		processTable[PID].copyOfPSWRegister=0;
	}

}


// Move a process to the READY state: it will be inserted, depending on its priority, in
// a queue of identifiers of READY processes
void OperatingSystem_MoveToTheREADYState(int PID) {
	int index = processTable[PID].queueID;
	if (Heap_add(PID, readyToRunQueue[index],QUEUE_PRIORITY ,&numberOfReadyToRunProcesses[index] ,PROCESSTABLEMAXSIZE)>=0) {
		// Save the previous state to print it V1 Ej10
		int prevState = processTable[PID].state;
		processTable[PID].state=READY;
		OperatingSystem_ShowTime(SYSPROC);
		//V1 Ej 10 Printing moving state message
		ComputerSystem_DebugMessage(111, SYSPROC, PID,programList[processTable[PID].programListIndex]->executableName, statesNames[prevState], statesNames[processTable[PID].state]);
		//OperatingSystem_PrintReadyToRunQueue();
	} 
	
}


// The STS is responsible of deciding which process to execute when specific events occur.
// It uses processes priorities to make the decission. Given that the READY queue is ordered
// depending on processes priority, the STS just selects the process in front of the READY queue
int OperatingSystem_ShortTermScheduler() {
	
	int selectedProcess;

	selectedProcess=OperatingSystem_ExtractFromReadyToRun();

	return selectedProcess;
}


// Return PID of more priority process in the READY queue
int OperatingSystem_ExtractFromReadyToRun() {
  
	int selectedProcess=NOPROCESS;

	selectedProcess=Heap_poll(readyToRunQueue[USERPROCESSQUEUE],QUEUE_PRIORITY ,&numberOfReadyToRunProcesses[USERPROCESSQUEUE]);
	if (selectedProcess==-1)
		selectedProcess=Heap_poll(readyToRunQueue[DAEMONSQUEUE],QUEUE_PRIORITY ,&numberOfReadyToRunProcesses[DAEMONSQUEUE]);
	// Return most priority process or NOPROCESS if empty queue
	return selectedProcess; 
}

void OperatingSystem_ExtractFromSleepingQueue() {
	int selected = Heap_poll(sleepingProcessesQueue,QUEUE_WAKEUP ,&numberOfSleepingProcesses);
	if (selected!=-1)
		OperatingSystem_MoveToTheREADYState(selected);
}

// Function that assigns the processor to a process
void OperatingSystem_Dispatch(int PID) {
	// The process identified by PID becomes the current executing process
	executingProcessID=PID;

	// Save the previous state to print it V1 Ej10
	int prevState = processTable[PID].state;
	// Change the process' state
	processTable[PID].state=EXECUTING;
	OperatingSystem_ShowTime(SYSPROC);
	//V1 Ej 10 Printing moving state message
	ComputerSystem_DebugMessage(111, SYSPROC, PID,programList[processTable[PID].programListIndex]->executableName, statesNames[prevState], statesNames[processTable[PID].state]);

	// Modify hardware registers with appropriate values for the process identified by PID
	OperatingSystem_RestoreContext(PID);
}


// Modify hardware registers with appropriate values for the process identified by PID
void OperatingSystem_RestoreContext(int PID) {
  
	// New values for the CPU registers are obtained from the PCB
	Processor_CopyInSystemStack(MAINMEMORYSIZE-1,processTable[PID].copyOfPCRegister);
	Processor_CopyInSystemStack(MAINMEMORYSIZE-2,processTable[PID].copyOfPSWRegister);
	
	// Same thing for the MMU registers
	MMU_SetBase(processTable[PID].initialPhysicalAddress);
	MMU_SetLimit(processTable[PID].processSize);
}


// Function invoked when the executing process leaves the CPU 
void OperatingSystem_PreemptRunningProcess() {

	// Save in the process' PCB essential values stored in hardware registers and the system stack
	OperatingSystem_SaveContext(executingProcessID);
	// Change the process' state
	OperatingSystem_MoveToTheREADYState(executingProcessID);
	// The processor is not assigned until the OS selects another process
	executingProcessID=NOPROCESS;
}


// Save in the process' PCB essential values stored in hardware registers and the system stack
void OperatingSystem_SaveContext(int PID) {
	
	// Load PC saved for interrupt manager
	processTable[PID].copyOfPCRegister=Processor_CopyFromSystemStack(MAINMEMORYSIZE-1);
	
	// Load PSW saved for interrupt manager
	processTable[PID].copyOfPSWRegister=Processor_CopyFromSystemStack(MAINMEMORYSIZE-2);
	
}


// Exception management routine
void OperatingSystem_HandleException() {
  
	OperatingSystem_ShowTime(SYSPROC);
	// Show message "Process [executingProcessID] has generated an exception and is terminating\n"
	ComputerSystem_DebugMessage(71,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
	
	OperatingSystem_TerminateProcess();

	//V2 Ej 7c
	OperatingSystem_PrintStatus();
}


// All tasks regarding the removal of the process
void OperatingSystem_TerminateProcess() {
  
	int selectedProcess;
	
  	// Save the previous state to print it V1 Ej10
	int prevState = processTable[executingProcessID].state;
	processTable[executingProcessID].state=EXIT;
	OperatingSystem_ShowTime(SYSPROC);
	//V1 Ej 10 Printing moving state message
	ComputerSystem_DebugMessage(111, SYSPROC, executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName, statesNames[prevState], statesNames[processTable[executingProcessID].state]);
	
	if (programList[processTable[executingProcessID].programListIndex]->type==USERPROGRAM) 
		// One more user process that has terminated
		numberOfNotTerminatedUserProcesses--;
	
	if (numberOfNotTerminatedUserProcesses==0 || executingProcessID==sipID) {
		if (executingProcessID==sipID) {
			// finishing sipID, change PC to address of OS HALT instruction
			OperatingSystem_TerminatingSIP();
			OperatingSystem_ShowTime(SHUTDOWN);
			ComputerSystem_DebugMessage(99,SHUTDOWN,"The system will shut down now...\n");
			return; // Don't dispatch any process
		}
		// Simulation must finish, telling sipID to finish
		OperatingSystem_ReadyToShutdown();
	}
	// Select the next process to execute (sipID if no more user processes)
	selectedProcess=OperatingSystem_ShortTermScheduler();

	// Assign the processor to that process
	OperatingSystem_Dispatch(selectedProcess);
}

// System call management routine
void OperatingSystem_HandleSystemCall() {
  
	int systemCallID, changed, prevPID, currentPID;

	
	// Register A contains the identifier of the issued system call
	systemCallID=Processor_GetRegisterA();
	
	switch (systemCallID) {
		case SYSCALL_PRINTEXECPID:
			OperatingSystem_ShowTime(SYSPROC);
			// Show message: "Process [executingProcessID] has the processor assigned\n"
			ComputerSystem_DebugMessage(72,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
			break;

		case SYSCALL_END:
			OperatingSystem_ShowTime(SYSPROC);
			// Show message: "Process [executingProcessID] has requested to terminate\n"
			ComputerSystem_DebugMessage(73,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
			OperatingSystem_TerminateProcess();
			//V2 Ej 7b
			OperatingSystem_PrintStatus();
			break;

		//V1 Ej 12
		case SYSCALL_YIELD: 
			changed = OperatingSystem_GiveControl();
			// v2 Ej 7a
			if (changed == 1)
				OperatingSystem_PrintStatus();
			break;
		
		//V2 Ej 5
		case SYSCALL_SLEEP:
			prevPID=executingProcessID;
			currentPID=OperatingSystem_ShortTermScheduler();
			OperatingSystem_MoveToTheBLOCKEDState(prevPID);
			OperatingSystem_Dispatch(currentPID);
			OperatingSystem_PrintStatus();
			break;
	}
}
	
//	Implement interrupt logic calling appropriate interrupt handle
// void HOOKEABLE(OperatingSystem_InterruptLogic)(int entryPoint){
void OperatingSystem_InterruptLogic(int entryPoint){
	switch (entryPoint){
		case SYSCALL_BIT: // SYSCALL_BIT=2
			OperatingSystem_HandleSystemCall();
			break;
		case EXCEPTION_BIT: // EXCEPTION_BIT=6
			OperatingSystem_HandleException();
			break;
		case CLOCKINT_BIT: // CLOCKINT_BIT=9 v2 Ej 2-B
			OperatingSystem_HandleClockInterrupt();
			break;
	}

}

//v1 Ej11
void OperatingSystem_PrintReadyToRunQueue() {
	int i,j,PID;
	OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
	ComputerSystem_DebugMessage(112,SHORTTERMSCHEDULE);
	for (i=0; i<NUMBEROFQUEUES; i++){
		ComputerSystem_DebugMessage(113, SHORTTERMSCHEDULE, queueNames[i]);
		for (j=0; j<numberOfReadyToRunProcesses[i]; j++){
			PID = readyToRunQueue[i][j].info;
			ComputerSystem_DebugMessage(114, SHORTTERMSCHEDULE, PID, processTable[PID].priority);
			if (j!=numberOfReadyToRunProcesses[i]-1)
				ComputerSystem_DebugMessage(107, SHORTTERMSCHEDULE);
				
		}
		ComputerSystem_DebugMessage(108, SHORTTERMSCHEDULE);
	}
}

//V1 Ej 12
int OperatingSystem_GiveControl() {
	int i,previousPID, currentPID;
	previousPID=executingProcessID;
	i=processTable[executingProcessID].queueID;
	if (numberOfReadyToRunProcesses[USERPROGRAM]>0 && processTable[previousPID].priority==processTable[readyToRunQueue[i][0].info].priority && previousPID!=readyToRunQueue[i][0].info) {
		currentPID = OperatingSystem_ShortTermScheduler();
		OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
		ComputerSystem_DebugMessage(115, SHORTTERMSCHEDULE, previousPID, programList[processTable[previousPID].programListIndex]->executableName, 
			currentPID, programList[processTable[currentPID].programListIndex]->executableName);
		OperatingSystem_PreemptRunningProcess();
		OperatingSystem_Dispatch(currentPID);
		return 1;
	}
	return 0;
}

// Exercise 2-b of V2
void OperatingSystem_HandleClockInterrupt() {
	numberOfClockInterrupts++;
	//V2 Ej4
	OperatingSystem_ShowTime(INTERRUPT); 
	ComputerSystem_DebugMessage(120,INTERRUPT,numberOfClockInterrupts);
	// V2 Ej 6a
	int i, counter=0, pid;
	for (i=0; i<numberOfSleepingProcesses; i++){
		pid=sleepingProcessesQueue[i].info;
		if (processTable[pid].whenToWakeUp==numberOfClockInterrupts) {
			OperatingSystem_ExtractFromSleepingQueue();
			i--;
			counter++;
		}
	}
	int plp;
	plp=OperatingSystem_LongTermScheduler(); // V3 ej 3a

	//V2 Ej 6b
	if (counter>0 || plp>0) { // plp>0 / V3 Ej 3a
		OperatingSystem_PrintStatus();
		//V2 Ej 6c
		int mostPriority = Heap_getFirst(readyToRunQueue[USERPROCESSQUEUE],numberOfReadyToRunProcesses[USERPROCESSQUEUE]);
		if (processTable[executingProcessID].queueID==DAEMONPROGRAM || processTable[executingProcessID].priority>processTable[mostPriority].priority){
			pid = OperatingSystem_ShortTermScheduler();
			OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
			ComputerSystem_DebugMessage(121,SHORTTERMSCHEDULE,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName,pid,
							programList[processTable[pid].programListIndex]->executableName);
			OperatingSystem_PreemptRunningProcess();
			OperatingSystem_Dispatch(pid);
			//V2 Ej 6d
			OperatingSystem_PrintStatus();
		}
		
	}
}

// V2 Ej5
void OperatingSystem_MoveToTheBLOCKEDState(int PID) {
	processTable[executingProcessID].whenToWakeUp=abs(Processor_GetAccumulator() + numberOfClockInterrupts + 1);
	if (Heap_add(executingProcessID, sleepingProcessesQueue, QUEUE_WAKEUP, &numberOfSleepingProcesses, PROCESSTABLEMAXSIZE)>=0){

		int prevState = processTable[PID].state;
		
		processTable[PID].state=BLOCKED;
		OperatingSystem_SaveContext(PID);
		OperatingSystem_ShowTime(SYSPROC);
		ComputerSystem_DebugMessage(111, SYSPROC, PID,programList[processTable[PID].programListIndex]->executableName, 
					statesNames[prevState], statesNames[processTable[PID].state]);
	}
}

//V3 Ej 1
int OperatingSystem_GetExecutingProcessID() {
	return executingProcessID;
}