#include "Clock.h"
#include "Processor.h"
#include "ComputerSystem.h"
#include "ComputerSystemBase.h"
#include "OperatingSystem.h"

int tics=0;
int numberOfClockInterrupts = 0; //V2 Ej 4

void Clock_Update() {
	tics++;
	if (tics>=intervalBetweenInterrupts && tics%intervalBetweenInterrupts==0){
		numberOfClockInterrupts++;
		//V2 Ej4
		OperatingSystem_ShowTime(INTERRUPT); 
    	ComputerSystem_DebugMessage(120,INTERRUPT,numberOfClockInterrupts);
		
		OperatingSystem_InterruptLogic(CLOCKINT_BIT); //V2 Ej 2
	}
}


int Clock_GetTime() {

	return tics;
}
