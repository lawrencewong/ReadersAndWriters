/* a1.h
Readers and Writers Library
Lawrence Wong
January 24, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "a1.h"

#define FILENAME "sharedFile.bin"

// Readers and Writers Simulation
int main(void){

	// Declaring and intializing variables.
	int i = 0;
	int k = 0 ;
	int ret = 0;
	int *settings;
	int num_writers = 0;
	int num_readers = 0;
	int num_iterations = 0;
	FILE *fp;

	settings = getSettings();
	num_readers = settings[0];
	num_writers = settings[1];
	num_iterations = settings[2];
	initializeFile(num_writers);

	// Setting up the reader and writer thread arrays. 
	thread_data writers_thread_data[num_writers];
	thread_data readers_thread_data[num_readers];
	pthread_t writers_thread[num_writers];
	pthread_t readers_thread[num_readers];
	pthread_mutex_t * temp_readers_lock = malloc(sizeof(pthread_mutex_t)*num_readers);

	// Initializing the writer lock.
	if(pthread_mutex_init(&writer_lock, NULL) != 0){
		printf("Mutex Failed\n");
		exit(1);
	}

	// Initializing the reader locks.
	for(i=0; i <num_readers;i++){
		if(pthread_mutex_init(&readers_thread_data[i].my_lock, NULL) != 0){
			printf("Mutex Failed\n");
			exit(1);
		}
		temp_readers_lock[i] = readers_thread_data[i].my_lock;
	}

	// Setting up the reader threads.
	for(i = 0; i <num_readers; i++){
		readers_thread_data[i].thread_id = i;
		readers_thread_data[i].iterations = num_iterations;
		readers_thread_data[i].writers = num_writers;
		readers_thread_data[i].readers = num_readers;
		readers_thread_data[i].reader_locks = NULL;
		ret = pthread_create(&readers_thread[i], 0, readNumber, &readers_thread_data[i]);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	// Setting up the writer threads.
	for(i = 0; i <num_writers; i++){
		writers_thread_data[i].thread_id = i;
		writers_thread_data[i].iterations = num_iterations;
		writers_thread_data[i].writers = num_writers;
		writers_thread_data[i].readers = num_readers;
		writers_thread_data[i].reader_locks = temp_readers_lock;
		ret = pthread_create(&writers_thread[i], 0, increment, &writers_thread_data[i]);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	// Cleaning up the simulation.
	for(i=0;i<num_writers;i++){
		pthread_join(writers_thread[i],NULL);
	}
	for(i=0;i<num_readers;i++){
		pthread_join(readers_thread[i],NULL);
	}

	pthread_mutex_destroy(&writer_lock);
	for(i=0;i<num_readers;i++){
		pthread_mutex_destroy(&readers_thread_data[i].my_lock);
	}
	pthread_mutex_destroy(temp_readers_lock);
	free(temp_readers_lock);
	pthread_exit(NULL);
	return 0;

}

// Getting all of the settings for the simulation.
int* getSettings(){
	static int input[3];
	printf("\nReaders and Writers Problem Simulation.\n");
	printf("How many readers do you wish to be created? ");
	scanf("%d", &input[0]);
	printf("How many writers do you wish to be created? ");
	scanf("%d", &input[1]);
	printf("How many iterations would you like? ");
	scanf("%d", &input[2]);
	printf("You have chosen to create %d readers and %d writers with %d iterations.\n", input[0], input[1], input[2]);
	return input;
}

// Filling the shared memory file with 0's.
void* initializeFile(int num_writers){
	int fill = 0;
	int i = 0;
	FILE *fp;
	fp =fopen(FILENAME,"w+");

	if(fp != NULL){
		for(i = 0; i < num_writers; i++){
			fwrite(&fill, sizeof(int), 1, fp);
		}
		fclose(fp);
	}else{
		printf("Could not open file.\n");
		exit(1);
	}
}

// The writer thread function that reads the value of the binary file pertaining to that writer and increments that value.
void* increment(void* parameter){
	thread_data * cur_thread;
	FILE *fp;
	cur_thread = (thread_data *)parameter;
	int value = 0;
	int i;
	int k;

	for(k=1;k<=cur_thread->iterations;k++){
		for(i=0;i<cur_thread->readers;i++){
			pthread_mutex_lock(&cur_thread->reader_locks[i]);
		}
		pthread_mutex_lock(&writer_lock);
		fp = fopen(FILENAME,"rb+");

		for( i = 0; i < cur_thread->writers; i++){
			fseek(fp,sizeof(int)*i,SEEK_SET);
			fread(&value, sizeof(int), 1, fp);

			if( i == cur_thread->thread_id){
				value++;
				fseek(fp,sizeof(int)*i,SEEK_SET);
				fwrite(&value, sizeof(int), 1, fp);
			}
		}
		fclose(fp);
		pthread_mutex_unlock(&writer_lock);
		for(i=0;i<cur_thread->readers;i++){
			pthread_mutex_unlock(&cur_thread->reader_locks[i]);
		}
		sleep(rand()%5);
	}
}

// The reader thread function that reads all the values of each writer's value and prints it to the screen.
void* readNumber(void* parameter){
	thread_data * cur_thread;
	cur_thread = (thread_data *)parameter;
	int contents[cur_thread->writers];
	FILE *fp;
	char *contents_string = malloc(sizeof(int)*cur_thread->writers);
	char temp[sizeof(int)];
	int i;
	int k;
	for(k=1;k<=cur_thread->iterations;k++){
		pthread_mutex_lock(&cur_thread->my_lock);

		strcpy(contents_string,"");
		fp = fopen(FILENAME,"rb+");
		fread(contents, sizeof(int),cur_thread->writers, fp);
		for(i=0;i<cur_thread->writers;i++){
			sprintf(temp, "%d ",contents[i]);
			strcat(contents_string,temp);

		}
		fclose(fp);
		printf("Iteration #: %d Reader Thread ID: %d contents: %s \n",k,cur_thread->thread_id+1, contents_string);
		fflush(stdout);
		pthread_mutex_unlock(&cur_thread->my_lock);

		sleep(rand()%5);

	}
	free(contents_string);
}