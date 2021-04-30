Ej 1:
    a. Añadido el rrgisterB_CPU en Processor.c
    b. Añadido el enumerado de excepciones en Processor.h
    c. Añadida la declaracion para que comple el nuevo codigo en Processor.h
    d. Añadidas las sentencias de control si
Ej 2:
    se ha hecho la función getRegisterB_CPU en Processor.c, que es el registro que tiene el tipo de exception. Ademas se ha ntenido que añadir la declaración de la funcion en Processor.h para poder usarla luego
    Se ha añadido un switch en OperatingSystem_HandleException para que, dependiendo del tipo de excepción, mande uno u otro mensaje
Ej 3:
    a. En el switch de Processor_DecodeAndExecuteInstruction, en el default, se ha quitado la linea que ponia el tipo de instruccion a unknown y se ha añadido el lanzamiento de excepcion INVALIDINSTRUCTION
    b. se ha añadido un case a switch de OperatingSystem_HandleException para INVALIDINSTRUCTION 
Ej 4: 
    En el switch de OperatingSystem_HandleSystemCall se ha añadido un default que enseña el mensaje 141 que se ha pedido
    Ademas en ese caso, se ha añadido la llamada al terminateProcess para terminar el proceso
    y se imprime el estado
Ej 5:
    En OperatingSystem.h se ha añadido definido el nombre del fichero para que compile
    en OperatingSystem_Initialize se ha añadido la llamada a OperatingSystem_InitializePartitionTable antes de la llamada al creador de daemons
Ej 6:
    a. Creada la funcion mejorAjuste que escoge entre las particiones disponibles, la particion que mas se ajusta al tamaño del proceso. Llamada en ObtainMainMemory
       La funcion devuelve MEMORYFULL si todas las particiones estan llenas
    b, c y d. Creados y mostrados los mensajes en los casos pedidos
    ObtainMainMemory ya no da la direccion inicial del proceso, sino la particion, por lo que despues se debe transformar en una direccion fisica
    En terminateProcess se ha añadido una orden para liberar la particion del proceso terminado
Ej 7:
    Llamada a la funcion OperatingSystem_ShowPartitionTable antes y despues de la llamada a ObtainMainMemory en OS_CreateProcess