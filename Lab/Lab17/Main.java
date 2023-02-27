public class Main{
    public static void main(String[] args){
        long[] arr = new long[100];
        for(int i=0; i<100; i++){
            arr[i] = (long)(Math.random() * 100);
        }
        new ForkJoinSum().sort(arr);
        for(int i=0; i<arr.length; i++){
            System.out.printf("%3d ", arr[i]);
            if((i+1) % 10 == 0){
                System.out.println();
            }
        }
    }
}
