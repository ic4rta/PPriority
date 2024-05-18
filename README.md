# PPriority
Herramienta que permite jugar con las clases de prioridad y prioridad de memoria de un proceso y sus hilos

La herramiente hace uso de ```SetPriorityClass``` y ```SetThreadPriority``` para calcular la prioridad base de los hilos, usa la estructura ```THREADENTRY32``` de ```tlhelp32.h``` para interar sobre todos los hilos de un proceso, tambien usa ```SetProcessInformation``` con ```MEMORY_PRIORITY_INFORMAT``` para cambiar la prioridad de memoria, y por ultimo, usa ```SetProcessPriorityBoost``` para activar o desactivar los priority boost del proceso

Si quieres saber las diferentes combinaciones con las prioridades, lee aqui:
https://learn.microsoft.com/en-US/windows/win32/procthread/scheduling-priorities
