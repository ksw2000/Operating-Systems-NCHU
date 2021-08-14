/*
    ReentranLock is owned by a single thread
    provide mutually exclusive access to a shared resource
*/

import java.util.concurrent.locks.*;

public class Exercise2{
    private static Lock lock = new ReentrantLock();
    private static Condition threadCond = lock.newCondition();
    private static int product = 0;

    public static void main(String[] args){
        Thread t1, t2;
        t1 = new Thread(()->{
            while(true){
                lock.lock();
                try{
                    while(product == 10){
                        try{
                            System.out.printf("\tFULL\n");
                            threadCond.await();
                        }catch(InterruptedException e){}
                    }

                    product++;
                    System.out.println("\tProduced : " + product);
                    threadCond.signal();
                }finally{
                    lock.unlock();
                }
            }
        });

        t2 = new Thread(()->{
            while(true){
                lock.lock();
                try{
                    while(product == 0){
                        try{
                            System.out.printf("EMPTY\n");
                            threadCond.await();
                        }catch(InterruptedException e){}
                    }

                    product--;
                    System.out.println("Consumed : " + product);
                    threadCond.signal();
                }finally{
                    lock.unlock();
                }
            }
        });

        t1.start();
        t2.start();

        try{
            t1.join();
            t2.join();
        }catch(InterruptedException e){}
    }
}
