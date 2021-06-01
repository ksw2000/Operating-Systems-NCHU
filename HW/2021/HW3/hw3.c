#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>		// dirent()
#include<sys/stat.h>		// lstat()
#include<pwd.h>			// getpwuid()
#include<grp.h>			// getgrpid()
#include<time.h>		// strftime(), localtime()
#include<unistd.h>
#include<sys/ioctl.h>		// ioctl()

typedef struct fileInfo{
	char rwx[11];
	char uname[256];
	char gname[256];
	char size[11];
	char time[16];
	char color[16];
	char name[256];
	char* link;
}FileInfo;

typedef struct fileList{
	int cap;
	int len;
	FileInfo** list;
	void(*add)(struct fileList* this, FileInfo* el);
}FileList;

void fileList_add(FileList* this, FileInfo* el){
	// increase capacity
	if(this->len == this->cap){
		this->cap = this->cap << 1;
		FileInfo** new = malloc(this->cap * sizeof(*new));
		int i;
		for(i = 0; i < this->len; i++){
			new[i] = this->list[i];
		}
		this->list = new;
	}
	this->list[this->len++] = el;
}

FileList* __FileList__(){
	FileList* new = malloc(sizeof(*new));
	new->len  = 0;
	new->cap  = 16;
	new->list = calloc(new->cap, sizeof(*new->list));
	new->add  = fileList_add;
	return new;
}

// 1. __FileList()__ can construct a data type FileList
// 2. FileList is an array list, which can records fileInfo

int getTerminalWidth();			// get the length of one line in terminal
char* human_time(time_t);		// convert time_t to user-friendly string
int cmp(const void*, const void*);	// sorting by filename before printing file list

// ./myls		= 	ls
// ./myls -l		= 	ls -l
// ./myls file		= 	ls file
// ./myls -l file	= 	ls -l file
// ./myls file -l	= 	ls -l file
int main(int argc, char** args){
	// (0) Parse arguments
	char* dir = ".";
	int listMode = 0;
	int showHint = 0;
	if(argc == 2){
		if(!strcmp(args[1], "-l")){
			listMode = 1;
		}else{
			dir = args[1];
		}
	}else if(argc == 3){
		if(!strcmp(args[1], "-l")){
			dir = args[2];
			listMode = 1;
		}else if(!strcmp(args[2], "-l")){
			dir = args[1];
			listMode = 1;
		}else{
			showHint = 1;
		}
	}else if(argc > 3){
		showHint = 1;
	}

	if(showHint){
		printf("please use %s -l [filepath]\n", args[0]);
		printf("or use %s [filepath]\n", args[0]);
		return 0;
	}

	// (1) Open directory
	DIR* d = opendir(dir);
	if(!d){
		fprintf(stderr, "opendir(\"%s\") error\n", dir);
		exit(EXIT_FAILURE);
	}

	FileList* list = __FileList__();

	// record the max length
	int max_uname = 0;
	int max_gname = 0;
	int max_size  = 0;

	// read directory
	struct dirent* dd;
	while(dd = readdir(d)){
		// generate path
		char filepath[1024];
		sprintf(filepath, "%s/%s", dir, dd->d_name);

		// get file info
		struct stat info;
		if(lstat(filepath, &info)) continue;	// if not success, skip

		// get protection
		char rwx[11];
		rwx[0] = S_ISLNK(info.st_mode)   ? 'l' : \
			 S_ISDIR(info.st_mode)   ? 'd' : '-';
		rwx[1] = info.st_mode & S_IRUSR  ? 'r' : '-';
		rwx[2] = info.st_mode & S_IWUSR  ? 'w' : '-';
		rwx[3] = info.st_mode & S_ISUID  ? \
			 (info.st_mode & S_IXUSR ? 's' : 'S') : \
                         info.st_mode & S_IXUSR  ? 'x' : '-';
		rwx[4] = info.st_mode & S_IRGRP  ? 'r' : '-';
		rwx[5] = info.st_mode & S_IWGRP  ? 'w' : '-';
		rwx[6] = info.st_mode & S_ISGID  ? \
			 (info.st_mode & S_IXGRP ? 's' : 'S') : \
                         info.st_mode & S_IXGRP  ? 'x' : '-';
		rwx[7] = info.st_mode & S_IROTH  ? 'r' : '-';
		rwx[8] = info.st_mode & S_IWOTH  ? 'w' : '-';
		rwx[9] = info.st_mode & S_ISVTX  ? 't' : \
			 info.st_mode & S_IXOTH  ? 'x' : '-';
		rwx[10] = '\0';

		// text color
		char color[16];
		if(rwx[0] == 'l'){						// is symbolic link
			strcpy(color, "\033[01;36m");
		}else if(rwx[0] == 'd'){					// is directory
			strcpy(color, "\033[01;34m");
		}else if(rwx[3] == 's' || rwx[3] == 'S'){			// setuid
			strcpy(color, "\033[37;41m");
		}else if(rwx[6] == 's' || rwx[6] == 'S'){			// setgid
			strcpy(color, "\033[30;43m");
		}else if(rwx[3] == 'x' || rwx[6] == 'x' || rwx[9] == 'x'){	// executable
			strcpy(color, "\033[01;32m");
		}else{								// normal
			strcpy(color, "\033[0m");
		}

		// record information by FileInfo
		FileInfo* finfo = calloc(1, sizeof(FileInfo));
		strcpy(finfo->rwx, rwx);
		strcpy(finfo->uname, getpwuid(info.st_uid)->pw_name);
		strcpy(finfo->gname, getgrgid(info.st_gid)->gr_name);
		sprintf(finfo->size, "%ld", info.st_size);
		strcpy(finfo->time, human_time(info.st_mtime));
		strcpy(finfo->color, color);
		strcpy(finfo->name, dd->d_name);

		// process symbolic link
		if(rwx[0] == 'l'){
			finfo->link = calloc(256, sizeof(char));
			readlink(filepath, finfo->link, 256 * sizeof(char));
		}else{
			finfo->link = NULL;
		}

		// append the file's info into Table
		list->add(list, finfo);

		// calculate the max length
		if(strlen(finfo->uname) > max_uname) max_uname = strlen(finfo->uname);
		if(strlen(finfo->gname) > max_gname) max_gname = strlen(finfo->gname);
		if(strlen(finfo->size)  > max_size)  max_size  = strlen(finfo->size);
	}

	// (2) Fetch all data from list
	int i, j, k;
	FileInfo* l;

	// sorting
	qsort(list->list, list->len, sizeof(FileInfo*), cmp);
	if(listMode){
		for(i=0; i < list->len; i++){
			l = list->list[i];
			printf("%s %-*s %-*s %*s %s %s%s\033[0m%s%s\n", \
			l->rwx, max_uname, l->uname, max_gname, l->gname, \
			max_size, l->size, l->time, l->color, l->name, (l->link)? " -> " : "" , (l->link)? l->link : "");
		}
	}else{
		int row = 1, remainder, col, lineLen;
		int rowNeedPlusOne = 1;
		int printMode = 0;
		int* maxLenPerCol;
		L1:
			if(!printMode){
				remainder = list->len % row;
				col = list->len / row + (remainder ? 1 : 0);
				maxLenPerCol = calloc(col, sizeof(int));
			}
			for(i=0; i<row; i++){
				for(j=0; j<col && row * j + i < list->len; j++){
					l = (j == col - 1 && remainder) ? \
					    list->list[(col-1)*row+i] : \
					    list->list[j*row + i];
					if(!printMode){
						if(strlen(l->name) > maxLenPerCol[j]){
							maxLenPerCol[j] = strlen(l->name);
						}
					}else{
						if(j == 0 && i != 0) printf("\n");
						printf("%s%s\033[0m  ", l->color, l->name);
						for(k = 0; k < maxLenPerCol[j] - strlen(l->name); k++) printf(" ");
					}
				}
			}
			if(printMode) goto L2;

			lineLen = 0;
			for(i=0; i<col; i++){
				lineLen += maxLenPerCol[i] + 2;
			}
			if(lineLen > getTerminalWidth()){
				row++;
				free(maxLenPerCol);
				goto L1;
			}else{
				printMode = 1;
				goto L1;
			}
		L2:
			printf("\n");
	}
	closedir(d);
	return 0;
}

int getTerminalWidth(){
	struct winsize w;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1){
		return w.ws_col;
	}
	return 100;
}

char* human_time(time_t raw){
	char* str = malloc(sizeof(char) * 16);
	strftime(str, 16, "%x %H:%M", localtime(&raw));
	return str;
}

int cmp(const void* elem1, const void* elem2){
	int i, delta;
	char* name1 = (*(FileInfo**)elem1)->name;
	char* name2 = (*(FileInfo**)elem2)->name;
	for(i = 0; name1[i] != '\0' && name2[i] != '\0'; i++){
		delta = name1[i] - name2[i];
		if(delta) return delta > 0 ? 1 : -1;
	}
	return (name1[i] != '\0') ? 1 : (name2[i] != '\0') ? -1 : 0;
}
