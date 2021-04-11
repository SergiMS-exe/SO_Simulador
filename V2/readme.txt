Ej 1:
    Se han añadido las llamadas a los metodos que muestran los tics de reloj antes de cada mensaje
Ej 2:
    a: añadido el CLOCKINT_BIT=9 al interruptLines_CPU del Processor.h. Además tambien hubo que inicializar el vector de interrupciones
    b: Creada la funcion de manejo de interrupciones de reloj
    c: En OperatingSystem_InterruptLogic se ha añadido otro case para CLOCKINT_BIT que llame a OperatingSystem_HandleClockInterrupt
    d: En Clock_Update se ha añadido un if para controlar si han pasado intervalBetweenInterrupts unidades de tiempo. Por alguna razon, si llamo a Processor_RaiseInterruption, entra en bucle infinito, por lo que de momento he llamado a OperatingSystem_InterruptLogic
Ej 3:
    a: Añadido INTERRUPT_MASKED_BIT=15 a PSW_BITS en Processor.h
    b: Añadidas las lineas que se ordenan en Processor_ShowPSW
    c/e: En Processor_ManageInterrupts se ha hecho la llamada a Processor_ActivatePSW_Bit(INTERRUPT_MASKED_BIT) para que active tambien el bit de enmascarado
Ej 4:
    Añadidas las llamadas a las funciones debugMessage y showTime en OperatingSystem_HandleClockInterrupt()
Ej 5:
    Introducida la nueva llamada al sistema de procesos bloqueados
    Añadidas las variables necesarias en el PCB y en OperatingSystem.c y OperatingSystem.h
    Añadida la llamada SYSCALL_SLEEP y en su case correspondiente, llamada a la funcion OperatingSystem_MoveToTheBLOCKEDState, donde se calcula whenToWakeUp y se hacen otras operaciones necesarias
    , ademas de llamar a PintStatus al final del metodo
Ej 6:
    Añadido un bucle en la funcion OperatingSystem_HandleClockInterrupt para comprobar qué procesos tienen que ser despertados. Si tienen que despertarse se llama a una funcion que lo hace
    Tras mostrar el estado del simulador si ha habido algun cambio, se comprueba si el proceso actual es el que mas prioridad tiene. Si no es asi, se echa y se ejecuta el mas prioritario mostrando el mensaje pertinente
    Si ha habido algun cambio de proceso, se vuelve a imprimir el estado del sistema
Ej 7:
    Se llama a PrintStatus en los puntos indicados: Al final del tratamiento de SYSCALL_YIELD si ha habido cambio de proceso; al final del tratamiento de SYSCALL_END; al final del manejador de excepciones y al final de la ejecucion del PLP si se ha creado algun proceso
Ej 8:
    Se ha comentado la llamada a la funcion que imprime la cola de listos dentro de la funcion OperatingSystem_MoveToTheREADYState