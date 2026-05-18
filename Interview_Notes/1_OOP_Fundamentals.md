# C++ OOP Fundamentals - Interview Notes

## Table of Contents
1. [Abstract Classes](#abstract-classes)
2. [Virtual Functions](#virtual-functions)
3. [VTable and VPtr](#vtable-and-vptr)
4. [Polymorphism - Pointer Initialization](#polymorphism-pointer-initialization)
5. [This Pointer](#this-pointer)
6. [Virtual Destructors](#virtual-destructors)

---

## Abstract Classes

### **Q: What is an Abstract Class?**

**Answer:**
An abstract class is a class that contains **at least one pure virtual function** (declared with `= 0`). It:
- **Cannot be instantiated** directly
- Serves as a **base class** for derived classes
- Forces derived classes to implement pure virtual functions
- Defines an **interface/contract** for derived classes

### **Pure Virtual Function:**
```cpp
virtual void functionName() = 0;  // Pure virtual function
```

### **Key Rules:**
1. **Cannot create objects** of abstract class
2. **Can have pointers/references** to abstract class
3. Abstract class **can have concrete functions** (non-pure virtual)
4. Derived class must **implement all pure virtual functions** to become concrete

### **Example:**

```cpp
#include <iostream>
using namespace std;

// Abstract class
class Shape {
public:
    // Pure virtual functions
    virtual void draw() = 0;
    virtual double area() = 0;
    
    // Concrete function (allowed in abstract class)
    void display() {
        cout << "Shape information\n";
    }
    
    virtual ~Shape() {}  // Virtual destructor
};

// Concrete derived class
class Circle : public Shape {
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    // Must implement all pure virtual functions
    void draw() override {
        cout << "Drawing Circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
};

int main() {
    // ❌ Cannot instantiate abstract class
    // Shape shape;  // ERROR: Cannot create object of abstract class
    
    // ✅ Can use pointers to abstract class
    Shape* ptr;
    
    Circle circle(5.0);
    ptr = &circle;
    ptr->draw();
    cout << "Area: " << ptr->area() << "\n";
    
    Rectangle rect(4.0, 6.0);
    ptr = &rect;
    ptr->draw();
    cout << "Area: " << ptr->area() << "\n";
    
    return 0;
}

/* Output:
Drawing Circle
Area: 78.5398
Drawing Rectangle
Area: 24
*/
```

### **Interview Points:**
- Abstract classes enable **runtime polymorphism**
- Used to define **common interface** for related classes
- **Design pattern**: Template Method, Strategy, Factory patterns use abstract classes
- Abstract class **can still be abstract** even after implementing pure virtual functions from another class

---

## Virtual Functions

### **Q: What are Virtual Functions and Why Do We Need Them?**

**Answer:**
Virtual functions enable **runtime polymorphism** (dynamic binding). They allow a function call to be resolved at **runtime** based on the **actual object type**, not the pointer type.

### **How It Works:**

**Without virtual:**
```cpp
class Base {
public:
    void print() { cout << "Base\n"; }  // Non-virtual
};

class Derived : public Base {
public:
    void print() { cout << "Derived\n"; }
};

int main() {
    Base* ptr = new Derived();
    ptr->print();  // Calls Base::print() - STATIC BINDING
    delete ptr;
}
// Output: Base  (Not what we wanted!)
```

**With virtual:**
```cpp
class Base {
public:
    virtual void print() { cout << "Base\n"; }  // Virtual
};

class Derived : public Base {
public:
    void print() override { cout << "Derived\n"; }
};

int main() {
    Base* ptr = new Derived();
    ptr->print();  // Calls Derived::print() - DYNAMIC BINDING
    delete ptr;
}
// Output: Derived  (Correct!)
```

### **Theory:**
- **Static binding** (compile-time): Function call resolved based on **pointer type**
- **Dynamic binding** (runtime): Function call resolved based on **object type** using **vtable**
- Virtual keyword creates **vtable** (virtual table) for the class
- Each object gets a **vptr** (virtual pointer) pointing to its vtable

---

## VTable and VPtr

### **Q: What is VTable and VPtr? How Does Virtual Function Mechanism Work Internally?**

**Answer:**
VTable (Virtual Table) and VPtr (Virtual Pointer) are C++'s implementation of runtime polymorphism.

### **Theory:**

**VTable (Virtual Table):**
- **Per-class** static array of function pointers
- Contains addresses of all virtual functions for that class
- Created at **compile-time**
- Shared by all objects of the class

**VPtr (Virtual Pointer):**
- **Per-object** hidden pointer
- Points to the vtable of the object's class
- Added automatically by compiler (8 bytes on 64-bit)
- Initialized in constructor

### **Memory Layout:**

```cpp
class Base {
    int x;                    // 4 bytes
    virtual void func() {}    // Creates vtable, adds vptr
};

// Memory layout of Base object:
// [vptr: 8 bytes][x: 4 bytes][padding: 4 bytes]
// Total: 16 bytes
//
// vptr points to Base's vtable:
// ┌──────────────────┐
// │ Base's VTable    │
// ├──────────────────┤
// │ &Base::func      │ ← Address of Base::func()
// └──────────────────┘
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void sound() {
        cout << "Animal sound\n";
    }
    
    virtual void move() {
        cout << "Animal moves\n";
    }
};

class Dog : public Animal {
public:
    void sound() override {
        cout << "Woof!\n";
    }
    
    void move() override {
        cout << "Dog runs\n";
    }
};

class Cat : public Animal {
public:
    void sound() override {
        cout << "Meow!\n";
    }
    
    void move() override {
        cout << "Cat jumps\n";
    }
};

int main() {
    Animal* ptr;
    
    Dog dog;
    ptr = &dog;
    ptr->sound();  // Calls Dog::sound()
    ptr->move();   // Calls Dog::move()
    
    Cat cat;
    ptr = &cat;
    ptr->sound();  // Calls Cat::sound()
    ptr->move();   // Calls Cat::move()
    
    return 0;
}

/* Output:
Woof!
Dog runs
Meow!
Cat jumps
*/
```

### **VTable Layout:**

```
Animal object (Dog):
┌────────────┐
│ vptr       │ ──→ Dog's VTable:
├────────────┤     ┌──────────────────┐
│ data...    │     │ &Dog::sound()    │
└────────────┘     │ &Dog::move()     │
                   └──────────────────┘

Animal object (Cat):
┌────────────┐
│ vptr       │ ──→ Cat's VTable:
├────────────┤     ┌──────────────────┐
│ data...    │     │ &Cat::sound()    │
└────────────┘     │ &Cat::move()     │
                   └──────────────────┘
```

### **Function Call Mechanism:**

```cpp
ptr->sound();

// Steps:
// 1. Dereference ptr to get object
// 2. Read vptr from object
// 3. Access vtable using vptr
// 4. Get function pointer from vtable (sound's slot)
// 5. Call the function

// Assembly (simplified):
// mov rax, [ptr]         ; Get object address
// mov rax, [rax]         ; Get vptr
// mov rax, [rax]         ; Get function pointer from vtable[0]
// call rax               ; Call the function
```

### **When VTable is Created:**

| Scenario | VTable Created? |
|----------|----------------|
| Class with **no virtual functions** | ❌ NO |
| Class with **≥1 virtual function** | ✅ YES |
| Class with **inherited virtual** | ✅ YES |
| Class with **pure virtual function** | ✅ YES (still needs vtable) |

### **Size Impact:**

```cpp
class NoVirtual {
    int x;  // 4 bytes
};
// sizeof(NoVirtual) = 4 bytes

class WithVirtual {
    int x;                    // 4 bytes
    virtual void func() {}    // Adds vptr (8 bytes on 64-bit)
};
// sizeof(WithVirtual) = 16 bytes (4 + 8 + 4 padding)
```

### **Interview Points:**
- VTable created **once per class** at **compile-time**
- VPtr added to **every object** at **runtime** (in constructor)
- VTable enables **O(1) virtual function call** (just pointer dereference)
- Adding more virtual functions **doesn't increase object size** (only vtable size)
- Destroying object with base pointer requires **virtual destructor** to find correct destructor in vtable

---

## Polymorphism - Pointer Initialization

### **Q: What is Pointer Initialization in Polymorphism? How Does Runtime Polymorphism Work?**

**Answer:**
Pointer initialization for polymorphism means using a **base class pointer** to point to **derived class objects**, enabling runtime polymorphic behavior through virtual functions.

### **Core Concept:**

```cpp
Base* ptr = new Derived();  // Base pointer → Derived object
ptr->virtualFunction();      // Calls Derived's version (polymorphism)
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

// Base class
class Animal {
public:
    virtual void sound() {
        cout << "Generic animal sound\n";
    }
    
    virtual void info() {
        cout << "I am an animal\n";
    }
    
    virtual ~Animal() {
        cout << "Animal destructor\n";
    }
};

// Derived classes
class Dog : public Animal {
public:
    void sound() override {
        cout << "Woof! Woof!\n";
    }
    
    void info() override {
        cout << "I am a dog\n";
    }
    
    ~Dog() override {
        cout << "Dog destructor\n";
    }
};

class Cat : public Animal {
public:
    void sound() override {
        cout << "Meow! Meow!\n";
    }
    
    void info() override {
        cout << "I am a cat\n";
    }
    
    ~Cat() override {
        cout << "Cat destructor\n";
    }
};

// Function demonstrating polymorphism
void makeSound(Animal* animal) {
    animal->sound();  // Calls appropriate derived class version
    animal->info();
}

int main() {
    cout << "=== Polymorphic Pointer Initialization ===\n\n";
    
    // 1. Base pointer → Dog object
    Animal* ptr1 = new Dog();
    ptr1->sound();  // Calls Dog::sound()
    ptr1->info();   // Calls Dog::info()
    delete ptr1;
    
    cout << "\n";
    
    // 2. Base pointer → Cat object
    Animal* ptr2 = new Cat();
    ptr2->sound();  // Calls Cat::sound()
    ptr2->info();   // Calls Cat::info()
    delete ptr2;
    
    cout << "\n=== Array of Base Pointers ===\n\n";
    
    // 3. Array of base pointers to different derived objects
    Animal* animals[3];
    animals[0] = new Dog();
    animals[1] = new Cat();
    animals[2] = new Dog();
    
    for(int i = 0; i < 3; i++) {
        cout << "Animal " << i << ":\n";
        animals[i]->sound();
        animals[i]->info();
        cout << "\n";
    }
    
    // Cleanup
    for(int i = 0; i < 3; i++) {
        delete animals[i];
    }
    
    cout << "\n=== Using Reference ===\n\n";
    
    // 4. Using reference (stack objects)
    Dog dog;
    Cat cat;
    
    makeSound(&dog);
    cout << "\n";
    makeSound(&cat);
    
    return 0;
}

/* Output:
=== Polymorphic Pointer Initialization ===

Woof! Woof!
I am a dog
Dog destructor
Animal destructor

Meow! Meow!
I am a cat
Cat destructor
Animal destructor

=== Array of Base Pointers ===

Animal 0:
Woof! Woof!
I am a dog

Animal 1:
Meow! Meow!
I am a cat

Animal 2:
Woof! Woof!
I am a dog

Dog destructor
Animal destructor
Cat destructor
Animal destructor
Dog destructor
Animal destructor

=== Using Reference ===

Woof! Woof!
I am a dog

Meow! Meow!
I am a cat
Dog destructor
Animal destructor
Cat destructor
Animal destructor
*/
```

### **Different Ways to Initialize Polymorphic Pointers:**

```cpp
// 1. Direct initialization with new
Base* ptr1 = new Derived();

// 2. Assignment after declaration
Base* ptr2;
ptr2 = new Derived();

// 3. Using stack objects (reference/pointer)
Derived obj;
Base* ptr3 = &obj;
Base& ref = obj;

// 4. Array of base pointers
Base* arr[3] = {new Derived1(), new Derived2(), new Derived3()};

// 5. Vector of base pointers
vector<Base*> vec;
vec.push_back(new Derived1());
vec.push_back(new Derived2());

// 6. Smart pointers (Modern C++)
unique_ptr<Base> ptr4 = make_unique<Derived>();
shared_ptr<Base> ptr5 = make_shared<Derived>();
```

### **Rules for Polymorphic Pointers:**

| Rule | Explanation |
|------|-------------|
| **Virtual Functions Required** | Base class must have `virtual` keyword |
| **Base Pointer Type** | Pointer/reference type is base class |
| **Derived Object** | Actual object is derived class |
| **Virtual Destructor Required** | Base destructor must be virtual for proper cleanup |
| **Override in Derived** | Derived class overrides virtual function |

### **What You CANNOT Do:**

```cpp
// ❌ Cannot instantiate abstract class
Base obj;  // ERROR if Base is abstract

// ❌ Cannot delete through base pointer without virtual destructor
// (Causes memory leak)
```

### **What You CAN Do:**

```cpp
// ✅ Pointer to abstract class
Base* ptr = &derivedObj;

// ✅ Reference to abstract class
Base& ref = derivedObj;

// ✅ Array/vector of base pointers
vector<Base*> objects;

// ✅ Pass by reference/pointer
void func(Base& obj);
void func(Base* obj);
```

### **Interview Points:**
- Polymorphic pointers enable **writing generic code** that works with all derived types
- The **same pointer** calls **different implementations** based on actual object type
- Decision of which function to call is made at **runtime** (dynamic binding)
- Requires **virtual functions** in base class and **override** in derived classes
- Must have **virtual destructor** to avoid memory leaks

---

## This Pointer

### **Q: What is `this` Pointer in C++?**

**Answer:**
`this` is an **implicit pointer** available inside **non-static member functions**. It points to the **object** for which the member function is called.

### **Theory:**

- **Type**: `ClassName* const` (constant pointer to object)
- **Availability**: Only in **non-static member functions**
- **Implicitly passed**: Compiler adds `this` as hidden first parameter
- **Cannot be modified**: `this` itself cannot point to different object
- **Can modify object**: Can modify data through `this` (unless function is `const`)

### **Basic Usage:**

```cpp
#include <iostream>
using namespace std;

class MyClass {
    int value;
    
public:
    MyClass(int value) {
        // this->value disambiguates from parameter
        this->value = value;
    }
    
    void setValue(int value) {
        this->value = value;  // Member variable = Parameter
    }
    
    int getValue() {
        return this->value;  // Can use this-> explicitly
        // return value;     // Or omit this-> (implicit)
    }
    
    void printAddress() {
        cout << "Object address: " << this << "\n";
        cout << "Member address: " << &(this->value) << "\n";
    }
    
    // Return *this for method chaining
    MyClass& increment() {
        this->value++;
        return *this;  // Return reference to current object
    }
};

int main() {
    MyClass obj(10);
    cout << "Value: " << obj.getValue() << "\n";
    obj.printAddress();
    
    // Method chaining using this
    obj.increment().increment().increment();
    cout << "After chaining: " << obj.getValue() << "\n";
    
    return 0;
}

/* Output:
Value: 10
Object address: 0x7ffd1234abcd
Member address: 0x7ffd1234abcd
After chaining: 13
*/
```

### **When to Use `this`:**

#### **1. Parameter Name Same as Member Variable:**

```cpp
class Person {
    string name;
    int age;
    
public:
    Person(string name, int age) {
        // this-> required to disambiguate
        this->name = name;
        this->age = age;
    }
};
```

#### **2. Return Current Object (Method Chaining):**

```cpp
class Builder {
    int x, y, z;
    
public:
    Builder& setX(int x) {
        this->x = x;
        return *this;  // Return reference to current object
    }
    
    Builder& setY(int y) {
        this->y = y;
        return *this;
    }
    
    Builder& setZ(int z) {
        this->z = z;
        return *this;
    }
};

int main() {
    Builder b;
    b.setX(10).setY(20).setZ(30);  // Method chaining
}
```

#### **3. Pass Current Object to Another Function:**

```cpp
class Node {
    int data;
    
public:
    void processNode(NodeProcessor& processor) {
        processor.process(this);  // Pass this node to processor
    }
};
```

#### **4. Compare Objects:**

```cpp
class MyClass {
    int id;
    
public:
    bool isEqual(const MyClass& other) {
        return this == &other;  // Same object in memory?
    }
    
    MyClass& operator=(const MyClass& other) {
        if (this != &other) {  // Prevent self-assignment
            id = other.id;
        }
        return *this;
    }
};
```

### **How Compiler Uses `this`:**

**Your code:**
```cpp
class MyClass {
    int x;
public:
    void setX(int value) {
        x = value;
    }
};

MyClass obj;
obj.setX(10);
```

**What compiler actually does:**
```cpp
// Compiler transforms member function to:
void setX(MyClass* const this, int value) {
    this->x = value;
}

// And transforms call to:
MyClass obj;
setX(&obj, 10);  // this = &obj
```

### **`this` in Different Contexts:**

```cpp
class Example {
    int data;
    
public:
    // 1. Regular member function
    void func1() {
        // this is: Example* const
        this->data = 10;
    }
    
    // 2. Const member function
    void func2() const {
        // this is: const Example* const
        // Cannot modify data through this
        // this->data = 10;  // ERROR
        cout << this->data;  // OK - can read
    }
    
    // 3. Static member function
    static void func3() {
        // ERROR: 'this' is not available in static functions
        // this->data = 10;  // ERROR
    }
};
```

### **Complete Example - Real World:**

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
    string accountNumber;
    string ownerName;
    double balance;
    
public:
    BankAccount(string accountNumber, string ownerName, double initialBalance)
        : accountNumber(accountNumber), ownerName(ownerName), balance(initialBalance) {
        cout << "Account created at address: " << this << "\n";
    }
    
    // Method chaining
    BankAccount& deposit(double amount) {
        this->balance += amount;
        cout << "Deposited $" << amount << " to account at " << this << "\n";
        return *this;
    }
    
    BankAccount& withdraw(double amount) {
        this->balance -= amount;
        cout << "Withdrew $" << amount << " from account at " << this << "\n";
        return *this;
    }
    
    // Comparison using this
    bool isSameAccount(const BankAccount& other) const {
        return this == &other;
    }
    
    // Self-assignment check
    BankAccount& operator=(const BankAccount& other) {
        if (this != &other) {  // Prevent self-assignment
            accountNumber = other.accountNumber;
            ownerName = other.ownerName;
            balance = other.balance;
        }
        return *this;
    }
    
    void display() const {
        cout << "Account: " << accountNumber 
             << ", Owner: " << ownerName 
             << ", Balance: $" << balance << "\n";
    }
};

int main() {
    BankAccount acc1("ACC001", "Alice", 1000.0);
    
    // Method chaining using this
    acc1.deposit(500).withdraw(200).deposit(100);
    acc1.display();
    
    BankAccount acc2("ACC002", "Bob", 2000.0);
    
    // Comparison
    cout << "Same account? " << boolalpha 
         << acc1.isSameAccount(acc2) << "\n";
    cout << "Same account? " << acc1.isSameAccount(acc1) << "\n";
    
    return 0;
}

/* Output:
Account created at address: 0x7ffd1234abc0
Deposited $500 to account at 0x7ffd1234abc0
Withdrew $200 from account at 0x7ffd1234abc0
Deposited $100 from account at 0x7ffd1234abc0
Account: ACC001, Owner: Alice, Balance: $1400
Account created at address: 0x7ffd1234abd0
Same account? false
Same account? true
*/
```

### **Interview Points:**
- `this` is a **constant pointer** to the object (you can't change where it points)
- Available only in **non-static member functions**
- Implicitly passed by compiler as **hidden first parameter**
- Used for **disambiguation**, **method chaining**, and **self-reference**
- Type is `ClassName* const` (non-const function) or `const ClassName* const` (const function)

---

## Virtual Destructors

### **Q: Why Do We Need Virtual Destructors?**

**Answer:**
Virtual destructors ensure **proper cleanup** when deleting derived class objects through base class pointers. Without virtual destructor, only the base class destructor is called, causing **resource leaks** and **undefined behavior**.

### **The Problem:**

**Without virtual destructor:**
```cpp
class Base {
public:
    ~Base() {  // NON-virtual destructor
        cout << "Base destructor\n";
    }
};

class Derived : public Base {
    int* data;
    
public:
    Derived() {
        data = new int[1000];
        cout << "Derived constructor\n";
    }
    
    ~Derived() {
        delete[] data;
        cout << "Derived destructor\n";
    }
};

int main() {
    Base* ptr = new Derived();
    delete ptr;  // ❌ Only Base destructor called!
                 // Derived destructor NEVER called
                 // Memory leak: data[] not freed
}

/* Output:
Derived constructor
Base destructor         ← Only base destructor!
                        ← Derived destructor NOT called!
                        ← Memory leak!
*/
```

### **The Solution:**

**With virtual destructor:**
```cpp
class Base {
public:
    virtual ~Base() {  // VIRTUAL destructor
        cout << "Base destructor\n";
    }
};

class Derived : public Base {
    int* data;
    
public:
    Derived() {
        data = new int[1000];
        cout << "Derived constructor\n";
    }
    
    ~Derived() override {
        delete[] data;
        cout << "Derived destructor\n";
    }
};

int main() {
    Base* ptr = new Derived();
    delete ptr;  // ✅ Correct order: Derived → Base
}

/* Output:
Derived constructor
Derived destructor      ← Derived destructor called first!
Base destructor         ← Base destructor called second!
                        ← Proper cleanup!
*/
```

### **Theory - Why This Happens:**

**Destructor Chaining Mechanism:**

1. **Without virtual**: Compiler uses **static binding** (based on pointer type)
   - Sees: `Base* ptr`
   - Calls: `Base::~Base()` only
   - Result: Derived resources leak

2. **With virtual**: Compiler uses **dynamic binding** (based on object type via vtable)
   - Sees: `Base* ptr` pointing to `Derived` object
   - Vtable lookup finds: `Derived::~Derived()`
   - Calls: `Derived::~Derived()` → automatically calls `Base::~Base()`
   - Result: Proper cleanup

### **Memory Leak Scenario:**

```cpp
class Resource {
    int* data;
    FILE* file;
    
public:
    Resource() {
        data = new int[1000000];  // 4 MB allocation
        file = fopen("data.txt", "r");
        cout << "Resource allocated\n";
    }
    
    ~Resource() {
        delete[] data;   // Free memory
        fclose(file);     // Close file
        cout << "Resource freed\n";
    }
};

class Base {
public:
    ~Base() {  // Non-virtual
        cout << "Base cleanup\n";
    }
};

class Derived : public Base {
    Resource* resource;
    
public:
    Derived() {
        resource = new Resource();
    }
    
    ~Derived() {
        delete resource;  // Never called without virtual destructor!
        cout << "Derived cleanup\n";
    }
};

int main() {
    Base* ptr = new Derived();
    
    // Creates Resource (4 MB + file handle)
    
    delete ptr;  // Without virtual: Resource NEVER freed!
                 // Memory leak: 4 MB
                 // File handle leak
}
```

### **VTable Mechanism:**

**How virtual destructor works:**

```
Base* ptr = new Derived();

ptr's vtable (Derived's vtable):
┌──────────────────────┐
│ Derived's VTable     │
├──────────────────────┤
│ &Derived::~Derived() │ ← Destructor entry
└──────────────────────┘

delete ptr:
1. Compiler: Look up destructor in vtable
2. Find: &Derived::~Derived()
3. Call: Derived::~Derived()
   └─→ Automatically calls Base::~Base() (destructor chaining)
4. Result: Complete cleanup
```

### **Design Guidelines:**

#### **When Virtual Destructor is REQUIRED:**

| Scenario | Virtual Destructor? | Why |
|----------|---------------------|-----|
| **Polymorphic base class** | ✅ MANDATORY | Objects deleted via base pointer |
| **Has virtual functions** | ✅ MANDATORY | Already polymorphic |
| **Will be inherited** | ✅ RECOMMENDED | Future-proofing |
| **Abstract class** | ✅ MANDATORY | Interface for derived classes |
| **Never inherited** | ❌ Not needed | No polymorphism |

#### **Two Design Patterns:**

**1. Public + Virtual (Polymorphic Use):**
```cpp
class Base {
public:
    virtual ~Base() {}  // Public virtual destructor
};

class Derived : public Base {
public:
    ~Derived() override {}
};

// Usage:
Base* ptr = new Derived();
delete ptr;  // ✅ OK - goes through base pointer
```

**2. Protected + Non-virtual (Prevent Polymorphic Deletion):**
```cpp
class Base {
protected:
    ~Base() {}  // Protected non-virtual destructor
};

class Derived : public Base {
public:
    ~Derived() {}
};

// Usage:
Derived* ptr = new Derived();
delete ptr;  // ✅ OK - delete through derived pointer

Base* basePtr = new Derived();
// delete basePtr;  // ❌ Compile error - destructor is protected
```

### **Complete Real-World Example:**

```cpp
#include <iostream>
#include <string>
using namespace std;

class Logger {
public:
    virtual ~Logger() {
        cout << "[Logger] Base cleanup\n";
    }
    
    virtual void log(const string& message) = 0;
};

class FileLogger : public Logger {
    FILE* file;
    
public:
    FileLogger(const string& filename) {
        file = fopen(filename.c_str(), "w");
        cout << "[FileLogger] File opened\n";
    }
    
    ~FileLogger() override {
        if (file) {
            fclose(file);
            cout << "[FileLogger] File closed\n";
        }
    }
    
    void log(const string& message) override {
        if (file) {
            fprintf(file, "%s\n", message.c_str());
        }
    }
};

class DatabaseLogger : public Logger {
    int* connection;
    
public:
    DatabaseLogger() {
        connection = new int(42);  // Simulate DB connection
        cout << "[DatabaseLogger] Database connected\n";
    }
    
    ~DatabaseLogger() override {
        delete connection;
        cout << "[DatabaseLogger] Database disconnected\n";
    }
    
    void log(const string& message) override {
        cout << "[DB] " << message << "\n";
    }
};

void processLogs(Logger* logger) {
    logger->log("Processing data...");
    logger->log("Operation complete");
    delete logger;  // Calls correct destructor via vtable
}

int main() {
    cout << "=== FileLogger ===\n";
    Logger* logger1 = new FileLogger("app.log");
    processLogs(logger1);
    
    cout << "\n=== DatabaseLogger ===\n";
    Logger* logger2 = new DatabaseLogger();
    processLogs(logger2);
    
    return 0;
}

/* Output:
=== FileLogger ===
[FileLogger] File opened
[DB] Processing data...
[DB] Operation complete
[FileLogger] File closed
[Logger] Base cleanup

=== DatabaseLogger ===
[DatabaseLogger] Database connected
[DB] Processing data...
[DB] Operation complete
[DatabaseLogger] Database disconnected
[Logger] Base cleanup
*/
```

### **Performance Cost:**

**Overhead of Virtual Destructor:**
- **Per-class**: One vtable entry (destructor pointer)
- **Per-object**: 8 bytes vptr (if no other virtual functions exist)
- **Runtime**: One vtable lookup during deletion

**Trade-off:**
- Small cost vs. **correctness and safety**
- In polymorphic classes (with virtual functions), vtable already exists
- Virtual destructor adds **negligible overhead**

### **Interview Points:**
- Virtual destructor ensures **correct destruction order**: Derived → Base
- Required for **any polymorphic base class** (with virtual functions)
- Prevents **resource leaks** (memory, file handles, database connections)
- Uses **vtable mechanism** for dynamic dispatch
- C++ Core Guidelines: "A base class destructor should be either public and virtual, or protected and non-virtual"
- Destructors are **implicitly noexcept** - should never throw exceptions

---

## Summary - Quick Reference

### **Abstract Classes:**
- Cannot instantiate
- ≥1 pure virtual function (`= 0`)
- Used as interface/base class
- Can have concrete functions

### **Virtual Functions:**
- Enable runtime polymorphism
- Resolved via vtable/vptr
- Use `override` in derived classes
- Dynamic binding at runtime

### **VTable/VPtr:**
- VTable: Per-class array of function pointers
- VPtr: Per-object pointer to vtable
- Created at compile-time (vtable)
- Initialized at runtime (vptr in constructor)
- Enables O(1) virtual function calls

### **Polymorphic Pointers:**
- Base pointer → Derived object
- Correct function called via vtable
- Requires virtual functions
- Must have virtual destructor

### **This Pointer:**
- Implicit pointer to current object
- Type: `ClassName* const`
- Used for disambiguation, chaining
- Not available in static functions

### **Virtual Destructors:**
- Ensures proper cleanup
- Required for polymorphic deletion
- Uses vtable for correct destructor
- Prevents resource leaks
- Implicitly noexcept

---

**Next Files:**
- [Memory Management](2_Memory_Management.md)
- [Modern C++ Keywords](3_Modern_Cpp_Keywords.md)
- [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md)
- [STL Internals](5_STL_Internals.md)
- [Performance Optimization](6_Performance_Optimization.md)
