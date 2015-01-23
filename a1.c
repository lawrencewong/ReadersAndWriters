#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FILENAME "sharedFile.bin"

typedef struct {
	int thread_id;
	int iterations;
	int writers;
	int readers;
} thread_data;

void* increment(void* parameter);
void* readNumber(void* parameter);

int main(void){

	int i = 0;
	int ret = 0;
	int p = 0;
	int k = 0 ;
	int buffer[10];
	int temp = 2;
	int fid = 1;
	int num_writers = 0;
	int num_readers = 0;
	int num_iterations = 0;
	FILE *fp;
	FILE *file_ptr;

	printf("\nReaders and Writers Problem Simulation.\n");
	printf("How many readers do you wish to be created? ");
	scanf("%d", &num_readers);
	printf("How many writers do you wish to be created? ");
	scanf("%d", &num_writers);
	printf("How many iterations would you like? ");
	scanf("%d", &num_iterations);
	printf("You have chosen to create %d readers and %d writers with %d iterations.\n", num_readers, num_writers, num_iterations);
	
	
	file_ptr =fopen(FILENAME,"w+");

	if( file_ptr != NULL){
		for(i = 0; i < num_writers; i++){
			fwrite(&i, sizeof(int), 1, file_ptr);
		}
		fclose(file_ptr);
	}else{
		printf("Could not open file.\n");
		exit(1);
	}

	pthread_t writers_thread[num_writers];
	pthread_t readers_thread[num_readers];

	thread_data writers_thread_data[num_writers];
	thread_data readers_thread_data[num_readers];

	for(i = 1; i <=num_readers; i++){
		readers_thread_data[i].thread_id = i;
		readers_thread_data[i].iterations = num_iterations;
		readers_thread_data[i].writers = num_writers;
		readers_thread_data[i].readers = num_readers;
		ret = pthread_create(&readers_thread[i], 0, readNumber, &readers_thread_data[i]);
		printf("Created %d\n",i);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	for(i = 1; i <= num_writers; i++){
		writers_thread_data[i].thread_id = i;
		writers_thread_data[i].iterations = num_iterations;
		writers_thread_data[i].writers = num_writers;
		writers_thread_data[i].readers = num_readers;
		ret = pthread_create(&writers_thread[i], 0, increment, &writers_thread_data[i]);
		printf("Created %d\n",i);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	// fp=fopen(FILENAME,"rb+");
	// fread(buffer, sizeof(int),num_writers,fp);
	// for(i=0;i<num_writers;i++){
	// 	printf("BEFORE : %d\n",buffer[i]);
	// 	if(fid == i){
	// 		temp = temp + i;
	// 		fseek(fp,sizeof(int)*i,SEEK_SET);
	// 		fwrite(&temp, sizeof(int), 1, fp);
	// 		rewind(fp);
	// 	}
		
	// }
	for(i=1;i<=num_writers;i++){
		pthread_join(writers_thread[i],NULL);
	}
	for(i=1;i<=num_readers;i++){
		pthread_join(readers_thread[i],NULL);
	}

	return 0;

}


void* increment(void* parameter){
	thread_data * cur_thread;
	cur_thread = (thread_data *)parameter;
	printf("THREAD ID: %d ITERATIONS: %d WRITERS: %d READERS: %d\n",cur_thread->thread_id, cur_thread->iterations, cur_thread->writers, cur_thread->readers);
	return 0;
}

void* readNumber(void* parameter){
	thread_data * cur_thread;
	cur_thread = (thread_data *)parameter;
	printf("READ THREAD ID: %d ITERATIONS: %d WRITERS: %d READERS: %d\n",cur_thread->thread_id, cur_thread->iterations, cur_thread->writers, cur_thread->readers);
	return 0;
}