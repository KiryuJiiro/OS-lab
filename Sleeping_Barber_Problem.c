#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUM_CHAIRS 5

HANDLE customers; // Semaphore to count customers
HANDLE barberChair; // Semaphore for barber chair
HANDLE barberDone; // Semaphore for barber finishing
HANDLE mutex; // Mutex to protect shared variables

int num_waiting = 0; // Number of customers waiting
int next_customer = 0; // Index of next customer to serve

DWORD WINAPI barber_function(LPVOID arg) {
    while (1) {
        WaitForSingleObject(customers, INFINITE); // Wait for a customer to arrive
        WaitForSingleObject(mutex, INFINITE);

        num_waiting--;
        ReleaseSemaphore(barberChair, 1, NULL); // Allow customer to sit in barber chair
        printf("Barber: Starting haircut for customer %d\n", next_customer);
        next_customer = (next_customer + 1) % NUM_CHAIRS;

        ReleaseMutex(mutex);
        Sleep(2000); // Simulate haircut duration

        printf("Barber: Finished haircut for customer\n");
        ReleaseSemaphore(barberDone, 1, NULL); // Signal haircut is finished
    }
    return 0;
}

DWORD WINAPI customer_function(LPVOID arg) {
    int id = *(int*)arg;
    WaitForSingleObject(mutex, INFINITE);

    if (num_waiting < NUM_CHAIRS) {
        num_waiting++;
        printf("Customer %d: Arrived and waiting for haircut\n", id);
        ReleaseSemaphore(customers, 1, NULL); // Signal barber that customer has arrived
        ReleaseMutex(mutex);

        WaitForSingleObject(barberChair, INFINITE); // Wait for barber chair to become available
        printf("Customer %d: Getting haircut\n", id);
        WaitForSingleObject(barberDone, INFINITE); // Wait for haircut to finish

        printf("Customer %d: Finished haircut and leaving\n", id);
    } else {
        printf("Customer %d: No free chairs, leaving without haircut\n", id);
        ReleaseMutex(mutex);
    }
    return 0;
}

int main() {
    HANDLE barber;
    HANDLE customers_threads[NUM_CHAIRS];
    int customer_ids[NUM_CHAIRS];

    // Initialize semaphores and mutex
    customers = CreateSemaphore(NULL, 0, NUM_CHAIRS, NULL);
    barberChair = CreateSemaphore(NULL, 0, 1, NULL);
    barberDone = CreateSemaphore(NULL, 0, 1, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);

    barber = CreateThread(NULL, 0, barber_function, NULL, 0, NULL);

    for (int i = 0; i < NUM_CHAIRS; i++) {
        customer_ids[i] = i;
        customers_threads[i] = CreateThread(NULL, 0, customer_function, &customer_ids[i], 0, NULL);
        Sleep(1000); // Interval between customer arrivals
    }

    // Wait for all customer threads to finish
    WaitForMultipleObjects(NUM_CHAIRS, customers_threads, TRUE, INFINITE);

    // Clean up handles
    for (int i = 0; i < NUM_CHAIRS; i++) {
        CloseHandle(customers_threads[i]);
    }
    CloseHandle(barber);
    CloseHandle(customers);
    CloseHandle(barberChair);
    CloseHandle(barberDone);
    CloseHandle(mutex);

    return 0;
}
