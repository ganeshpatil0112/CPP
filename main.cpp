#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include "stl.cpp"
#include "pointers.cpp"
#include "multithreading.cpp"

using namespace std;

int main()
{

    try
    {
        std::cout << "Hello World C++!\n";
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

        multithreading threadobject1, threadobject2;
        int value = 1000000;

        //auto start = chrono::steady_clock::now();
        //cout<<"Type : "<<typeid(start).name()<<endl;

        thread t1(&multithreading::mutex_operations, &threadobject1, value);
        thread t2(&multithreading::mutex_operations, &threadobject2, value);

        cout<<"Main thread id : "<<this_thread::get_id()<<endl;

        t1.join();
        t2.join();

        cout<<"Count : "<<multithreading::count<<endl;

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