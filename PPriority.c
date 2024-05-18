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

CONST DWORD prioridad_memoria[] = {
    MEMORY_PRIORITY_VERY_LOW,
    MEMORY_PRIORITY_LOW,
    MEMORY_PRIORITY_MEDIUM,
    MEMORY_PRIORITY_BELOW_NORMAL,
    MEMORY_PRIORITY_NORMAL
};

BOOL cambiar_clase_prioridad(DWORD proceso_id, DWORD clase_prioridad) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proceso_id);
    if (hProcess == NULL) {
        return FALSE;
    }
    BOOL result = SetPriorityClass(hProcess, clase_prioridad);
    CloseHandle(hProcess);
    return result;
}

VOID cambiar_prioridad_hilos(DWORD proceso_id, DWORD error, int prioridad) {
    THREADENTRY32 thread_entry;
    HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h_snapshot == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, _T("No se pudo crear el snapshot para los hilos"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }
    thread_entry.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(h_snapshot, &thread_entry)) {
        CloseHandle(h_snapshot);
        return;
    }
    do {
        if (thread_entry.th32OwnerProcessID == proceso_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread_entry.th32ThreadID);
            if (hThread != NULL) {
                if (!SetThreadPriority(hThread, prioridad)) {
                    MessageBox(NULL, _T("No se pudo establecer la prioridad de los hilos"), _T("Error"), MB_OK | MB_ICONERROR);
                }
                CloseHandle(hThread);
            } else {
                MessageBox(NULL, _T("Error al obtener el hilo"), _T("Error"), MB_OK | MB_ICONERROR);
            }
        }
    } while (Thread32Next(h_snapshot, &thread_entry));
    CloseHandle(h_snapshot);
}

BOOL cambiar_priority_boost(DWORD proceso_id, BOOL boost_deshabilitado) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proceso_id);
    if (hProcess == NULL) {
        return FALSE;
    }
    BOOL result = SetProcessPriorityBoost(hProcess, boost_deshabilitado);
    CloseHandle(hProcess);
    return result;
}

BOOL cambiar_memory_priority(DWORD proceso_id, ULONG memory_priority) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proceso_id);
    if (hProcess == NULL) {
        return FALSE;
    }

    MEMORY_PRIORITY_INFORMATION memoryPriorityInfo;
    memoryPriorityInfo.MemoryPriority = memory_priority;
    BOOL result = SetProcessInformation(hProcess, ProcessMemoryPriority, &memoryPriorityInfo, sizeof(memoryPriorityInfo));
    CloseHandle(hProcess);
    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditPID, hComboPriorityClass, hComboThreadPriority, hComboMemoryPriority, hTogglePriorityBoost;
    static BOOL boost_habilitado = TRUE;
    switch (uMsg) {
	case WM_CREATE:
	    CreateWindow(_T("STATIC"), _T("PID:"), WS_VISIBLE | WS_CHILD,10, 10, 50, 20, hwnd, NULL, NULL, NULL);
	    hEditPID = CreateWindow(_T("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 10, 50, 20, hwnd, NULL, NULL, NULL);

	    CreateWindow(_T("STATIC"), _T("Priority class:"), WS_VISIBLE | WS_CHILD, 10, 40, 160, 20, hwnd, NULL, NULL, NULL);
	    hComboPriorityClass = CreateWindow(_T("COMBOBOX"), NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 120, 40, 280, 280, hwnd, NULL, NULL, NULL);

	    for (INT i = 0; i < sizeof(clases_prioridad) / sizeof(DWORD); i++) {
	        TCHAR buffer[50];
	        switch (clases_prioridad[i]) {
	            case IDLE_PRIORITY_CLASS: _tcscpy(buffer, _T("IDLE_PRIORITY_CLASS")); break;
	            case BELOW_NORMAL_PRIORITY_CLASS: _tcscpy(buffer, _T("BELOW_NORMAL_PRIORITY_CLASS")); break;
	            case NORMAL_PRIORITY_CLASS: _tcscpy(buffer, _T("NORMAL_PRIORITY_CLASS")); break;
	            case ABOVE_NORMAL_PRIORITY_CLASS: _tcscpy(buffer, _T("ABOVE_NORMAL_PRIORITY_CLASS")); break;
	            case HIGH_PRIORITY_CLASS: _tcscpy(buffer, _T("HIGH_PRIORITY_CLASS")); break;
	            case REALTIME_PRIORITY_CLASS: _tcscpy(buffer, _T("REALTIME_PRIORITY_CLASS")); break;
	            default: _tcscpy(buffer, _T("UNKNOWN")); break;
	        }
	        SendMessage(hComboPriorityClass, CB_ADDSTRING, 0, (LPARAM)buffer);
	    }

	    CreateWindow(_T("STATIC"), _T("Thread priority:"), WS_VISIBLE | WS_CHILD, 10, 70, 160, 20, hwnd, NULL, NULL, NULL);
	    hComboThreadPriority = CreateWindow(_T("COMBOBOX"), NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 120, 70, 280, 280, hwnd, NULL, NULL, NULL);

	    for (INT i = 0; i < sizeof(prioridades_hilo) / sizeof(int); i++) {
	        TCHAR buffer[50];
	        switch (prioridades_hilo[i]) {
	            case THREAD_PRIORITY_IDLE: _tcscpy(buffer, _T("THREAD_PRIORITY_IDLE")); break;
	            case THREAD_PRIORITY_LOWEST: _tcscpy(buffer, _T("THREAD_PRIORITY_LOWEST")); break;
	            case THREAD_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("THREAD_PRIORITY_BELOW_NORMAL")); break;
	            case THREAD_PRIORITY_NORMAL: _tcscpy(buffer, _T("THREAD_PRIORITY_NORMAL")); break;
	            case THREAD_PRIORITY_ABOVE_NORMAL: _tcscpy(buffer, _T("THREAD_PRIORITY_ABOVE_NORMAL")); break;
	            case THREAD_PRIORITY_HIGHEST: _tcscpy(buffer, _T("THREAD_PRIORITY_HIGHEST")); break;
	            case THREAD_PRIORITY_TIME_CRITICAL: _tcscpy(buffer, _T("THREAD_PRIORITY_TIME_CRITICAL")); break;
	            default: _tcscpy(buffer, _T("UNKNOWN")); break;
	        }
	        SendMessage(hComboThreadPriority, CB_ADDSTRING, 0, (LPARAM)buffer);
	    }

	    CreateWindow(_T("STATIC"), _T("Memory priority:"), WS_VISIBLE | WS_CHILD, 10, 100, 160, 20, hwnd, NULL, NULL, NULL);
	    hComboMemoryPriority = CreateWindow(_T("COMBOBOX"), NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 120, 100, 280, 280, hwnd, NULL, NULL, NULL);

	    for (INT i = 0; i < sizeof(prioridad_memoria) / sizeof(DWORD); i++) {
	        TCHAR buffer[50];
	        switch (prioridad_memoria[i]) {
	            case MEMORY_PRIORITY_VERY_LOW: _tcscpy(buffer, _T("MEMORY_PRIORITY_VERY_LOW")); break;
	            case MEMORY_PRIORITY_LOW: _tcscpy(buffer, _T("MEMORY_PRIORITY_LOW")); break;
	            case MEMORY_PRIORITY_MEDIUM: _tcscpy(buffer, _T("MEMORY_PRIORITY_MEDIUM")); break;
	            case MEMORY_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("MEMORY_PRIORITY_BELOW_NORMAL")); break;
	            case MEMORY_PRIORITY_NORMAL: _tcscpy(buffer, _T("MEMORY_PRIORITY_NORMAL")); break;
	            default: _tcscpy(buffer, _T("UNKNOWN")); break;
	        }
	        SendMessage(hComboMemoryPriority, CB_ADDSTRING, 0, (LPARAM)buffer);
	    }

	    CreateWindow(_T("BUTTON"), _T("Set Priorities"), WS_VISIBLE | WS_CHILD, 140, 170, 120, 30, hwnd, (HMENU)1, NULL, NULL);
	    hTogglePriorityBoost = CreateWindow(_T("BUTTON"), _T("Priority Boost: Activado"), WS_VISIBLE | WS_CHILD, 10, 140, 200, 20, hwnd, (HMENU)2, NULL, NULL);
	    break;


    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            TCHAR buffer[50];
            GetWindowText(hEditPID, buffer, 50);
            DWORD proceso_id = _ttoi(buffer);

            INT clase_opcion = SendMessage(hComboPriorityClass, CB_GETCURSEL, 0, 0);
            if (clase_opcion == CB_ERR) {
                MessageBox(hwnd, _T("Opcion invalida para la prioridad de la clase"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }
            DWORD clase_prioridad = clases_prioridad[clase_opcion];

            if (!cambiar_clase_prioridad(proceso_id, clase_prioridad)) {
                MessageBox(hwnd, _T("No se pudo establecer la prioridad de la clase"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }

            INT hilo_opcion = SendMessage(hComboThreadPriority, CB_GETCURSEL, 0, 0);
            if (hilo_opcion == CB_ERR) {
                MessageBox(hwnd, _T("Opcion invalida para la prioridad de los hilos"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }
            INT prioridad_hilo = prioridades_hilo[hilo_opcion];
            cambiar_prioridad_hilos(proceso_id, GetLastError(), prioridad_hilo);

            INT memory_opcion = SendMessage(hComboMemoryPriority, CB_GETCURSEL, 0, 0);
            if (memory_opcion == CB_ERR) {
                MessageBox(hwnd, _T("Opcion invalida para la prioridad de la memoria"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }
            ULONG memory_priority = prioridad_memoria[memory_opcion];

            if (!cambiar_memory_priority(proceso_id, memory_priority)) {
                MessageBox(hwnd, _T("No se pudo establecer la prioridad da la memoria"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }

            BOOL boost_deshabilitado = !boost_habilitado;
            if (!cambiar_priority_boost(proceso_id, boost_deshabilitado)) {
                MessageBox(hwnd, _T("No se pudo activar el Priority Boost"), _T("Error"), MB_OK | MB_ICONERROR);
                break;
            }

            MessageBox(hwnd, _T("Se establecieron las prioridades"), _T("Success"), MB_OK | MB_ICONINFORMATION);
        }
        else if (LOWORD(wParam) == 2) {
            boost_habilitado = !boost_habilitado;
            if (boost_habilitado) {
                SetWindowText(hTogglePriorityBoost, _T("Priority Boost: Activado"));
            }
            else {
                SetWindowText(hTogglePriorityBoost, _T("Priority Boost: Desactivado"));
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("PriorityChangerClass");
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("PPriority | c4rta"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 460, 250, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (INT)msg.wParam;
}
