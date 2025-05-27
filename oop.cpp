#include <iostream>
using namespace std;

template <typename T> class Calculator
{
    public:
    T a, b;
    Calculator(){}
    Calculator(T x, T y) : a(x), b(y){}

    T add(T a, T b)
    {
        return a+b;
    }

    friend void fun();

};


class shapes
{
    int length, width, circuference, valume;
    //int area;
    int* ptr_area;
    string shapename;

    shapes(string shapename, int length, int width)
    {
        this->shapename = shapename;
        this->length = length;
        this->width = width;
        //area = length*width;
        ptr_area = new int(length*width);
        cout<<"Object is created.."<<endl;
    }
    shapes(){}
    ~shapes()
    {
        delete ptr_area;
        cout<<"\ncalling destructor.."<<endl;
    }

    void start_fun()
    {
        shapes rectangle("Rectangle", 10, 4);
        rectangle.ptr_area = rectangle.find_Area();
        cout<<rectangle;
    }

    public:
    
    int* find_Area()
    {
        ptr_area = new int(length*width+2);
        return ptr_area;
    }

    /*
    The operator<< function is not a member of the class but operates on objects of the class.
    To access private or protected members of the class (like shapename, length, width, and area in your example), the function must be declared as a friend of the class.
    Without the friend keyword, the operator<< function would not have access to these private members.

    Why Pass std::ostream& as a Reference?
    Efficiency: Passing by reference avoids copying the std::ostream object, which is expensive and unnecessary.
    Chaining Support: The operator<< function returns the same std::ostream reference it receives. This allows chaining multiple << operations, like:
    */
    friend ostream& operator<<(ostream& os, const shapes& shape)
    {
        os<<"\nShape : "<<shape.shapename
        <<" | Length : "<<shape.length
        <<" | Width : "<<shape.width
        <<" | Area : "<<*shape.ptr_area;
        return os;
    }

    friend void fun();

};

void fun()
{
    shapes shape;
    shape.start_fun();

    Calculator calc(3, 4);

    //template <typename T> adresult = calc.add(3, 4);
    auto addresult = calc.add(3, 4);
    cout<<"Template ADD : "<<addresult;


}

int main()
{

    fun();

}