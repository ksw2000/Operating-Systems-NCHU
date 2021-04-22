public class Jthread{
    volatile static int count;
    public static void main(String[] argv){
        Thread[] threadList = new Thread[4];
        for(int i=0; i<4; i++){
            final int j = i;
            threadList[i] = new Thread(()->{
                for(int k=0; k<250000; k++){
                    count++;
                }
                System.out.printf("Thread%d count = %d\n", j, count);
            });
        }

        try{
          for(Thread v : threadList){
              v.start();
              v.join();
          }
        }catch(InterruptedException e){
        }
        System.out.println("Total Sum = "+count);
    }
}
