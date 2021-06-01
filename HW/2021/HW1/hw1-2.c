#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#define KB (1000)
#define SIZE (100 * 1000 * 1000)
#define nil NULL
#define file "text2.txt"

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

void randomSeek(int fd){
	// 4KB aligned
	// random range [0, SIZE / 4KB) blocks
	lseek(fd, (random() % (SIZE / 4 / KB)) * 4 * KB, SEEK_SET);
}

int main(){
	// Initilize
	srand(time(nil));		// set random seed
	Timer* t = __Timer__();		// timer

	// 0. create a 100MB files
	// 1. sequential read
	// 2. sequential write
	// 3. radom read
	// 4. random buffered write_1
	// 5. radnom buffered write_2

	// create 100MB text
	printf("generating 100MB data");

	char* data = malloc(SIZE);
	int i;
	for(i = 0; i < SIZE / (sizeof(char) * 10); i++){
		sprintf(data + 10*i, "%010d", i);
	}

	int f = open(file, O_WRONLY | O_TRUNC | O_CREAT);
	if(f == -1){
		fprintf(stderr, "open() error\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < SIZE / (2 * KB); i++){
		write(f, data+2*KB*i, 2*KB);
	}
	close(f);
	printf("\t\tdone\n");
	printf("1. Sequential read:");

	// 1. sequential read
	char* buffer = malloc(4 * KB);
	f = open(file, O_RDONLY);
	if(f == -1){
		fprintf(stderr, "open() error");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < SIZE / (4 * KB); i++){
		read(f, buffer, 4 * KB);
	}
	t->end(t);
	printf("%14ld us\n", t->diff(t));
	close(f);

	// 2. sequential overwrite
	// update data
	printf("2. Sequential write:");
	for(i = 0; i < SIZE / (sizeof(char) * 10); i++){
		sprintf(data + i*10, "%010x", i);
	}
	f = open(file, O_WRONLY | O_TRUNC);
	if(f == -1){
		fprintf(stderr, "open() error");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < SIZE / (2 * KB); i++){
		write(f, data + 2 * KB * i, 2 * KB);
		fsync(f);
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	close(f);

	// 3. radnom read 50,000 times
	printf("3. Random read:");
	f = open(file, O_RDONLY);
	if(f == -1){
		fprintf(stderr, "open() error");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < 50000; i++){
		randomSeek(f);
		read(f, buffer, 4 * KB);
	}
	t->end(t);
	printf("%18ld us\n", t->diff(t));
	close(f);

	// 4. random buffered write type 1
	printf("4. Random write - 1:");
	// prepare 2KB data
	for(i = 0; i < 2 * KB / sizeof(char); i++){
		data[i]  = (int)random() % 26 + 'A';
	}

	f = open(file, O_WRONLY | O_TRUNC);
	if(f == -1){
		fprintf(stderr, "open() error");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i < 50000; i++){
		randomSeek(f);
		write(f, data, 2 * KB);
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	close(f);

	// 5. random buffered write type 2
	printf("5. Random write - 2:");
	// prepare 2KB data
	for(i = 0; i < 2 * KB / sizeof(char); i++){
		data[i] = (int)random() % 26 + 'a';
	}

	f = open(file, O_WRONLY | O_TRUNC);
	if(f == -1){
		fprintf(stderr, "open() error");
		exit(EXIT_FAILURE);
	}
	t->start(t);
	for(i = 0; i< 50000; i++){
		randomSeek(f);
		write(f, data, 2 * KB);
		fsync(f);
	}
	t->end(t);
	printf("%13ld us\n", t->diff(t));
	close(f);

	return 0;
}
