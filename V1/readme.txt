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