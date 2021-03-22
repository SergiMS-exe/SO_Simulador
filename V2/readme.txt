Ejercicio 0: copiado del V0
Ejercicio 1: Función ComputerSystem_PrintProgramList creada, que imprime los programas de usuario
Ejercicio 2: ComputerSystem_PrintProgramList llamado en ComputerSystem_PowerOn justo antes de la inicializacion del sistema
Ejercicio 3: Creado el programa prog-V1-E3 que permita su ejecucion mas de una vez gracias al cambio de la instruccion HALT por la instruccion TRAP 3
Ejercicio 4: Modificado OperatingSystem_CreateProcess para que, en la llamada a OperatingSystem_ObtainAnEntryInTheProcessTable, si el resultado es NOFREEENTRY pare la ejecucion.
            De la misma forma se ha modificado OperatingSystem_LongTermScheduler para que, si en el bucle encuentra algun proceso que devuelva NOFREEENTRY, pare la ejecucion y devuelva un mensaje de error
Ejercicio 5: Se han añadido las caracteristicas pedidas. 
            Viendo que, aun no habiendo ningun programa disponible, seguia buscando alguno para ejecutar he decidido terminar la ejecución del SO al comprobar que no queda ningun programa válido en la lista de tipo user.

NOTAS: 
        - No considero que pueda entregar los programas prog-V1-E4 y prog-V1-E5-b porque:
           - En el primer caso, se nos pide ejecutar un numero de programas mayor que la capacidad de la tabla de procesos, por lo que he ejecutado varias veces
               el programVerySimple
           - En el segundo, el programa no debe existir para ver el funcionamiento de simulador

Ejercicio 6: Se ha modificado OperatingSystem_CreateProcess para que devuelva TOOBIDPROCESS si el tamaño del programa es demasiado grande.
             Ademas, se ha modificado OperatingSystem_LongTermScheduler para controlar que el metodo antes mencionado devuelva TOOBIDPROCESS y muestre un mensaje de error.
             En OperatingSystem_LongTermScheduler se ha cambiado el bloque de varios if-else if-else por un bloque switch-case
Ejercicio 7: Se ha cambiado la funcion OperatingSystem_CreateProcess para que, en la llamada a OperatingSystem_LoadProgram retorne TOOBIDPROCESS si el tamaño del programa es menor que el programa en si
Ejercicio 8: Modificado el initialPID a PROCESSTABLEMAXSIZE-1 que es el tamaño de la tabla de procesos. Por lo que el PID de los siguientes programas empezara en 0. Pero esto no afecta a la opcion de ejecucion --initialPID
Ejercicio 9: Creada la funcion OperatingSystem_PrintReadyToRunQueue que imprime la lista de procesos en estado LISTO. Además se le ha llamado en OperatingSystem_MoveToTheREADYState
Ejercicio 10: Creada la estructura de datos con los nombres de los estados de los procesos (char * statesNames). Ahora se imprime un mensaje cada vez que se cambia de estado a un proceso 
Ejercicio 11: Modificados todos los metodos necewsarios y funciona bien, pero error en la impresion de la cola de programas de usuario
Ejercicio 12: Creada la funcion OperatingSystem_GiveControl para dar control al siguiente proceso que tenga su la misma prioridad. Añadido en el enum corresponiente SYSCALL_YIELD y su case para el tratamiento de la llamada, la cual se ejecuta con la orden TRAP 4 
Ejercicio 13:
    a. Se deben guardar los estados de esos registros para que, después de una interrupción, el proceso vuela al estado en el que estaba.
    b. También se deberia guardar el registro de acumulador
    c. Habría que añadir el guardado del registro acumulador en la función
    d. Afectaria al PCB
Ejercicio 14: Cuando el programa que se esta ejecutando es ultimo y ademas es el SystemIdleProcess, ejecuta la funcion OperatingSystem_TerminatingSIP y hace un return para terminar la funcion
Ejercicio 15: Si el numero de procesos creados en el PLP es 0, se recoge en el initialize y se llama a la funcion OperatingSystem_ReadyToShutdown para terminar la simulacions
Ejercicio 16: Hace lo que hace porque no estamos en modo protegido y no deberiamos tener permisos para ejecutar esas instrucciones
    a. Se han añadido condicionales a los case de OS, IRET y HALT de Processor_DecodeAndExecuteInstruction para que si el bit de ejecucion no esta en modo privilegiado, lance una excepcion
    b. Se activa el modo protegido cuando se ejecuta un daemon o se lanza una excepcion