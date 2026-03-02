# C++ Memory Management - Interview Notes

## Table of Contents
1. [Copy Constructor Behavior](#copy-constructor-behavior)
2. [Rule of Zero, Three, and Five](#rule-of-zero-three-and-five)
3. [Member Initializer List](#member-initializer-list)
4. [Delete Operator](#delete-operator)

---

## Copy Constructor Behavior

### **Q: How Does Copy Constructor Behave with Different Types (vector, array, shared_ptr, unique_ptr)?**

**Answer:**
Copy constructor behavior varies significantly based on the member type. Understanding this is crucial for proper resource management.

### **Quick Summary:**

| Member Type | Default Copy | Behavior | Manual Implementation? |
|-------------|--------------|----------|----------------------|
| **`vector`** | ✅ Works | Deep copy automatically | ❌ No |
| **Raw pointer (`int*`)** | ⚠️ Shallow | Copies pointer only | ✅ YES |
| **`shared_ptr`** | ✅ Works | Shares ownership (ref count++) | ❌ No (for sharing) |
| **`unique_ptr`** | ❌ Deleted | Cannot copy | ✅ YES (move or clone) |

---

### **1. VECTOR - Automatic Deep Copy** ✅

```cpp
#include <iostream>
#include <vector>
using namespace std;

class MyClass {
public:
    vector<int> data;
    
    MyClass(const vector<int>& v) : data(v) {}
    
    // DEFAULT copy constructor works perfectly!
    // Compiler generates: MyClass(const MyClass& other) : data(other.data) {}
};

int main() {
    MyClass obj1({1, 2, 3, 4, 5});
    MyClass obj2 = obj1;  // Uses DEFAULT copy constructor
    
    // DEEP COPY happened automatically
    obj2.data[0] = 999;
    
    cout << "obj1.data[0]: " << obj1.data[0] << "\n";  // 1
    cout << "obj2.data[0]: " << obj2.data[0] << "\n";  // 999
    
    return 0;
}
```

**Theory:** `std::vector` manages dynamic memory internally and provides its own copy constructor that performs deep copy. No manual implementation needed!

---

### **2. RAW POINTER - Manual Deep Copy Required** ⚠️

```cpp
#include <iostream>
#include <cstring>
using namespace std;

class MyClass {
    int* arr;
    int size;
    
public:
    MyClass(int sz) : size(sz) {
        arr = new int[size];
        for(int i = 0; i < size; i++) arr[i] = i;
    }
    
    // ❌ DEFAULT copy constructor (SHALLOW COPY - BAD!)
    // MyClass(const MyClass& other) {
    //     arr = other.arr;      // Just copies pointer!
    //     size = other.size;
    // }
    
    // ✅ CUSTOM copy constructor (DEEP COPY - GOOD!)
    MyClass(const MyClass& other) : size(other.size) {
        arr = new int[size];  // Allocate NEW memory
        memcpy(arr, other.arr, size * sizeof(int));
        cout << "Deep copy performed\n";
    }
    
    void modify(int idx, int val) { arr[idx] = val; }
    
    void print() {
        for(int i = 0; i < size; i++) cout << arr[i] << " ";
        cout << "\n";
    }
    
    ~MyClass() { delete[] arr; }
};

int main() {
    MyClass obj1(5);
    obj1.print();
    
    MyClass obj2 = obj1;  // Calls custom copy constructor
    obj2.modify(0, 999);
    
    cout << "obj1: "; obj1.print();  // 0 1 2 3 4
    cout << "obj2: "; obj2.print();  // 999 1 2 3 4
    
    return 0;
}
```

**Theory:** Raw pointers require manual deep copy. Default copy constructor only copies pointer value (same memory address), leading to double-delete and corruption.

---

### **3. SHARED_PTR - Shares Ownership** ✅

```cpp
#include <iostream>
#include <memory>
using namespace std;

class MyClass {
public:
    shared_ptr<int> ptr;
    
    MyClass(int val) {
        ptr = make_shared<int>(val);
        cout << "Created, ref count: " << ptr.use_count() << "\n";
    }
    
    // DEFAULT copy constructor works!
    // Shares same memory, increments reference count
};

int main() {
    MyClass obj1(42);
    cout << "obj1 ref count: " << obj1.ptr.use_count() << "\n";
    
    MyClass obj2 = obj1;  // DEFAULT copy
    
    cout << "After copy:\n";
    cout << "obj1 ref count: " << obj1.ptr.use_count() << "\n";  // 2
    cout << "obj2 ref count: " << obj2.ptr.use_count() << "\n";  // 2
    
    // Both share same memory
    *obj2.ptr = 999;
    cout << "obj1 value: " << *obj1.ptr << "\n";  // 999
    cout << "obj2 value: " << *obj2.ptr << "\n";  // 999
    
    return 0;
}
```

**For Independent Copies:**
```cpp
class MyClass {
public:
    shared_ptr<int> ptr;
    
    MyClass(int val) : ptr(make_shared<int>(val)) {}
    
    // CUSTOM copy for deep copy
    MyClass(const MyClass& other) {
        ptr = make_shared<int>(*other.ptr);  // New shared_ptr
    }
};
```

**Theory:** `shared_ptr` default copy creates **shared ownership** (reference counting). Both objects point to same memory. For independent copies, manually create new `shared_ptr`.

---

### **4. UNIQUE_PTR - Copy Deleted** ❌

```cpp
#include <iostream>
#include <memory>
using namespace std;

class MyClass {
public:
    unique_ptr<int> ptr;
    
    MyClass(int val) : ptr(make_unique<int>(val)) {}
    
    // Copy constructor is DELETED by unique_ptr
    // MyClass(const MyClass&) = delete;  // Implicit
    
    // ✅ MOVE constructor (transfers ownership)
    MyClass(MyClass&& other) noexcept : ptr(move(other.ptr)) {
        cout << "Move constructor\n";
    }
    
    // ✅ CLONE pattern for copying
    MyClass clone() const {
        MyClass copy(0);
        if(ptr) {
            copy.ptr = make_unique<int>(*ptr);
        }
        return copy;
    }
};

int main() {
    MyClass obj1(42);
    
    // ❌ Won't compile
    // MyClass obj2 = obj1;  // ERROR: copy deleted
    
    // ✅ Use MOVE
    MyClass obj2 = move(obj1);  // obj1 loses ownership
    cout << "obj2 value: " << *obj2.ptr << "\n";
    
    // ✅ Use CLONE for copying
    MyClass obj3(100);
    MyClass obj4 = obj3.clone();
    
    *obj4.ptr = 999;
    cout << "obj3: " << *obj3.ptr << "\n";  // 100
    cout << "obj4: " << *obj4.ptr << "\n";  // 999
    
    return 0;
}
```

**Theory:** `unique_ptr` enforces **unique ownership**. Copy constructor is deleted. Use **move semantics** (transfer ownership) or **clone pattern** (deep copy).

---

## Rule of Zero, Three, and Five

### **Q: What are Rule of Zero, Rule of Three, and Rule of Five?**

**Answer:**
These rules define when you need to implement special member functions for proper resource management.

---

### **Rule of Three (C++98/03)**

**When:** Class manages resources manually (raw pointers, file handles)

**Must implement:**
1. Destructor
2. Copy constructor
3. Copy assignment operator

```cpp
class Rule3Example {
    int* data;
    int size;
    
public:
    Rule3Example(int sz) : size(sz) {
        data = new int[size];
    }
    
    // 1. Destructor
    ~Rule3Example() {
        delete[] data;
    }
    
    // 2. Copy constructor
    Rule3Example(const Rule3Example& other) : size(other.size) {
        data = new int[size];
        memcpy(data, other.data, size * sizeof(int));
    }
    
    // 3. Copy assignment
    Rule3Example& operator=(const Rule3Example& other) {
        if(this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            memcpy(data, other.data, size * sizeof(int));
        }
        return *this;
    }
};
```

---

### **Rule of Five (C++11+)**

**When:** Class manages resources AND wants move semantics

**Must implement:**
1. Destructor
2. Copy constructor
3. Copy assignment
4. Move constructor
5. Move assignment

```cpp
class Rule5Example {
    int* data;
    int size;
    
public:
    Rule5Example(int sz) : size(sz) {
        data = new int[size];
    }
    
    // 1. Destructor
    ~Rule5Example() {
        delete[] data;
    }
    
    // 2. Copy constructor
    Rule5Example(const Rule5Example& other) : size(other.size) {
        data = new int[size];
        memcpy(data, other.data, size * sizeof(int));
    }
    
    // 3. Copy assignment
    Rule5Example& operator=(const Rule5Example& other) {
        if(this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            memcpy(data, other.data, size * sizeof(int));
        }
        return *this;
    }
    
    // 4. Move constructor
    Rule5Example(Rule5Example&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // 5. Move assignment
    Rule5Example& operator=(Rule5Example&& other) noexcept {
        if(this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};
```

---

### **Rule of Zero (Modern C++)**

**When:** Use smart pointers/containers instead of raw resources

**Implement:** NOTHING - use defaults!

```cpp
#include <memory>
#include <vector>

class RuleZeroExample {
    unique_ptr<int> ptr;
    vector<int> data;
    shared_ptr<string> name;
    
public:
    RuleZeroExample(int val, string n) 
        : ptr(make_unique<int>(val)),
          data{1, 2, 3},
          name(make_shared<string>(n)) {}
    
    // No special members needed!
    // Compiler generates everything correctly
};
```

**Theory:** Smart pointers and containers handle resource management automatically. Following Rule of Zero leads to cleaner, safer code.

---

### **Decision Tree:**

```
Does class have raw pointers or manual resources?
├─ YES → Use Rule of 5 (or Rule of 3 for C++98)
│
└─ NO (only smart pointers/containers)
    └─ Use Rule of Zero (do nothing!)
```

---

## Member Initializer List

### **Q: What is Member Initializer List and Why Use It?**

**Answer:**
Member initializer list initializes members **before** constructor body executes. It's the **only way** to initialize const members, references, and base classes.

### **Syntax:**

```cpp
class MyClass {
    int x;
    const int y;
    int& ref;
    
public:
    // Constructor with initializer list
    MyClass(int a, int b, int& r) 
        : x(a),      // Initialize x
          y(b),      // Initialize const y
          ref(r) {   // Initialize reference ref
        // Constructor body
    }
};
```

### **Why Use Initializer List:**

#### **1. Required for Const Members:**

```cpp
class MyClass {
    const int value;
    
public:
    // ✅ Must use initializer list
    MyClass(int v) : value(v) {}
    
    // ❌ Cannot do this:
    // MyClass(int v) {
    //     value = v;  // ERROR: cannot assign to const
    // }
};
```

#### **2. Required for Reference Members:**

```cpp
class MyClass {
    int& ref;
    
public:
    // ✅ Must use initializer list
    MyClass(int& r) : ref(r) {}
    
    // ❌ Cannot initialize in body
    // MyClass(int& r) {
    //     ref = r;  // ERROR: reference must be initialized
    // }
};
```

#### **3. Required for Base Class Constructor:**

```cpp
class Base {
public:
    Base(int x) { cout << "Base: " << x << "\n"; }
};

class Derived : public Base {
public:
    // ✅ Call base constructor in initializer list
    Derived(int x) : Base(x) {}
    
    // ❌ Cannot call base constructor in body
};
```

#### **4. Performance - Avoids Double Initialization:**

```cpp
class MyClass {
    string name;
    vector<int> data;
    
public:
    // ✅ EFFICIENT - Direct initialization
    MyClass(string n, vector<int> d) : name(n), data(d) {}
    
    // ❌ INEFFICIENT - Default construction + assignment
    // MyClass(string n, vector<int> d) {
    //     name = n;     First: default construct, Second: assign
    //     data = d;     First: default construct, Second: assign
    // }
};
```

### **Initialization Order:**

**Members are initialized in DECLARATION order, not initializer list order!**

```cpp
class MyClass {
    int a;  // Initialized first
    int b;  // Initialized second
    int c;  // Initialized third
    
public:
    // ⚠️ Initialized in declaration order: a, b, c
    // NOT in initializer list order!
    MyClass() : c(3), a(1), b(2) {}
    //          ^^^^^  ^^^^  ^^^^
    //          3rd   1st   2nd  (actual order)
};
```

### **Complete Example:**

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
    const int accountNumber;  // Const member
    string& ownerName;        // Reference member
    double balance;
    
public:
    // Member initializer list
    BankAccount(int accNum, string& owner, double initial)
        : accountNumber(accNum),  // Must initialize const
          ownerName(owner),       // Must initialize reference
          balance(initial) {      // Efficient initialization
        cout << "Account created\n";
    }
    
    void display() const {
        cout << "Account #" << accountNumber 
             << ", Owner: " << ownerName 
             << ", Balance: $" << balance << "\n";
    }
};

int main() {
    string name = "Alice";
    BankAccount acc(12345, name, 1000.0);
    acc.display();
    
    return 0;
}
```

### **Interview Points:**
- **Required** for: const members, references, base class initialization
- **Performance**: Avoids double initialization (default + assignment)
- **Order**: Members initialized in **declaration order**, not list order
- Always use for non-trivial types (string, vector, etc.)

---

## Delete Operator

### **Q: What Does `delete` Do? How Does It Work?**

**Answer:**
`delete` deallocates memory allocated with `new`. It performs TWO operations:
1. Calls the **destructor** of the object
2. **Frees the memory** back to the heap

### **Internal Process:**

```
delete ptr;

Step 1: ptr->~ClassName()     // Call destructor
Step 2: operator delete(ptr)   // Free memory
Step 3: ptr = dangling         // ptr now points to freed memory
```

### **Basic Usage:**

#### **Single Object - `delete`:**

```cpp
MyClass* ptr = new MyClass(42);
delete ptr;       // Calls ~MyClass(), then frees memory
ptr = nullptr;    // Good practice
```

#### **Array of Objects - `delete[]`:**

```cpp
int* arr = new int[5]{1, 2, 3, 4, 5};
delete[] arr;     // MUST use delete[] for arrays!
arr = nullptr;
```

**⚠️ CRITICAL:** `new` → `delete`, `new[]` → `delete[]`. Mixing causes undefined behavior!

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class Resource {
    int* data;
    int id;
    
public:
    Resource(int i) : id(i) {
        data = new int[100];
        cout << "Resource " << id << " constructed\n";
    }
    
    ~Resource() {
        delete[] data;
        cout << "Resource " << id << " destructed\n";
    }
};

int main() {
    cout << "=== Creating object ===\n";
    Resource* ptr = new Resource(1);
    
    cout << "\n=== Using object ===\n";
    // ... use ptr ...
    
    cout << "\n=== Deleting object ===\n";
    delete ptr;  // 1. Calls ~Resource(), 2. Frees memory
    
    cout << "\n=== After delete ===\n";
    ptr = nullptr;  // Avoid dangling pointer
    
    return 0;
}

/* Output:
=== Creating object ===
Resource 1 constructed

=== Using object ===

=== Deleting object ===
Resource 1 destructed

=== After delete ===
*/
```

### **Common Mistakes:**

#### **1. Double Delete:**
```cpp
int* ptr = new int(10);
delete ptr;
delete ptr;  // ❌ CRASH! Double free
```

**Fix:**
```cpp
int* ptr = new int(10);
delete ptr;
ptr = nullptr;
delete ptr;  // ✅ Safe - delete nullptr is no-op
```

#### **2. Using `delete` on `new[]`:**
```cpp
int* arr = new int[5];
delete arr;  // ❌ WRONG! Memory leak
```

**Fix:**
```cpp
int* arr = new int[5];
delete[] arr;  // ✅ Correct
```

#### **3. Dangling Pointer:**
```cpp
int* ptr = new int(42);
delete ptr;
cout << *ptr;  // ❌ Undefined behavior
```

**Fix:**
```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;
if(ptr) cout << *ptr;  // Won't execute
```

### **`delete` vs `free()`:**

| Feature | `delete` (C++) | `free()` (C) |
|---------|---------------|--------------|
| **Calls destructor** | ✅ YES | ❌ NO |
| **Works with** | `new` | `malloc` |
| **Type-safe** | ✅ YES | ❌ NO |
| **Use in C++** | ✅ Preferred | ⚠️ Avoid |

### **Modern Alternative:**

```cpp
// Old way (manual delete)
MyClass* ptr = new MyClass();
delete ptr;

// Modern way (auto delete)
{
    unique_ptr<MyClass> ptr = make_unique<MyClass>();
    // No delete needed!
}
```

### **Interview Points:**
- `delete` calls destructor then frees memory
- `new` → `delete`, `new[]` → `delete[]`
- Set to `nullptr` after delete
- Cannot delete same pointer twice
- Modern C++: Use smart pointers instead

---

**Next Files:**
- [Modern C++ Keywords](3_Modern_Cpp_Keywords.md)
- [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md)
- [STL Internals](5_STL_Internals.md)
