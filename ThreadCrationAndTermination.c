#include <stdio.h>
#include <windows.h>

// Function that will be run by the child thread
DWORD WINAPI child_thread_function(LPVOID lpParam) {
    printf("This is the child thread. Thread ID: %lu\n", GetCurrentThreadId());
    Sleep(300);  // Wait for 0.3 seconds
    printf("Child thread exiting. Thread ID: %lu\n", GetCurrentThreadId());
    return 0;
}

int main() {
    HANDLE child_thread;
    DWORD thread_id;

    // Create the child thread
    child_thread = CreateThread(
        NULL,              // Default security attributes
        0,                 // Default stack size
        child_thread_function,  // Thread function
        NULL,              // Argument to thread function
        0,                 // Default creation flags
        &thread_id         // Receive thread identifier
    );

    if (child_thread == NULL) {
        printf("ERROR: Failed to create child thread\n");
        return 1;
    }

    // This is the parent thread
    printf("This is the parent thread. Thread ID: %lu\n", GetCurrentThreadId());

    // Wait for the child thread to finish
    WaitForSingleObject(child_thread, INFINITE);

    // Close the thread handle
    CloseHandle(child_thread);

    printf("Parent thread exiting. Thread ID: %lu\n", GetCurrentThreadId());
    return 0;
}
