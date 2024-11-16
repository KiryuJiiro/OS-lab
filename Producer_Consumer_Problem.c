#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 5

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

HANDLE empty;  // Semaphore to count empty slots
HANDLE full;   // Semaphore to count filled slots
HANDLE mutex;  // Mutex for mutual exclusion

DWORD WINAPI producer(LPVOID param) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        WaitForSingleObject(empty, INFINITE);  // Wait if no empty slots
        WaitForSingleObject(mutex, INFINITE);  // Lock mutex

        buffer[in] = i;
        printf("Producer produced: %d\n", i);
        in = (in + 1) % BUFFER_SIZE;

        ReleaseMutex(mutex);  // Unlock mutex
        ReleaseSemaphore(full, 1, NULL);  // Signal that there's a new item in the buffer

        Sleep(1000);  // Simulate production time
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID param) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        WaitForSingleObject(full, INFINITE);  // Wait if no items to consume
        WaitForSingleObject(mutex, INFINITE);  // Lock mutex

        int item = buffer[out];
        printf("Consumer consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        ReleaseMutex(mutex);  // Unlock mutex
        ReleaseSemaphore(empty, 1, NULL);  // Signal that there's an empty slot in the buffer

        Sleep(1000);  // Simulate consumption time
    }
    return 0;
}

int main() {
    HANDLE prod_thread, cons_thread;

    // Initialize the semaphores and mutex
    empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);

    if (empty == NULL || full == NULL || mutex == NULL) {
        fprintf(stderr, "Error creating synchronization objects\n");
        return 1;
    }

    // Create the producer and consumer threads
    prod_thread = CreateThread(NULL, 0, producer, NULL, 0, NULL);
    cons_thread = CreateThread(NULL, 0, consumer, NULL, 0, NULL);

    // Wait for both threads to complete
    WaitForSingleObject(prod_thread, INFINITE);
    WaitForSingleObject(cons_thread, INFINITE);

    // Clean up
    CloseHandle(prod_thread);
    CloseHandle(cons_thread);
    CloseHandle(empty);
    CloseHandle(full);
    CloseHandle(mutex);

    return 0;
}
