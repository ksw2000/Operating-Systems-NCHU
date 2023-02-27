#include<stdio.h>
extern void selection_sort(int* arrar, int len);

int main(){
    int arr[] = {15, 22, 41, 8, 35};
    int len = sizeof(arr)/sizeof(int);
    selection_sort(arr, len);
    int i;
    for(i=0; i<len; i++){
        printf("%2d%s", arr[i],\
        (i != len-1)? ", " : "\n");
    }
}
