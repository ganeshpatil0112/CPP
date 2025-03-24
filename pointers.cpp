#include <iostream>
using namespace std;

class Pointers{
    public:
    int x = 5, y = 10;
    char a = 'G';
    double d = 10000;
    float f = 10.50;
    string name = "Ganesh";
    char str1[20] = "C programming";
    int intarray[5] = {1, 2, 3, 4, 5};
    char chararray[5] = {'a', 'b', 'c', 'd', 'e'};
    string stringarray[5] = {"string1", "string2", "string3", "string4", "string5"}; 
    
    int *x1, *y1 = &y;
    
    void printstring(char *c)
    {
        //*c = 'A';
        while(*c)
            cout<<"c: "<<*c++<<endl;
    }
    
    /*void printstring(const char* c1, const char* c2, const char* c3, const char* c4)
    {
        //cout<<&c1<<" "<<c1<<" "<<*c1<<endl;
        cout<<&c2<<" "<<c2<<" "<<*c2<<endl;
        cout<<&c3<<" "<<c3<<" "<<*c3<<endl;
        cout<<&c4<<" "<<c4<<" "<<*c4<<endl;
    }*/
    
    
};
/*int main() {
    cout << "Hello Pointers!\n";
    try
    {
        Pointers obj;
        
        char a = 'G';
        char *b = &a;
        string name = "Ganesh";
        char str1[20] = "C programming";
        char chararray[5] = {'a', 'b', 'c', 'd', 'e'};
        string stringarray[5] = {"string1", "string2", "string3", "string4", "string5"}; 
        obj.printstring(chararray);
        cout<<"*b: "<<*b<<endl;
        //obj.printstring(a, name, chararray, stringarray);
        
        
        int *x2, *x3, **x4;
        x2 = &obj.y;
        //cout<<"x2 : "<<*x2<<endl;
        if(x2 != NULL)
        {
            //cout<<"OK"<<endl;
            //throw x2;
        }
        else
        {
            throw runtime_error("Not valid");
        }
        
        *x2 = obj.x;
        //cout<<"x2 : "<<*x2<<endl;
        x3 = &obj.x;
        obj.x1 = &obj.x;
        //cout<<"x address: "<<&obj.x<< " and value: "<<obj.x<<endl;
        //cout<<"x1 address: "<<&obj.x1<<" x1 value: "<<obj.x1<<" value of pointer: "<<*obj.x1<<endl;
        x4 = &obj.x1;
        //cout<<"*x4 address: "<<*x4<<" x4 value: "<<x4<<" &x4 value: "<<&x4<<" **x4 "<<**x4<<endl;
        
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
    
}*/