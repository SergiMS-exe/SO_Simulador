#include "Clock.h"
#include "Processor.h"
#include "ComputerSystem.h"
#include "ComputerSystemBase.h"
#include "OperatingSystem.h"

int tics=0;


void Clock_Update() {
	tics++;
	if (tics>=intervalBetweenInterrupts && tics%intervalBetweenInterrupts==0){
		Processor_RaiseInterrupt(CLOCKINT_BIT); //V2 Ej 2
	}
}


int Clock_GetTime() {

	return tics;
}
