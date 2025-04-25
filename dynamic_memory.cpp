#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>
#include <span>
using namespace std;
//void dynamic_memory_management();

class TradeOrder
{
    //private
    int orderid;
    int orderquentity;
    string tradename;
    string ordertype;
    
    void set(int orderid, int orderquentity, string tradename, string ordertype)
    {
        this->orderid = orderid;
        this->orderquentity = orderquentity;
        this->tradename = tradename;
        this->ordertype = ordertype;
    }
    void print()
    {
        ostringstream print1;
        print1<<"\nOrder Id: "<<orderid<<" | Order Quantity: "<<orderquentity;
        char print2[100];
        sprintf(print2," | Order Name: %s |  Order Type: %s",tradename.c_str(), ordertype.c_str());
        cout<<print1.str() + print2;
    }

    void dynamic_memory_management();
    void smart_pointer();

    //friend function
    friend void memory_management();

    public:
    TradeOrder(){};
    TradeOrder(int orderid, int orderquentity, string tradename, string ordertype)
    {
        this->orderid = orderid;
        this->orderquentity = orderquentity;
        this->tradename = tradename;
        this->ordertype = ordertype;
    }

};

class MyObject {
public:
    int value;
    MyObject(int v) : value(v) {}
};

class MyClass {
public:
    MyClass(int value) : value(value) {
        std::cout << "MyClass constructor called with value: " << value << std::endl;
    }
    MyClass(){};
    ~MyClass() {
        std::cout << "MyClass destructor called for value: " << value << std::endl;
    }
    void print() const {
        std::cout << "Value: " << value << std::endl;
    }

private:
    int value;
};

void TradeOrder::dynamic_memory_management()
{
    cout<<"\n---Dynamic Memory Management---"<<endl;
    int *ptrnum = new int(5);
    int num = 5, size = 3, order = 1;

    //The below notes are only in case of Poniter Arrays:
    /* 1--> This will Create Array Pointer and directly alloacte continuous memeory location of 5 integer in directly into HEAP memory.
     Thats why we can direct delete continuous memory location with "delete[] arrnum;"
    */
    int *arrnum = new int[5](10);
    for(auto autonum = 0; autonum < *ptrnum; autonum++)
    {
        cout<<arrnum[autonum];
    }

    /* 2--> This will create Empty Pointer Array variable into STACK memory first, without pointing to any memory location--
     After this we have to manually alloacte the NEW() HEAP memory to every pointer location by iterating through each, 
     and this will Randomaly allaocate memory NOT in continuous location. Thats why at the time of deletion we have to delete each location separatally by iterating through each location.
     This behaviour applicable in case of the ARRAY POINTER only, Premitive data type will work as per the expected. 
     */
    int *arrptrnum[num];

    // 3--> Array variables of premitive 
    int basicarr[] = {1,2,3,4,5};
    
    for(auto autonum = 0; autonum < *ptrnum; autonum++)
    {
        arrnum[autonum] = 9;
        arrptrnum[autonum] = new int(8);
        cout<<arrnum[autonum];
    }

    auto fun = [](int *a){cout<<*a;};
    auto printfun = [](int a){cout<<a;};
    cout<<"\nFor_Each Prt Arr : ";
    for_each(arrptrnum, arrptrnum+num, fun);
    cout<<endl<<"For_Each Nrml Arr : ";
    for_each(basicarr, basicarr+num, printfun);
    cout<<endl;

    for(auto autonum : basicarr)
    {
        cout<<autonum;
    }

    TradeOrder *arrTradeOrder = new TradeOrder[num]();
    for(int i = 0; i < num; i++)
    {
        arrTradeOrder[i].set(order++, order, "GPP", "BUY");
        arrTradeOrder[i].print();
    }
    cout << endl;

    MyObject *objects = new MyObject[3] { MyObject(1), MyObject(2), MyObject(3) };

    // Range-based for loop using pointers
    for (MyObject &obj : std::span(objects, size)) {
        std::cout << obj.value << " ";
    }
    cout << endl;


    {
        delete ptrnum;
        ptrnum = nullptr;
        delete[] arrnum;
        arrnum = nullptr;
        for(int *&i : arrptrnum)
        {
            delete i;
        }
        delete[] arrTradeOrder;
        arrTradeOrder = nullptr;
        delete[] objects;
        cout<<"\nAll dynamacily created objects are Deleted"<<endl;
    }
    
}

/*
Smart Poniters Member Functions:

1. std::unique_ptr Member Functions
get(): Returns the raw pointer managed by the unique_ptr.
release(): Releases ownership of the managed object and returns the raw pointer.
reset(): Deletes the managed object and optionally takes ownership of a new raw pointer.
swap(): Swaps the managed object with another unique_ptr.
operator* and operator->: Access the managed object.

2. std::shared_ptr Member Functions
get(): Returns the raw pointer managed by the shared_ptr.
reset(): Deletes the managed object and optionally takes ownership of a new raw pointer.
swap(): Swaps the managed object with another shared_ptr.
use_count(): Returns the number of shared_ptr instances managing the same object.
unique(): Returns true if the shared_ptr is the only owner of the managed object.
operator* and operator->: Access the managed object.

3. std::weak_ptr Member Functions
lock(): Returns a shared_ptr if the managed object is still available, otherwise returns an empty shared_ptr.
expired(): Returns true if the managed object has been deleted.
use_count(): Returns the number of shared_ptr instances managing the same object.
reset(): Releases the weak_ptr's reference to the managed object.
swap(): Swaps the managed object with another weak_ptr.
*/

void TradeOrder::smart_pointer(){

    cout<<"\n---Smart Poniters---"<<endl;

    int *intptr;
    unique_ptr<int> uni_ptr = make_unique<int>(10);
    unique_ptr<int> uni_ptr1;
    unique_ptr<TradeOrder> uni_objptr = make_unique<TradeOrder>();
    //unique_ptr<TradeOrder> uni_objptr(new TradeOrder());
    //unique_ptr<TradeOrder> uni_objptr(new TradeOrder(101, 101, "GPP", "BUY"));




    // Create a unique_ptr for an array of MyClass objects
    std::unique_ptr<MyClass[]> Uni_Ptr_Array = std::make_unique<MyClass[]>(3);
    // Access and initialize the array elements
    for (int i = 0; i < 3; ++i) {
        Uni_Ptr_Array[i] = MyClass(i + 1); // Assign values to each object
    }
    // Access and print the array elements
    for (int i = 0; i < 3; ++i) {
        Uni_Ptr_Array[i].print();
    }
    // The array will be automatically deleted when uniqueArray goes out of scope
    




    intptr = uni_ptr.get();        // .get() will return reference to the raw poniter
    cout<<"Unique_P : "<<*uni_ptr<<" || Raw_P : "<<*intptr;

    //move sementic - tranfers the ownership to new Poniter Obj efficiently without copying the whole object and leaves existing object in valid state with null values 
    //and if we try to access null pointer then will return undefined behaviour.
    uni_ptr1 = move(uni_ptr);
    //uni_ptr = nullptr;
    cout<<"\nUnique_P1 : "<<*uni_ptr1<<endl;

    //cout<<"Unique_P : "<<*uni_ptr;    This poniter is Null this will return undefined behaviour.
    if(uni_ptr) cout<<"\nUnique_P : "<<*uni_ptr;
    else cout<<"uni_ptr is null";

    uni_objptr->set(101, 101, "GPP", "BUY");
    uni_objptr->print();
    cout<<endl;




    /*
    the release function releases the ownership of the underlying object owned by the unique pointer and sets the unique pointer to nullptr.
     The release function returns a raw pointer to the underlying object. 
    */
    //TradeOrder *rawTrade = uni_objptr.release();

    shared_ptr<string> shr_ptr(new string("GP"));
    shared_ptr<string> shr_ptr1 = make_shared<string>("GP");
    shared_ptr<TradeOrder> shr_objptr = make_shared<TradeOrder>(202, 202, "GPP", "BUY");
    shared_ptr<TradeOrder> shr_objptr1(new TradeOrder(303, 303, "GPP", "BUY"));

    // Below Both swap are valid
    shr_objptr.swap(shr_objptr1);
    swap(shr_objptr, shr_objptr1);

    shared_ptr<TradeOrder> shr_objptr2(shr_objptr1);
    shared_ptr<TradeOrder> shr_objptr3(shr_objptr2);

    weak_ptr<TradeOrder> wek_ptr = shr_objptr3;

    if(!shr_objptr1.unique())
        cout<<"\nTotal number of active strong refernce of Shared_Prt : "<<shr_objptr1.use_count()<<endl;  //:3




    /*The reset() function in C++ smart pointers is used to change the managed object.
      It releases ownership of the currently managed object and can take ownership of a new object, if provided.
      If the smart pointer is the only owner of the current object, that object is deleted.
    */
    cout<<"\nValue of Shr_Ptr Before Reset : ";
    shr_objptr->print();

    TradeOrder *tradeObj = new TradeOrder(404, 404, "GPP", "SELL");
    shr_objptr.reset();             // This will delete the existing pointer completly
    
    shr_objptr.reset(tradeObj);     // This will delete the existing poniter and takes the ownership of new object which passed in argument.
    //shr_objptr.reset(new TradeOrder()); 

    cout<<"\nValue of Shr_Ptr After Reset : ";
    shr_objptr->print();            // If prt is deleted then will return undefind behaviour.

    delete tradeObj;



    /*
    The line wek_ptr->print(); attempts to call the print() method on an object managed by a std::weak_ptr.
     However, std::weak_ptr does not provide direct access to the managed object because it is a non-owning reference.
     To safely access the object, you need to convert the std::weak_ptr to a std::shared_ptr using the lock() method.
    */
    cout<<"\n Weak Prt values : ";
    if(shared_ptr<TradeOrder> weak_to_shr = wek_ptr.lock())
        weak_to_shr->print();
    else
        std::cout << "The object managed by weak_ptr has expired." <<endl;


    shr_objptr1.reset();
    shr_objptr2.reset();
    shr_objptr3.reset();



}

void memory_management()
{
    TradeOrder *TradeObj = new TradeOrder();
    TradeObj->dynamic_memory_management();
    delete TradeObj;

    unique_ptr<TradeOrder> Smart_TradeObj(new TradeOrder());
    Smart_TradeObj->smart_pointer();
    //we not need to delete smart pointers explicitly.

}

// int main()
// {
//     cout<<"Version : "<<__cplusplus;
//     memory_management();

//     return 0;
// }