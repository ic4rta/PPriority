#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

CONST INT prioridades_hilo[] = {
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_TIME_CRITICAL
};

CONST DWORD clases_prioridad[] = {
    IDLE_PRIORITY_CLASS,
    BELOW_NORMAL_PRIORITY_CLASS,
    NORMAL_PRIORITY_CLASS,
    ABOVE_NORMAL_PRIORITY_CLASS,
    HIGH_PRIORITY_CLASS,
    REALTIME_PRIORITY_CLASS
};

BOOL cambiar_clase_prioridad(DWORD proceso_id, DWORD clase_prioridad) {
    return SetPriorityClass(OpenProcess(PROCESS_ALL_ACCESS, FALSE, proceso_id), clase_prioridad);
}

VOID cambiar_prioridad_hilos(DWORD proceso_id, DWORD error, int prioridad) {
    THREADENTRY32 thread_entry;
    HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h_snapshot == INVALID_HANDLE_VALUE) {
        _tprintf(_T("%d"), error);
        return;
    }
    thread_entry.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(h_snapshot, &thread_entry)) {
        _tprintf(_T("%d"), error);
        CloseHandle(h_snapshot);
        return;
    }
    while (Thread32Next(h_snapshot, &thread_entry)) {
        if (thread_entry.th32OwnerProcessID == proceso_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread_entry.th32ThreadID);
            if (hThread != NULL) {
                if (!SetThreadPriority(hThread, prioridad)) {
                    _tprintf(_T("%d"), error);
                }
                CloseHandle(hThread);
            } else {
                _tprintf(_T("%d"), error);
            }
        }
    }
    CloseHandle(h_snapshot);
}

int _tmain() {
    DWORD proceso_id, hilo_opcion, clase_opcion, prioridad_hilo;
    
    _tprintf(_T("Enter the PID: "));
    _tscanf(_T("%d"), &proceso_id);

    _tprintf(_T("1. IDLE_PRIORITY_CLASS\n"));
    _tprintf(_T("2. BELOW_NORMAL_PRIORITY_CLASS\n"));
    _tprintf(_T("3. NORMAL_PRIORITY_CLASS\n"));
    _tprintf(_T("4. ABOVE_NORMAL_PRIORITY_CLASS\n"));
    _tprintf(_T("5. HIGH_PRIORITY_CLASS\n"));
    _tprintf(_T("6. REALTIME_PRIORITY_CLASS\n"));
    _tprintf(_T("Select the priority class: "));
    _tscanf(_T("%d"), &clase_opcion);
    DWORD clase_prioridad;

    if (clase_opcion < 1 || clase_opcion > sizeof(clases_prioridad) / sizeof(DWORD)) {
        _tprintf(_T("Invalid option\n"));
        return 1;
    }
    clase_prioridad = clases_prioridad[clase_opcion - 1];

    if (!cambiar_clase_prioridad(proceso_id, clase_prioridad)) {
        _tprintf(_T("Error setting process priority class\n"));
        return 1;
    }

    _tprintf(_T("1. THREAD_PRIORITY_IDLE\n"));
    _tprintf(_T("2. THREAD_PRIORITY_LOWEST\n"));
    _tprintf(_T("3. THREAD_PRIORITY_BELOW_NORMAL\n"));
    _tprintf(_T("4. THREAD_PRIORITY_NORMAL\n"));
    _tprintf(_T("5. THREAD_PRIORITY_ABOVE_NORMAL\n"));
    _tprintf(_T("6. THREAD_PRIORITY_HIGHEST\n"));
    _tprintf(_T("7. THREAD_PRIORITY_TIME_CRITICAL\n"));

    _tprintf(_T("Select thread priority: "));
    _tscanf(_T("%d"), &hilo_opcion);

    if (hilo_opcion < 1 || hilo_opcion > sizeof(prioridades_hilo) / sizeof(int)) {
        _tprintf(_T("Invalid option\n"));
        return 1;
    }
    prioridad_hilo = prioridades_hilo[hilo_opcion - 1];
    cambiar_prioridad_hilos(proceso_id, GetLastError(), prioridad_hilo);
    _tprintf(_T("Priorities have been established\n"));
    return 0;
}