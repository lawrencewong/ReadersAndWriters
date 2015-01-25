/* a1.h
Readers and Writers Library
Lawrence Wong
January 24, 2015
*/

// Information for each thread, including the thread's lock and all of the reader's locks
typedef struct {
	int thread_id;
	int iterations;
	int writers;
	int readers;
	pthread_mutex_t my_lock;
	pthread_mutex_t * reader_locks;
} thread_data;

// Global writer lock
pthread_mutex_t writer_lock;

// increment function that is used for the writer threads. The thread will run for the amount of iterations. Before reading and writing writer thread will lock all readers and the main writer lock.
void* increment(void* parameter);
// readNumber function that is used for reader threads. The thread will run for the amount of iterations. Before reading it will use it's own lock.
void* readNumber(void* parameter);
// initializeFile function sets up the binary file based on how many writer threads there will be.
void* initializeFile(int num_writers);
// getSettings function gets the number of readers, writers and iterations from the user.
int* getSettings();