#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <memory>
#include <thread>
using namespace std;


class order
{
    public:
    int id;
    string name;

    order(){};
    order(int id, string name):id(id),name(name){};
    void print()
    {
        cout<<"\nOrder Id : "<<id<<", Order Name : "<<name;
    }

    // //for find(), unordered_set
    // bool operator==(const order& other) const
    // {
    //     return id==other.id && name == other.name;
    // }

    // //for sort(), max_element(), set comparison
    // bool operator<(const order& other) const
    // {
    //     return id<other.id;
    // }

    // //for min_element()
    // bool operator>(const order& other) const
    // {
    //     return id>other.id;
    // }

};

struct OrderHash
{
    size_t operator()(const order& other) const{
        return hash<int>()(other.id) ^ hash<string>()(other.name);
    }
};


void fun_vector()
{
    cout<<"\n\n---Vector---"<<endl;
    vector<shared_ptr<order>> shr_order_vec; //= {order(1, "order1"), order(2, "order2")};
    shr_order_vec.push_back(shared_ptr<order>(new order(1, "order1")));

    vector<order> order_vec;
    order_vec.push_back(order(2, "order2"));
    order_vec.emplace_back(order(3, "order3"));
    order_vec.emplace_back(order(4, "order4"));
    order_vec.push_back(order(1, "order1"));

    //Insert element at the given position
    order_vec.insert(order_vec.end()-1, order(10, "order10"));

    //vector emplace() is used to insert elements at the given position in a vector by constructing it in-place within the vector,
    // rather than creating a temporary object and then moving or copying it into the vector.
    order_vec.emplace(order_vec.end(), order(11, "order11"));

    for(auto it : order_vec)
    {
        it.print();
    }

    cout<<"\nElement At 1st position : ";
    order_vec[1].print();
    order_vec.at(1).print();

    
    //In C++, the vector assign() is a built-in method used to assign the new values to the given vector by replacing old ones.
    // It also modifies the size of the vector according to the given number of elements.
    
    //order_vec.assign(1, order(5, "order5"));

    sort(order_vec.begin(), order_vec.end());

    //for(vector<order>::iterator it = order_vec.begin(); it != order_vec.end(); it++)
    //for(int i = 0; i < order_vec.size(); i++)
    cout<<"\n\nPrint sorted array"<<endl;
    for(auto it : order_vec)
    {
        it.print();
    }

    //for find() we have to overload the == operator
    vector<order>::iterator it = find(order_vec.begin(), order_vec.end(), order(2, "order2"));
    if(it != order_vec.end())
    {
        cout<<"\nOrder id is present!!"<<endl;
    }
    
    it = min_element(order_vec.begin(), order_vec.end());
    cout<<"\nMin Element : "; it->print();

    it = max_element(order_vec.begin(), order_vec.end());
    cout<<"\nMax Element : "; it->print();

    cout<<"\nDeleted the Max Element : ";order_vec.erase(it);

    it = max_element(order_vec.begin(), order_vec.end());
    cout<<"\nMax Element : "; it->print();
}

void fun_set()
{
    cout<<"\n\n---Ordered Set---"<<endl;
    set<order> order_set;

        // Insert orders into the unordered_set
    order_set.insert(order(1, "order1"));
    order_set.insert(order(2, "order2"));
    order_set.insert(order(3, "order3"));

    //for find() we have to overload the == operator
    // Check if an order exists
    order search_order(2, "order2");
    if (order_set.find(search_order) != order_set.end()) {
        cout << "Order found: ID = " << search_order.id << ", Name = " << search_order.name << endl;
    } else {
        cout << "Order not found." << endl;
    }

}

void fun_unset()
{
    cout<<"\n\n---Unordered Set---"<<endl;
    // Create an unordered_set with custom hash and equality
    unordered_set<order, OrderHash> order_unset;

    // Insert orders into the unordered_set
    order_unset.insert(order(1, "order1"));
    order_unset.insert(order(2, "order2"));
    order_unset.insert(order(3, "order3"));

    // Check if an order exists
    order search_order(2, "order2");
    if (order_unset.find(search_order) != order_unset.end()) {
        cout << "Order found: ID = " << search_order.id << ", Name = " << search_order.name << endl;
    } else {
        cout << "Order not found." << endl;
    }


}

void fun_map()
{
    cout<<"\n\n---MAP---"<<endl;
    map<int, order> order_map;

    order_map.insert({1, order(1, "order1")});
    order_map[2] = order(2, "order2");
    order_map[3] = order(3, "order3");
    order_map.at(3) = order(4, "order4");

    for(auto it : order_map)
    {
        cout<<"\n {Key = "<<it.first<<" : Value = "<<it.second.name<<"}";
    }
    

    cout<<"\n\n---Unordered MAP---"<<endl;
    unordered_map<int, order> order_unmap;
    order_unmap.insert({1, order(1, "order1")});
    order_unmap[2] = order(2, "order2");
    order_unmap[3] = order(3, "order3");
    order_unmap.at(3) = order(4, "order4");

    for(auto it : order_map)
    {
        cout<<"\n {Key = "<<it.first<<" : Value = "<<it.second.name<<"}";
    }

}

void fun_list()
{
    cout<<"\n\n---LIST---"<<endl;
    list<order> order_list;

    order_list.push_back(order(1, "order1"));
    order_list.push_front(order(2, "order2"));
    order_list.emplace_back(order(3, "order3"));
    order_list.emplace_front(order(4, "order4"));
    order_list.insert(order_list.begin(), order(5, "order5"));
    order_list.emplace(order_list.end(), order(7, "order7"));

    for(auto it : order_list)
    {
        cout<<"\nValue at Location : "<<it.id<<" = "<<it.name;
    }

}

void fun(void (*function)())
{
    function();
}

int main()
{
    //fun(fun_vector);
    //fun(fun_set);
    //fun(fun_unset);
    fun(fun_map);
    fun(fun_list);
    

    return 0;
}