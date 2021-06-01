#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<unistd.h>
#define KB (1000)
#define SIZE (100 * 1000 * 1000)
#define nil NULL
#define file "text.txt"

typedef struct timer{
	struct timeval s;
	struct timeval e;
	void (*start)(struct timer* this);
	void (*end)(struct timer* this);
	long (*diff)(struct timer* this);
}Timer;

void start(Timer* this){
	gettimeofday(&this->s, nil);
}

void end(Timer* this){
	gettimeofday(&this->e, nil);
}

long diff(Timer* this){
	return 1000000 * (this->e.tv_sec - this->s.tv_sec) + (this->e.tv_usec - this->s.tv_usec);
}

Timer* __Timer__(){
	Timer* this = malloc(sizeof(Timer));
	this->start = start;
	this->end = end;
	this->diff = diff;
    return this;
}

void randomSeek(FILE* f){
	// 4KB aligned
	// random range [0, SIZE / 4KB) blocks
	fseek(f, (random() % (SIZE / 4 / KB)) * 4 * KB, SEEK_SET);
}

int main(){
	// Initilize
	srand(time(nil));			// set random seed
	Timer* t = __Timer__();		// timer

	// 0. create a 100MB files
	// 1. sequential read
	// 2. sequential write
	// 3. radom read
	// 4. random buffered write_1
	// 5. radnom buffered write_2

	// 0. create 100MB text
	printf("generating 100MB data");
	FILE* f = fopen(file, "w");
	if(!f){
		fprintf(stderr, "fopen() error\n");
		exit(EXIT_FAILURE);
	}
	int i;
	for(i = 0; i < SIZE / (sizeof(char) * 10); i++){
		fprintf(f, "%010d", i);
	}
	printf("\t\tdone\n");
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
		exit(EXIT_FAILURE);
	}
	printf("1. Sequential read:");

	// 1. sequential read
	char* buffer = malloc(4 * KB);
	f = fopen(file, "r");
	if(!f){
		fprintf(stderr, "fopen() error\n");
	}
	t->start(t);
	for(i = 0; i < SIZE / (4 * KB); i++){
		fread(buffer, sizeof(char), 4 * KB / sizeof(char), f);
	}
	t->end(t);
	printf("%14ld us\n", t->diff(t));
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
	}

	// 2. sequential overwrite
	printf("2. Sequential write:");
	char* data = malloc(SIZE);
	data[0] = '\0';
	for(i = 0; i < SIZE / (sizeof(char) * 10); i++){
		sprintf(data + 10*i, "%010x", i);
	}
	f = fopen(file, "w");
	if(!f){
		fprintf(stderr, "fopen() error\n");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < SIZE / (2 * KB); i++){
		fwrite(data + 2*KB*i ,sizeof(char), 2 * KB / sizeof(char), f);
		fsync(fileno(f));
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
		exit(EXIT_FAILURE);
	}
	// 3. radnom read 50,000 times
	printf("3. Random read:");
	f = fopen(file, "r");
	if(!f){
		fprintf(stderr, "fopen() error\n");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < 50000; i++){
		randomSeek(f);
		fread(buffer, sizeof(char), 4 * KB / sizeof(char), f);
	}
	t->end(t);
	printf("%18ld us\n", t->diff(t));
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
		exit(EXIT_FAILURE);
	}

	// 4. random buffered write type 1
	printf("4. Random write - 1:");
	// prepare 2KB data
	for(i = 0; i < 2 * KB / sizeof(char); i++){
		data[i]  = (int)random() % 26 + 'A';
	}

	f = fopen(file, "w");
	if(!f){
		fprintf(stderr, "fopen() error\n");
	}
	t->start(t);
	for(i = 0; i < 50000; i++){
		randomSeek(f);
		fwrite(data, sizeof(char), 2 * KB / sizeof(char), f);
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
		exit(EXIT_FAILURE);
	}

	// 5. random buffered write type 2
	printf("5. Random write - 2:");
	// prepare 2KB data
	for(i = 0; i < 2 * KB / sizeof(char); i++){
		data[i] = (int)random() % 26 + 'a';
	}

	f = fopen(file, "w");
	if(!f){
		fprintf(stderr, "fopen() error\n");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i< 50000; i++){
		randomSeek(f);
		fwrite(data, sizeof(char), 2 * KB / sizeof(char), f);
		fsync(fileno(f));
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	if(fclose(f) == -1){
		fprintf(stderr, "fclose() error\n");
	}

	return 0;
}
