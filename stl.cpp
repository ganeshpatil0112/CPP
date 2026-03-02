#include<iostream>
#include<vector>
#include<deque>
#include<map>
#include<string>
#include<algorithm>
using namespace std;

class stl
{
    public:
    stl()
    {
        cout<<"STL here..\n";
    }
    // ~stl()
    // {
    //     cout<<"STL Over XXX\n";
    // }
    virtual void print() = 0; //pure virtual function
    virtual void print(int);
    virtual void operations()=0; //pure virtual function
};

class vectores: public stl
{
    vector <int> carspeed = {1,3,5,6};
    public:
    vectores()
    {
        cout<<"\nVectores here..\n"<<endl;   
    }
    ~vectores()
    {
        cout<<"Vectores Over XXX\n";
    }
    void print() override;
    void operations() override;
};

class deques:public stl
{
    deque<string> cars = {"BMW", "AUDI", "VOLVO"};
    public:
    deques()
    {
        cout<<"\nDeque here...\n";
    }
    ~deques()
    {
        cout<<"Deque Over XXX\n";
    }
    void print() override;
    void operations() override;
};

class maps:public stl
{
    map<int, string> rank = {{10,"Tenth"},{3, "Third"}, {2,"Second"}};
    map<string, int> ranks = {{"Tenth", 10}, {"First", 1}, {"Seventh", 7},{"Fourth", 4}};
    map<string, string> rankss = {{"Ganesh", "Patil"}, {"Tejas", "Borase"}, {"Gopal", "Patil"}, {"Asir", "Shaikh"}};
    public:
    void print() override;
    void operations() override;
};

void maps::print()
{
    auto it = rankss.begin();
    while(it != rankss.end())
    {
        cout<<it->first<<" : "<<it->second<<endl;
        it++;
    }
    //cout<<rank[1]<<endl;
}

void maps::operations()
{
    //update and .find() value of map
    rank[4] = "Fourth";
    rankss.insert({"Akshay", "Borase"});
    print();
    string value = to_string(ranks["Tenth"]);
    cout<<endl<<ranks["Tenth"]<<", At: "<<ranks.at("Tenth")<<", Size: "<<ranks.size()<<", Count: "<<ranks.count("Tenth")<<", ";

    // auto it1 = find(ranks.begin(), ranks.end(), "First");       This find function not works in map
    // cout<<"\nFind stl function : "<<it1->second<<endl;
    
    auto it = ranks.find("First");
    if(it != ranks.end())
    {
        cout<<"Find: {"<<it->first<<":"<<it->second<<"}";
    }
}


int main() {
    std::cout << "Hello World STL!\n";
    vectores vecobj;
    stl *stlobj = &vecobj;
    stlobj->print();
    stlobj->operations();
    
    deques deqobj;
    stlobj = &deqobj;
    stlobj->print();
    stlobj->operations();
    
    maps mapobj;
    stlobj = &mapobj;
    stlobj->operations();
    stlobj->print();
    
    return 0;
}

void stl::print(int x)
{
    cout<<"X : "<<x;
}

void stl::print()
{
    cout<<"STL print()\n";
}

void vectores::print()
{
    for(auto a : carspeed)
    {
        cout<<a<<", ";
    }cout<<endl;
}

void vectores::operations()
{
    //find() in all stl
    cout<<"vectore capacity : "<<carspeed.capacity(); //print total capacity of vector
    cout<<"\nvectore size : "<<carspeed.size();
    carspeed.push_back(9);
    carspeed.push_back(10);
    carspeed.pop_back();
    carspeed.pop_back();
    carspeed.resize(5,1); //resize vector size to 5 and fill new elements with 1
    carspeed.shrink_to_fit();
    carspeed.push_back(10);
    cout<<"\nvectore capacity : "<<carspeed.capacity();
    cout<<"\nvectore size : "<<carspeed.size();

    vector <int>::iterator it = carspeed.begin();
    it = max_element(carspeed.begin(), carspeed.end());

    auto it1 = min_element(carspeed.begin(), carspeed.end());

    cout<<"\nMax : "<<*it<<"\nMin : "<<*it1;

    auto it2 = find(carspeed.begin(), carspeed.end(), 5);
    cout<<"\nFind : Element 5 is present at position : "<<*it2<<endl;

    carspeed.erase(carspeed.begin()+3);
    sort(carspeed.begin(), carspeed.end());
    
    
    cout<<"\nAt : "<<carspeed.at(4)<<" First : "<<carspeed.front()<<" Last : "<<carspeed.back()<<endl;
    
    for(vector<int>::iterator it = carspeed.begin(); it != carspeed.end(); it++){
        cout<<*it<<" ";
    }cout<<endl;

    ////In C++, the vector assign() is a built-in method used to assign the new values to the given vector by replacing old ones.
    // It also modifies the size of the vector according to the given number of elements.
    //Use assign() when you want to completely replace all vector contents with new repeated values.
    // Before assign(): carspeed = {1, 3, 5, 6, 1, 10}
    // carspeed.assign(1, 11); //assign 1 element with value 11 and resize vector to 1
    // After assign():  carspeed = {11}

    //insert() inserts element(s) at a specified position in the vector, shifting existing elements to the right.
    vector<int> vec = {1, 2, 3, 4};
    vec.insert(vec.begin() + 2, 99);
    // Result: {1, 2, 99, 3, 4}
    //                 ↑ inserted at index 2

    // vector<int> vec = {1, 2, 3};
    // vec.insert(vec.begin(), 0);
    // Result: {0, 1, 2, 3}

    // vector<int> vec = {1, 5};
    // vec.insert(vec.begin() + 1, 3, 99);
    // Result: {1, 99, 99, 99, 5}
    //             ↑  3 copies of 99

    // vector<int> vec1 = {1, 2, 7, 8};
    // vector<int> vec2 = {3, 4, 5, 6};

    // vec1.insert(vec1.begin() + 2, vec2.begin(), vec2.begin() + 3);
    // Result: {1, 2, 3, 4, 5, 7, 8}
    //              ↑  inserted 3,4,5 from vec2

}

void deques::print()
{
    for(auto c : cars)
    {
        cout<<c<<",\t";
    }cout<<endl;
    //cout<<cars[1]<<endl;
}

void deques::operations()
{
    
    cars.push_back("FARARI");
    cars.push_front("TARZEN");
    cars.pop_back();
    auto it = find(cars.begin(), cars.end(), "BMW");
    //cars.erase(it);
    cars.insert(cars.begin()+1, "Jaguar");
    it = min_element(cars.begin(), cars.end());
    cout<<"Min : "<<*it<<endl;
    
    sort(cars.begin(), cars.end());
    cout<<"\nSorting : ";
    for(deque<string>::iterator it = cars.begin(); it != cars.end(); it++)
    {
        cout<<*it<<", ";
    }cout<<endl;

    cout<<"\nReverse : ";
    reverse(cars.begin(), cars.end());
    for(deque<string>::iterator it = cars.begin(); it != cars.end(); it++)
    {
        cout<<*it<<", ";
    }cout<<endl;
    
    cout<<"First : "<<cars.front()<<", Last : "<<cars.back()<<", Size : "<<cars.size()<<endl;
    for(deque<string>::iterator it = cars.begin(); it != cars.end(); it++)
    {
        cout<<*it<<", ";
    }cout<<endl;
    
}
