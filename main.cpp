#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include "stl.cpp"
#include "pointers.cpp"
#include "multithreading.cpp"
#include "dynamic_memory.cpp"

using namespace std;

void stl()
{
    //vectores vecobj;
    //stl stlobj;
    //stlobj.print();
    //vecobj.print();
    //vecobj.operations();
    
    //deques deqobj;
    //deqobj.print();
    //deqobj.operations();
    
    //maps mapobj;
    //mapobj.operations();
    //mapobj.print(rankss);
}

void multithreading()
{
    //multithreading threadobject1, threadobject2;
    //int value = 1000000;

    //auto start = chrono::steady_clock::now();
    //cout<<"Type : "<<typeid(start).name()<<endl;

    // thread t1(&multithreading::mutex_operations, &threadobject1, value);
    // thread t2(&multithreading::mutex_operations, &threadobject2, value);

    // cout<<"Main thread id : "<<this_thread::get_id()<<endl;

    // t1.join();
    // t2.join();

    // cout<<"Count : "<<multithreading::count<<endl;
}

//void dynamic_memory();

//function poniter
void fun(void (*funtocall)())
{
    funtocall();
}

int main()
{
    //fun(memory_management);

    try
    {
        std::cout << "Hello World C++! = Version = "<<__cplusplus<<endl;
        //fun(stl);
        //fun(multithreading);
        fun(memory_management);

        return 0;
        
    }
    catch (const exception& e) {
        cout << "Exception " << e.what() << endl;
    }
    catch(int *e)
    {
        cout<<"Int Pointer exception: "<<e<<endl;
    }
    catch(...)
    {
        cout<<"exception occured";
    }
    
}