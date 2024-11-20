#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
using namespace std;

#define MAX_THREADS 2

DWORD WINAPI producer(LPVOID);
DWORD WINAPI consumer(LPVOID);

HANDLE hThreads[MAX_THREADS]; // Array of Handles to Threads
DWORD id[MAX_THREADS]; // Array of thread ids
DWORD waiter; // Waiter for thread completion

int bufferSize = 0;
int in = 0;
int out = 0;
int counterLimit = 0; // User decides counter limit
int itemCount = 0; // Keeps track of total items, increments from producer, decrements from consumer

struct item {
    int value; // Producer and consumer value
};
vector<item> buffer;

DWORD WINAPI producer(LPVOID) {
    int produced = 0; // Local counter for produced items
    while (produced < counterLimit) {
        while (itemCount == bufferSize) {
            ; // Do nothing if buffer is full
        }

        item next_produced;
        next_produced.value = rand() % 10;
        buffer[in] = next_produced;
        in = (in + 1) % bufferSize;
        itemCount++; // Increment total number of items in buffer
        produced++; // Increment local counter

        cout << "Producer thread produced: " << next_produced.value << endl;
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID) {
    int consumed = 0; // Local counter for consumed items
    while (consumed < counterLimit) {
        while (itemCount == 0) {
            ; // Do nothing if buffer is empty
        }

        item next_consumed = buffer[out];
        out = (out + 1) % bufferSize;
        itemCount--; // Decrement items in buffer
        consumed++; // Increment local counter

        cout << "Consumer consumed: " << next_consumed.value << endl;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Incorrect number of arguments" << endl;
        return -1;
    }
    bufferSize = atoi(argv[1]);
    counterLimit = atoi(argv[2]);

    // Make sure user enters POSITIVE INTEGERS ONLY
    if (bufferSize <= 0 || counterLimit <= 0) {
        cout << "ONLY ENTER POSITIVE INTEGERS!" << endl;
        return 1;
    }

    buffer.resize(bufferSize, {0}); // Initialize buffer with default items
    srand(time(NULL));

    hThreads[0] = CreateThread(NULL, 0, producer, NULL, 0, &id[0]);
    hThreads[1] = CreateThread(NULL, 0, consumer, NULL, 0, &id[1]);

    // Tell main to wait for threads to finish
    waiter = WaitForMultipleObjects(MAX_THREADS, hThreads, TRUE, INFINITE);

    // Close handles
    for (int i = 0; i < MAX_THREADS; i++) {
        CloseHandle(hThreads[i]);
    }

    cout << "THREADS TERMINATED" << endl;
    return 0;
}

/* g++ -o producer_consumer main.cpp -lws2_32
.\producer_consumer 2 4
*/
