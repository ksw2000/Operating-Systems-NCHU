import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;
import java.util.stream.LongStream;

public class ForkJoinSum implements sumOfArray{
    // create forkjoin pool
    private ForkJoinPool pool;

    public ForkJoinSum(){
        pool = new ForkJoinPool();
    }

    // write the forkjointask with RecursiveTask
    private static class SumTask extends RecursiveTask<Long>{
        private long[] numbers;
        private int from;
        private int to;

        public SumTask(long[] numbers, int from, int to){
            this.numbers = numbers;
            this.from = from;
            this.to = to;
        }

        @Override
        protected Long compute(){
            if(to-from < 6){
                long total = 0;
                for(int i=0; i<=to; i++){
                    total += numbers[i];
                }
                return total;
            }else{
                int split = (from + to) >> 1;
                SumTask f1 = new SumTask(numbers, from, split);
                SumTask f2 = new SumTask(numbers, split+1, to);
                f1.fork();
                return f2.compute() + f1.join();
            }
        }
    }

    public long sumUp(long[] numbers){
        return pool.invoke(new SumTask(numbers, 0, numbers.length-1));
    }
}
