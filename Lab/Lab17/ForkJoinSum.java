import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;
import java.util.stream.LongStream;
import java.util.Arrays;

public class ForkJoinSum{
    private ForkJoinPool pool;

    public ForkJoinSum(){
        pool = new ForkJoinPool();
    }

    private static class SortTask extends RecursiveAction{
        private long[] arr;
        private int lo;
        private int hi;

        public SortTask(long[] arr, int lo, int hi){
            this.arr = arr;
            this.lo = lo;
            this.hi = hi;
        }

        protected void compute(){
            if(hi-lo < 2){
                Arrays.sort(arr, lo, hi);
            }else{
                int mid = (lo + hi) >>> 1;
                SortTask f1 = new SortTask(arr, lo, mid);
                SortTask f2 = new SortTask(arr, mid, hi);

                f1.fork();
                f2.compute();
                f1.join();
                // i.e., invoke(f1); invoke(f2);
                //       or just use invokeAll() invokeAll(f1, f2);

                merge(lo, mid, hi);
            }
        }

        void merge(int lo, int mid, int hi){
            long[] buf = Arrays.copyOfRange(arr, lo, mid);
            for(int i=0, j=lo, k=mid; i<buf.length; j++){
                arr[j] = (k == hi || buf[i] < arr[k])? buf[i++] : arr[k++];
            }
        }
    }

    public void sort(long[] arr){
        pool.invoke(new SortTask(arr, 0, arr.length));
    }
}
