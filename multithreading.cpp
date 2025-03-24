#include <mutex>
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