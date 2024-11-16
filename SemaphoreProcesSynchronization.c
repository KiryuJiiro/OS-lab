#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUM_PROCESSES 5

// Semaphore handle
HANDLE semaphore;

// Thread function that simulates a critical section
DWORD WINAPI process_function(LPVOID lpParam) {
    // Wait (P) operation
    WaitForSingleObject(semaphore, INFINITE);
    
    // Critical section
    printf("Thread %d is in the critical section\n", GetCurrentThreadId());
    Sleep(1000);  // Simulate some work in the critical section
    printf("Thread %d is leaving the critical section\n", GetCurrentThreadId());
    
    // Signal (V) operation
    ReleaseSemaphore(semaphore, 1, NULL);

    return 0;
}

int main() {
    HANDLE threads[NUM_PROCESSES];

    // Create a semaphore with an initial value of 1
    semaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (semaphore == NULL) {
        fprintf(stderr, "CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Create threads
    for (int i = 0; i < NUM_PROCESSES; i++) {
        threads[i] = CreateThread(NULL, 0, process_function, NULL, 0, NULL);
        if (threads[i] == NULL) {
            fprintf(stderr, "CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    // Wait for all threads to complete
    WaitForMultipleObjects(NUM_PROCESSES, threads, TRUE, INFINITE);

    // Close thread handles
    for (int i = 0; i < NUM_PROCESSES; i++) {
        CloseHandle(threads[i]);
    }

    // Close the semaphore handle
    CloseHandle(semaphore);

    return 0;
}
