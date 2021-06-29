#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>

unsigned long int calculateDir(char* path, int first){
	struct stat fileinfo;
	unsigned long int count = 0;

	if(first){
		// Read the sizeof [path]
		if(lstat(path, &fileinfo) == -1){
			fprintf(stderr, "lstat() error at %s\n", path);
			exit(EXIT_FAILURE);
		}
		// If [path] is a symbolic link or a regular file
		// Get its size and do not use opendir()
		if(S_ISLNK(fileinfo.st_mode)){
			count += fileinfo.st_size;
			printf("%-16ld(SYM)\t\t%s\n", count, path);
			return count;
		}else if(S_ISREG(fileinfo.st_mode)){
			count += fileinfo.st_size;
			printf("%-16ld(FILE)\t\t%s\n", count, path);
			return count;
		// If [path] is a directory, get its size and opendir() recurrently
		}else if(S_ISDIR(fileinfo.st_mode)){
			count += fileinfo.st_size;
		// If [path] is not a symbolic link, regular file or directory, print error
		}else{
			fprintf(stderr, "\"%s\" is not a directory, symbolic link or a regular file\n", path);
			exit(EXIT_FAILURE);
		}
	}

	DIR* d = opendir(path);
	if(!d){
		fprintf(stderr, "opender(\"%s\") error\n", path);
		exit(EXIT_FAILURE);
	}

	// recurrently opendir()
	struct dirent* entry;
	while(entry = readdir(d)){
		// do not read .. and .
		if(!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".")) continue;
		// Consider only dir, link, and reg
		if(entry->d_type == DT_DIR || entry->d_type == DT_LNK || entry->d_type == DT_REG){
			// generate file path
			char* path2 = malloc(sizeof(char) * (strlen(path) + strlen(entry->d_name) + 16));
			sprintf(path2, "%s/%s", path, entry->d_name);
			// [size] is used to represent the size of [path2]
			unsigned long int size = 0;
			if(lstat(path2, &fileinfo) == -1){
				fprintf(stderr, "lstat() error at %s\n", path2);
				exit(EXIT_FAILURE);
			}
			size += fileinfo.st_size;

			// Get the sizeof directory recurrently
			if(entry->d_type == DT_DIR){
				size += calculateDir(path2, 0);
			}

			// It is only when calcuateDir() is first round that we print the [size]
			if(first){
				printf("%-16ld(", size);
				switch(entry->d_type){
					case DT_DIR:
						printf("DIR");
						break;
					case DT_LNK:
						printf("SYM");
						break;
					case DT_REG:
						printf("FILE");
						break;
				}
				printf(")\t\t%s\n", entry->d_name);
			}
			count += size;
			free(path2);
		}
	}
	closedir(d);
	return count;
}

int main(int argc, char** args){
	printf("Total Size: %ld Bytes\n", calculateDir(argc > 1 ? args[1] : ".", 1));
	return 0;
}
