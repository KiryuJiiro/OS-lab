#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUM_PHILOSOPHERS 5
#define MAX_EAT_TIMES 3

// Handles for semaphores (acting as forks)
HANDLE forks[NUM_PHILOSOPHERS];

// Mutex for managing access
HANDLE mutex;

// Function for each philosopher thread
DWORD WINAPI philosopher(LPVOID num)
{
    int id = *(int *)num;
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;
    int eat_count = 0;

    while (eat_count < MAX_EAT_TIMES)
    {
        printf("Philosopher %d is thinking\n", id);
        Sleep(1000);  // Thinking

        // Pick up forks with opposite order for the last philosopher
        WaitForSingleObject(mutex, INFINITE);
        if (id == NUM_PHILOSOPHERS - 1) {
            WaitForSingleObject(forks[right], INFINITE);
            WaitForSingleObject(forks[left], INFINITE);
        } else {
            WaitForSingleObject(forks[left], INFINITE);
            WaitForSingleObject(forks[right], INFINITE);
        }
        ReleaseMutex(mutex);

        // Eating
        printf("Philosopher %d is eating\n", id);
        eat_count++;
        Sleep(1000);  // Eating

        // Put down forks
        ReleaseSemaphore(forks[left], 1, NULL);
        ReleaseSemaphore(forks[right], 1, NULL);
    }

    printf("Philosopher %d has finished eating %d times and leaves the table.\n", id, eat_count);
    return 0;
}

int main()
{
    HANDLE philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize mutex and forks
    mutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        forks[i] = CreateSemaphore(NULL, 1, 1, NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        philosophers[i] = CreateThread(NULL, 0, philosopher, &ids[i], 0, NULL);
    }

    // Wait for all philosopher threads to finish
    WaitForMultipleObjects(NUM_PHILOSOPHERS, philosophers, TRUE, INFINITE);

    // Clean up handles
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        CloseHandle(forks[i]);
        CloseHandle(philosophers[i]);
    }
    CloseHandle(mutex);

    return 0;
}
