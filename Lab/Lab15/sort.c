void selection_sort(int* arr, int len){
    int i, j, k;
    for(i=0; i<len-1; i++){
        k = i;
        // find min
        for(j=i+1; j<len; j++){
            if(arr[j] < arr[k]){
                k = j;
            }
        }
        // swap
        arr[i] = arr[i] ^ arr[k];
        arr[k] = arr[i] ^ arr[k];
        arr[i] = arr[i] ^ arr[k];
    }
}
