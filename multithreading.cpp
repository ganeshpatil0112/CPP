#include <mutex>
#include <iostream>
#include <deque>
#include <thread>
#include <condition_variable>
// using namespace std;

std::mutex m;
std::timed_mutex tmx;       // for the Deadlock we have "timed_mutex" std::timed_mutex tmx; tmx.try_lock_for(std::chrono::seconds(1)); - another thread will wait to unlock() for given time
std::recursive_mutex rm;

class multithreading
{
    public:
    
    static int count;
    //int count = 0;
    void mutex_operations(int x)
    {

        //m.lock() - one thread execute at a time onthers will stop untill first thread complete. 
        //we can also use if(m.try_lock()) - this will not block another thread and will skip the execution of critical area if first thread is not completed.
        //if (timed_mutex tmx.try_lock_for(chrono::seconds(1)) ) - another thread will wait to unlock() for given time and then execute if unlock within that time unless it will skip the critical area.
        if(tmx.try_lock_for(std::chrono::seconds(1)))
        {
            for(int i = 0; i < x; i++)
            {
                // for the Recursiona and Loops we have "recursive_mutex" std::recursive_mutex rm; rm.lock(); rm.unlock();
                // recursive_mutex allows to same thread to lock the mutex multiple times. and same times we to unlock as well.
                //rm.lock();
                count++;
                //rm.unlock();
            }

            //std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        tmx.unlock();
    }
    
};

int multithreading::count = 0;


// ============================================================================
// Producer–Consumer Problem  (std::mutex + std::condition_variable)
// ----------------------------------------------------------------------------
// Two threads share a bounded buffer:
//   - Producer pushes items into the buffer.   Must wait if buffer is FULL.
//   - Consumer pops items from the buffer.     Must wait if buffer is EMPTY.
//
// Why condition_variable?
//   Without it, threads would have to busy-wait (spin) on the condition,
//   wasting CPU. condition_variable lets a thread SLEEP until another
//   thread notifies it that the condition may now be true.
//
// Why unique_lock (not lock_guard)?
//   cond.wait() must atomically UNLOCK the mutex while sleeping and
//   RE-LOCK it on wakeup. Only unique_lock supports that.
// ============================================================================

std::mutex                mu;             // Protects the shared buffer
std::condition_variable   cond;           // Signals between producer & consumer
std::deque<int>           buffer;         // The shared bounded buffer
const unsigned int        maxBufferSize = 50;

// ---------------- Producer ----------------
void producer(int val)
{
    while (val)
    {
        // unique_lock is required because condition_variable::wait()
        // needs the ability to unlock/relock the mutex.
        std::unique_lock<std::mutex> locker(mu);

        // Wait until buffer has space.
        // The predicate (lambda) protects against "spurious wakeups":
        // wait() will only return when buffer.size() < maxBufferSize.
        cond.wait(locker, [](){ return buffer.size() < maxBufferSize; });

        // ----- Critical section -----
        buffer.push_back(val);
        std::cout << "Produced: " << val << std::endl;
        val--;
        // ----------------------------

        // Unlock BEFORE notifying so the woken consumer doesn't
        // immediately block again on the mutex (minor optimization).
        locker.unlock();

        // Wake up one waiting consumer so it can re-check its predicate.
        cond.notify_one();
    }
}

// ---------------- Consumer ----------------
void consumer()
{
    while (true)
    {
        std::unique_lock<std::mutex> locker(mu);

        // Sleep until the buffer has at least one item.
        cond.wait(locker, [](){ return buffer.size() > 0; });

        // ----- Critical section -----
        int val = buffer.back();
        buffer.pop_back();
        std::cout << "Consumed: " << val << std::endl;
        // ----------------------------

        locker.unlock();

        // Notify producer in case it was blocked on a full buffer.
        cond.notify_one();
    }
}

// ---------------- Driver ----------------
// Uncomment main() below to run the producer–consumer demo standalone.
//
// int main()
// {
//     std::thread t1(producer, 100);
//     std::thread t2(consumer);
//     t1.join();
//     t2.join();
//     return 0;
// }