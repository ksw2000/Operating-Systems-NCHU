# Lab14 Java Monitor

## Exercise 1

+ A program declares a global variable count and then creates 4 identical threads.
+ Two of these threads increments the same global variable 25,000,000 times. The other threads decrements the same global variable 25,000,000 times.
+ The main program waits for the 4 threads to complete.
+ Using the `Synchronized` to solve race condition problem.

## Exercise 2

Using the `ReentrantLock` to solve the multiple producer multiple consumer problem.

+ The maximum amount of products is 10.
+ The minimum amount of products is 0.
+ If the total amount of product is 0 and consumer want to consume, print "empty" on the screen.
+ If the total amount of product is 10 and producer want to produce, print "full" on the screen

