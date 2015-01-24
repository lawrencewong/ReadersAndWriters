#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

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
	int fill = 0;
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
			fwrite(&fill, sizeof(int), 1, file_ptr);
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

	for(i = 0; i <num_readers; i++){
		readers_thread_data[i].thread_id = i;
		readers_thread_data[i].iterations = num_iterations;
		readers_thread_data[i].writers = num_writers;
		readers_thread_data[i].readers = num_readers;
		ret = pthread_create(&readers_thread[i], 0, readNumber, &readers_thread_data[i]);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	for(i = 0; i <num_writers; i++){
		writers_thread_data[i].thread_id = i;
		writers_thread_data[i].iterations = num_iterations;
		writers_thread_data[i].writers = num_writers;
		writers_thread_data[i].readers = num_readers;
		ret = pthread_create(&writers_thread[i], 0, increment, &writers_thread_data[i]);
		if(ret != 0){
			printf("Create pthread error!\n");
			exit(1);
		}
	}

	for(i=0;i<num_writers;i++){
		pthread_join(writers_thread[i],NULL);
	}
	for(i=0;i<num_readers;i++){
		pthread_join(readers_thread[i],NULL);
	}

	return 0;

}


void* increment(void* parameter){
	thread_data * cur_thread;
	FILE *fp;
	cur_thread = (thread_data *)parameter;
	int value = 0;
	int i;
	int k;

	fp = fopen(FILENAME,"rb+");
	for(k=1;k<=cur_thread->iterations;k++){

		for( i = 0; i < cur_thread->writers; i++){
			fseek(fp,sizeof(int)*i,SEEK_SET);
			fread(&value, sizeof(int), 1, fp);

			if( i == cur_thread->thread_id){
				value++;
				fseek(fp,sizeof(int)*i,SEEK_SET);
				fwrite(&value, sizeof(int), 1, fp);
			}
		}
		sleep(rand()%5);
	}
	
}

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
		strcpy(contents_string,"");
		fp = fopen(FILENAME,"rb+");
		fread(contents, sizeof(int),cur_thread->writers, fp);
		for(i=0;i<cur_thread->writers;i++){
			sprintf(temp, "%d ",contents[i]);
			strcat(contents_string,temp);

		}
		printf("Iteration #: %d Reader Thread ID: %d contents: %s \n",k,cur_thread->thread_id+1, contents_string);
		fflush(stdout);
		sleep(rand()%5);
	}
	free(contents_string);
}