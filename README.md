# TPriority
CLI tool to play with the priority classes and priority of the threads of a process

The tool uses the ```SetPriorityClass``` and ```SetThreadPriority``` functions to calculate the base priority of threads, uses ```THREADENTRY32``` struct from ```tlhelp32.h``` to iterate over all threads in a process and set the priority according to user input.

If you want to know the combinations of class priority and thread priority, read this:
https://learn.microsoft.com/en-US/windows/win32/procthread/scheduling-priorities

If you use GNU/Linux, you can compile it with: ```x86_64-w64-mingw32-g++ TPriority.c -o TPriority```

To-do: Use ```SetProcessPriorityBoost``` and ```SetThreadPriorityBoost``` to manage Priority Boosts
