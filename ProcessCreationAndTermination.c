#include <stdio.h>
#include <windows.h>

int main() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Initialize the STARTUPINFO structure
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create a child process (could be another executable you have)
    if (CreateProcess(NULL, "notepad.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // This is the parent process
        printf("This is the parent process. PID: %lu\n", GetCurrentProcessId());

        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        // Error occurred
        printf("ERROR: Failed to create child process\n");
        return 1;
    }

    printf("Exiting the program. PID: %lu\n", GetCurrentProcessId());
    return 0;
}
