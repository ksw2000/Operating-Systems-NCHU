public class Exercise1{
    static int count = 0;

    public static synchronized void inc(){
        for(int j=0; j<25000000; j++){
            count++;
        }
    }

    public static synchronized void dec(){
        for(int j=0; j<25000000; j++){
            count--;
        }
    }

    public static void main(String[] args){
        Thread[] t = new Thread[4];

        t[0] = new Thread(()->{inc();});
        t[0].start();
        t[1] = new Thread(()->{inc();});
        t[1].start();
        t[2] = new Thread(()->{dec();});
        t[2].start();
        t[3] = new Thread(()->{dec();});
        t[3].start();

        try{
            for(Thread v : t){
                v.join();
            }
        }catch(InterruptedException e){}

        System.out.println("count: "+count);
    }
}
