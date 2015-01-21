#include <stdio.h>
#include <stdlib.h>

#define FILENAME "sharedFile.bin"

int num_readers;
int num_writers;
int num_interations;

void intro();
void initializeBinaryFile();

int main(){

	intro();
	initializeBinaryFile();

	return 0;

}

void intro(){

	printf("\nReaders and Writers Problem Simulation.\n");
	printf("How many readers do you wish to be created? ");
	scanf("%d", &num_readers);
	printf("How many writers do you wish to be created? ");
	scanf("%d", &num_writers);
	printf("How many iterations would you like? ");
	scanf("%d", &num_interations);
	printf("You have chosen to create %d readers and %d writers with %d iterations.\n", num_readers, num_writers, num_interations);

}

void initializeBinaryFile(){

	FILE *file_ptr;
	int i = 0;
	int fill = 0;
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

}