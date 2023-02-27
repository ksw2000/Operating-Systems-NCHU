#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef enum{
    false, true
}bool;

typedef struct{
    char* data;
    int frame;
}logical_mem;

int main(){
    // initialize logical mem
    logical_mem p[2][3] = {
        {{data: "ab"}, {data: "ef"}, {data: "cd"}},
        {{data: "rx"}, {data: "yy"}, {data: "zz"}}
    };

    // initialize free frame list
    bool fflist[16];
    int i, j, r;
    char mem[16][2];
    for(i=0; i<16; i++) fflist[i] = false;
    srand(time(NULL));

    // page p[0] and p[1]
    for(j=0; j<2; j++){
        for(i=0; i<3; i++){
            do{
                r = rand() % 16;
            }while(fflist[r]);
            fflist[r] = true;
            p[j][i].frame = r;
            mem[r][0] = p[j][i].data[0];
            mem[r][1] = p[j][i].data[1];
        }
    }

    // print result
    for(j=0; j<2; j++){
        printf("process%d's page_table is:\n", j);
        printf("page\tframe\n");
        for(i=0; i<3; i++){
            printf("%d\t%d\n", i, p[j][i].frame);
        }

        printf("proccess%d's logical memory is:\n", j);
        printf("page\tdata\n");
        for(i=0; i<3; i++){
            printf("%d\t%s\n", i, p[j][i].data);
        }
    }

    printf("physical memory is:\n");
    printf("fram\toffset\tdata\n");
    for(i=0; i<16; i++){
        printf("%d\t%d\t%c\n", i, 0, (fflist[i])? mem[i][0] : ' ');
        printf("%d\t%d\t%c\n", i, 1, (fflist[i])? mem[i][1] : ' ');
    }

    printf("free frame list is:\n");
    for(i=0; i<16; i++){
        if(!fflist[i]){
            printf("%d ", i);
        }
    }
    return 0;
}
