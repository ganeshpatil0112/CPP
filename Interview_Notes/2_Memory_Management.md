# C++ Memory Management - Interview Notes

## Table of Contents
1. [Copy Constructor Behavior](#copy-constructor-behavior)
2. [Rule of Zero, Three, and Five](#rule-of-zero-three-and-five)
3. [Member Initializer List](#member-initializer-list)
4. [Delete Operator](#delete-operator)
5. [Dangling Pointers](#dangling-pointers)
6. [RAII Principle](#raii-principle)

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

- Base class must be fully constructed before derived members exist — the derived object literally cannot exist until its base sub-object is built. So the base ctor has to run first.
- The compiler needs to know which base ctor to run. If a default exists, it picks that silently; otherwise, you must say so. This is the same rule that applies to data members.
- While constructing a derived class object, the compiler can automatically call only the default constructor of the base class. If the base has no default constructor, or you want to call a parameterized one, the only way is through the member-initializer list.


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

## Dangling Pointers

### **Q: What is a Dangling Pointer?**

**Answer:**
A **dangling pointer** is a pointer that points to **memory that has already been deleted/freed** or is **no longer valid**. Accessing dangling pointers leads to **undefined behavior** - crashes, corrupted data, or security vulnerabilities.

### **Definition:**

```cpp
int* ptr = new int(42);  // ptr points to valid memory
delete ptr;              // Memory freed
// ptr is now DANGLING - points to freed memory
*ptr = 100;              // ❌ UNDEFINED BEHAVIOR
```

**Key Point:** After `delete`, the pointer **still holds the address**, but that address is **no longer valid**. The memory may be reused for other allocations.

---

### **Common Causes of Dangling Pointers:**

#### **1. Using Pointer After Delete:**

```cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = new int(42);
    cout << *ptr << "\n";  // ✅ OK: 42
    
    delete ptr;  // Memory freed
    
    // ❌ DANGLING POINTER - undefined behavior!
    cout << *ptr << "\n";  // May crash, print garbage, or "work"
    *ptr = 100;            // May corrupt memory
    
    return 0;
}
```

**What Happens:**
- Memory is returned to heap
- Pointer still holds old address
- That memory may be reused by other allocations
- Reading/writing causes undefined behavior

---

#### **2. Returning Pointer to Local Variable:**

```cpp
int* createDanglingPointer() {
    int x = 42;      // Local variable on stack
    return &x;       // ❌ DANGER: Returning address of local
}  // x destroyed here - stack memory reclaimed

int main() {
    int* ptr = createDanglingPointer();
    // ptr is DANGLING - points to destroyed stack variable
    cout << *ptr;  // ❌ Undefined behavior
    return 0;
}
```

**Why It's Dangerous:**
- Local variables are destroyed when function returns
- Stack memory is reused by next function call
- Pointer points to invalid stack location

**Correct Version:**
```cpp
int* createValidPointer() {
    int* ptr = new int(42);  // ✅ Heap allocation
    return ptr;              // OK - heap memory persists
}

int main() {
    int* ptr = createValidPointer();
    cout << *ptr << "\n";  // ✅ OK: 42
    delete ptr;             // Don't forget to delete!
    return 0;
}
```

---

#### **3. Object Goes Out of Scope:**

```cpp
#include <iostream>
using namespace std;

int* danglingPointer = nullptr;

void createDangling() {
    int x = 42;
    danglingPointer = &x;  // ❌ Storing address of local
}  // x destroyed - danglingPointer now dangling

int main() {
    createDangling();
    cout << *danglingPointer;  // ❌ Undefined behavior
    return 0;
}
```

---

#### **4. Multiple Pointers to Same Memory:**

```cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr1 = new int(42);
    int* ptr2 = ptr1;  // Both point to same memory
    
    delete ptr1;  // Memory freed
    // ptr1 is dangling
    // ptr2 is also dangling!
    
    cout << *ptr2;  // ❌ Undefined behavior
    delete ptr2;    // ❌ Double delete - crash!
    
    return 0;
}
```

---

#### **5. Returning Reference to Temporary:**

```cpp
string& getDanglingReference() {
    string temp = "Hello";  // Local temporary
    return temp;            // ❌ Returning reference to temporary
}  // temp destroyed

int main() {
    string& ref = getDanglingReference();
    cout << ref;  // ❌ Undefined behavior
    return 0;
}
```

---

### **Complete Example - All Cases:**

```cpp
#include <iostream>
using namespace std;

class MyClass {
public:
    int value;
    MyClass(int v) : value(v) {
        cout << "Constructor(" << value << ")\n";
    }
    ~MyClass() {
        cout << "Destructor(" << value << ")\n";
    }
};

int main() {
    cout << "=== Case 1: Delete then use ===\n";
    {
        MyClass* ptr = new MyClass(1);
        delete ptr;
        // ptr->value = 100;  // ❌ DANGLING - would crash
    }
    
    cout << "\n=== Case 2: Out of scope ===\n";
    {
        MyClass* ptr;
        {
            MyClass obj(2);  // Stack object
            ptr = &obj;
        }  // obj destroyed
        // ptr->value = 100;  // ❌ DANGLING - would crash
    }
    
    cout << "\n=== Case 3: Multiple pointers ===\n";
    {
        MyClass* ptr1 = new MyClass(3);
        MyClass* ptr2 = ptr1;
        delete ptr1;
        // Both ptr1 and ptr2 are now dangling
        // cout << ptr2->value;  // ❌ Would crash
    }
    
    return 0;
}
/* Output:
=== Case 1: Delete then use ===
Constructor(1)
Destructor(1)

=== Case 2: Out of scope ===
Constructor(2)
Destructor(2)

=== Case 3: Multiple pointers ===
Constructor(3)
Destructor(3)
*/
```

---

### **How to Prevent Dangling Pointers:**

#### **1. Set to nullptr After Delete:**

```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;  // ✅ No longer dangling

if (ptr) {
    *ptr = 100;  // Won't execute
}
```

**Why This Helps:**
- `nullptr` is a **safe value** - cannot be dereferenced
- Easy to check if pointer is valid
- Deleting `nullptr` is safe (does nothing)

---

#### **2. Use Smart Pointers:**

```cpp
#include <memory>
using namespace std;

int main() {
    // unique_ptr - automatic cleanup
    {
        unique_ptr<int> ptr = make_unique<int>(42);
        // No delete needed - automatic!
    }  // ptr destroyed, memory freed
    
    // shared_ptr - reference counting
    {
        shared_ptr<int> ptr1 = make_shared<int>(42);
        shared_ptr<int> ptr2 = ptr1;  // Share ownership
        // Memory freed when last shared_ptr destroyed
    }
    
    return 0;
}
```

**Benefits:**
- **Automatic cleanup** - no manual delete
- **Exception safe** - cleanup even if exception thrown
- **No dangling pointers** - memory managed correctly

---

#### **3. RAII (Resource Acquisition Is Initialization):**

```cpp
class ResourceHolder {
    int* data;
public:
    ResourceHolder() : data(new int(42)) {}
    
    ~ResourceHolder() {
        delete data;  // Automatic cleanup
        data = nullptr;
    }
    
    // Delete copy to prevent multiple owners
    ResourceHolder(const ResourceHolder&) = delete;
    ResourceHolder& operator=(const ResourceHolder&) = delete;
};

int main() {
    ResourceHolder holder;  // Automatic cleanup in destructor
    return 0;
}
```

---

#### **4. Never Return Address of Local:**

```cpp
// ❌ WRONG
int* bad() {
    int x = 42;
    return &x;  // Dangling!
}

// ✅ CORRECT Option 1: Return by value
int good1() {
    int x = 42;
    return x;  // Copy returned
}

// ✅ CORRECT Option 2: Heap allocation
int* good2() {
    return new int(42);  // Caller must delete
}

// ✅ CORRECT Option 3: Smart pointer
unique_ptr<int> good3() {
    return make_unique<int>(42);  // Auto cleanup
}
```

---

### **Detecting Dangling Pointers:**

#### **1. Valgrind (Linux/Mac):**
```bash
valgrind --leak-check=full ./program
# Detects:
# - Invalid reads/writes
# - Use after free
# - Memory leaks
```

#### **2. AddressSanitizer (Compiler Flag):**
```bash
g++ -fsanitize=address -g program.cpp -o program
./program
# Detects use-after-free at runtime
```

#### **3. Visual Studio Debug Mode:**
- Fills freed memory with special pattern (0xDD)
- Detects use-after-free in debug builds

---

### **Dangling Pointer vs Memory Leak:**

| Issue | Problem | Symptoms |
|-------|---------|----------|
| **Dangling Pointer** | Pointer to freed memory | Crashes, undefined behavior, corruption |
| **Memory Leak** | Memory never freed | Growing memory usage, out of memory |

```cpp
// Memory Leak - forgot to delete
int* leak() {
    int* ptr = new int(42);
    return nullptr;  // Memory leaked - never freed
}

// Dangling Pointer - used after delete
void dangling() {
    int* ptr = new int(42);
    delete ptr;
    *ptr = 100;  // Dangling pointer access
}
```

---

### **Real-World Example - Safe Wrapper:**

```cpp
#include <iostream>
using namespace std;

template<typename T>
class SafePointer {
    T* ptr;
    bool* valid;  // Shared validity flag
    
public:
    SafePointer(T* p) : ptr(p), valid(new bool(true)) {}
    
    ~SafePointer() {
        if (valid && *valid) {
            delete ptr;
            *valid = false;  // Mark as invalid
        }
        delete valid;
    }
    
    T& operator*() {
        if (!valid || !*valid) {
            throw runtime_error("Dangling pointer access!");
        }
        return *ptr;
    }
    
    bool isValid() const {
        return valid && *valid;
    }
};

int main() {
    SafePointer<int> sp(new int(42));
    cout << *sp << "\n";  // OK
    
    // sp destroyed, memory freed
    // Any copy would detect invalid access
    
    return 0;
}
```

---

### **Interview Points:**

**Definition:**
- Dangling pointer = pointer to freed/invalid memory
- Causes **undefined behavior** (crashes, corruption, security issues)

**Common Causes:**
1. Using pointer after `delete`
2. Returning address of local variable
3. Object goes out of scope
4. Multiple pointers to same memory (one deleted)
5. Container reallocation (vector, string)

**Prevention:**
1. Set to `nullptr` after delete
2. Use **smart pointers** (unique_ptr, shared_ptr)
3. Follow **RAII** principle
4. Never return address of local variables
5. Use modern C++ (avoid raw `new`/`delete`)

**Detection Tools:**
- Valgrind (Linux/Mac)
- AddressSanitizer (-fsanitize=address)
- Visual Studio debug mode

**Best Practice:**
```cpp
// ❌ Old style - prone to dangling pointers
int* ptr = new int(42);
delete ptr;

// ✅ Modern style - no dangling pointers
auto ptr = make_unique<int>(42);
// Automatic cleanup, no manual delete
```

---

## RAII Principle

### **Q: What is RAII (Resource Acquisition Is Initialization)?**

**Answer:**
RAII is a **C++ programming idiom** where **resource lifetime is bound to object lifetime**. Resources (memory, files, locks, sockets) are **acquired in constructor** and **released in destructor**. This ensures **automatic cleanup** and **exception safety**.

### **Core Principle:**

```
┌─────────────────────────────────────┐
│  Object Created                     │
│    ↓                                │
│  Constructor acquires resource      │
│    ↓                                │
│  Use resource                       │
│    ↓                                │
│  Object destroyed (out of scope)    │
│    ↓                                │
│  Destructor releases resource       │
└─────────────────────────────────────┘
```

**Key Insight:** You **never manually manage resources** - the destructor does it automatically when object goes out of scope.

---

### **Why RAII?**

#### **1. Automatic Cleanup:**
```cpp
// ❌ Manual Management - Error-prone
void processFile() {
    FILE* file = fopen("data.txt", "r");
    // ... processing ...
    fclose(file);  // Must remember to close!
    // What if exception thrown before fclose?
}

// ✅ RAII - Automatic cleanup
void processFile() {
    ifstream file("data.txt");  // Opens in constructor
    // ... processing ...
    // Automatically closed in destructor!
}
```

#### **2. Exception Safety:**
```cpp
// ❌ Without RAII - Resource leak on exception
void unsafeFunction() {
    int* data = new int[1000];
    
    processData(data);  // Might throw exception
    
    delete[] data;  // Never reached if exception thrown!
}

// ✅ With RAII - Guaranteed cleanup
void safeFunction() {
    vector<int> data(1000);  // RAII container
    
    processData(data.data());  // If exception thrown,
                               // destructor still called!
}
```

#### **3. No Manual Delete:**
```cpp
// ❌ Manual delete - easy to forget
void manual() {
    MyClass* obj = new MyClass();
    // ... code ...
    delete obj;  // Must remember!
}

// ✅ RAII - automatic
void automatic() {
    MyClass obj;  // Stack object
    // ... code ...
}  // Destructor called automatically
```

---

### **RAII Examples:**

#### **1. Memory Management - Smart Pointers:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Resource {
public:
    Resource() { cout << "Resource acquired\n"; }
    ~Resource() { cout << "Resource released\n"; }
    void use() { cout << "Using resource\n"; }
};

// ❌ Manual management
void manual() {
    Resource* res = new Resource();
    res->use();
    delete res;  // Must remember!
}

// ✅ RAII with unique_ptr
void raii() {
    unique_ptr<Resource> res = make_unique<Resource>();
    res->use();
}  // Automatic cleanup!

int main() {
    cout << "=== Manual ===\n";
    manual();
    
    cout << "\n=== RAII ===\n";
    raii();
    
    return 0;
}
/* Output:
=== Manual ===
Resource acquired
Using resource
Resource released

=== RAII ===
Resource acquired
Using resource
Resource released
*/
```

---

#### **2. File Handling:**

```cpp
#include <fstream>
#include <iostream>
using namespace std;

// ❌ C-style - Manual close
void cStyle() {
    FILE* file = fopen("data.txt", "w");
    if (!file) return;
    
    fprintf(file, "Hello\n");
    fclose(file);  // Must remember!
}

// ✅ RAII - Automatic close
void raii() {
    ofstream file("data.txt");  // Opens in constructor
    if (!file) return;
    
    file << "Hello\n";
}  // Closes in destructor!

// Custom RAII wrapper
class FileHandle {
    FILE* file;
public:
    FileHandle(const char* filename, const char* mode) 
        : file(fopen(filename, mode)) {
        if (!file) throw runtime_error("Failed to open file");
    }
    
    ~FileHandle() {
        if (file) {
            fclose(file);
            cout << "File closed\n";
        }
    }
    
    FILE* get() { return file; }
    
    // Prevent copying
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};

void useCustomWrapper() {
    FileHandle file("data.txt", "w");
    fprintf(file.get(), "Hello\n");
}  // Automatic close!
```

---

#### **3. Mutex/Lock Management:**

```cpp
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

mutex mtx;
int sharedData = 0;

// ❌ Manual lock/unlock - Error-prone
void unsafeIncrement() {
    mtx.lock();
    sharedData++;
    // What if exception here?
    mtx.unlock();  // Might not be reached!
}

// ✅ RAII with lock_guard
void safeIncrement() {
    lock_guard<mutex> lock(mtx);  // Locks in constructor
    sharedData++;
    // Even if exception, destructor unlocks!
}  // Unlocks in destructor

int main() {
    thread t1(safeIncrement);
    thread t2(safeIncrement);
    
    t1.join();
    t2.join();
    
    cout << "Final value: " << sharedData << "\n";
    return 0;
}
```

---

#### **4. Database Connections:**

```cpp
class DatabaseConnection {
    bool connected;
public:
    DatabaseConnection(const string& connStr) {
        // Connect to database
        connected = true;
        cout << "Connected to database\n";
    }
    
    ~DatabaseConnection() {
        if (connected) {
            // Close connection
            cout << "Disconnected from database\n";
            connected = false;
        }
    }
    
    void execute(const string& query) {
        cout << "Executing: " << query << "\n";
    }
    
    // Prevent copying
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
};

void queryDatabase() {
    DatabaseConnection db("localhost:5432");
    db.execute("SELECT * FROM users");
    // Exception or early return - still disconnects!
}  // Automatic disconnect
```

---

### **RAII with Move Semantics:**

**Q: With move semantics, how will you ensure deallocation to avoid dangling pointer?**

**Answer:**

**Move semantics + RAII ensures safe deallocation through a three-step mechanism:**

**1. Ownership Transfer:**
When an object is moved, the **source object transfers ownership** of the resource to the destination object. The source's internal pointer is set to `nullptr` immediately after the transfer. This means the source no longer "owns" the resource.

**2. Safe Source Destruction:**
When the source object's destructor runs, it checks if the pointer is `nullptr` before attempting deallocation. Since the pointer was nullified during the move, the destructor safely does nothing. This prevents **double deletion**.

**3. Guaranteed Cleanup by Final Owner:**
The destination object (which now owns the resource) will eventually go out of scope. When it does, its destructor executes and **properly deallocates** the resource. Since only one object ever owns the resource at any time, there's exactly **one deallocation** - no more, no less.

**Why This Prevents Dangling Pointers:**
- **No double delete:** Source pointer is `nullptr`, so its destructor won't delete again
- **No memory leak:** Destination object's destructor ensures cleanup
- **No dangling pointer:** After deallocation by the final owner, no other object holds a pointer to that memory (source has `nullptr`)
- **Clear ownership:** At any moment, exactly one object owns the resource

**Key Principle:** RAII + Move Semantics = "Whoever owns the resource at destruction time cleans it up." By ensuring only one owner exists at any time (enforced by setting source to `nullptr`), we guarantee exactly one deallocation with no dangling pointers.

**Quick Interview Answer:**
> "With move semantics, I ensure deallocation without dangling pointers through RAII: The move constructor transfers ownership by copying the pointer and then setting the source's pointer to `nullptr`. When the source object destructs, it checks for `nullptr` and skips deletion. The destination object, being the sole owner, properly deallocates when it destructs. This guarantees exactly one deallocation with no double-delete or dangling pointer issues."

---

**Code Example Demonstrating the Mechanism:**

```cpp
#include <iostream>
#include <utility>
using namespace std;

class RAIIResource {
    int* data;
public:
    // Constructor - Acquire resource
    RAIIResource(int size) : data(new int[size]) {
        cout << "Resource acquired (" << data << ")\n";
    }
    
    // Destructor - Release resource
    ~RAIIResource() {
        if (data) {
            cout << "Resource released (" << data << ")\n";
            delete[] data;
            data = nullptr;
        }
    }
    
    // Copy operations deleted
    RAIIResource(const RAIIResource&) = delete;
    RAIIResource& operator=(const RAIIResource&) = delete;
    
    // Move constructor - Transfer ownership
    RAIIResource(RAIIResource&& other) noexcept : data(other.data) {
        other.data = nullptr;  // Source loses ownership
        cout << "Resource moved (" << data << ")\n";
    }
    
    // Move assignment - Transfer ownership
    RAIIResource& operator=(RAIIResource&& other) noexcept {
        if (this != &other) {
            // Clean up current resource
            delete[] data;
            
            // Transfer ownership
            data = other.data;
            other.data = nullptr;
            
            cout << "Resource move-assigned (" << data << ")\n";
        }
        return *this;
    }
};

RAIIResource createResource() {
    RAIIResource res(100);  // Acquire
    return res;  // Move (RVO may elide this)
}  // Original destroyed (but data is nullptr after move)

int main() {
    cout << "Creating resource...\n";
    RAIIResource res1 = createResource();
    
    cout << "\nMoving resource...\n";
    RAIIResource res2 = std::move(res1);
    // res1 no longer owns resource (data = nullptr)
    // res2 now owns it
    
    cout << "\nExiting scope...\n";
    return 0;
}
/* Output:
Creating resource...
Resource acquired (0x...)
Moving resource...
Resource moved (0x...)
Exiting scope...
Resource released (0x...)
*/
```

**Key Points:**
- After move, source object's destructor runs but does nothing (data = nullptr)
- Destination object's destructor cleans up the resource
- **No memory leak**, **no dangling pointer**, **no double delete**

---

### **RAII Design Pattern:**

```cpp
template<typename ResourceType>
class RAIIWrapper {
    ResourceType* resource;
    
public:
    // Acquire resource in constructor
    explicit RAIIWrapper(ResourceType* res) : resource(res) {}
    
    // Release resource in destructor
    ~RAIIWrapper() {
        cleanup();
    }
    
    // Move semantics
    RAIIWrapper(RAIIWrapper&& other) noexcept 
        : resource(other.resource) {
        other.resource = nullptr;
    }
    
    RAIIWrapper& operator=(RAIIWrapper&& other) noexcept {
        if (this != &other) {
            cleanup();
            resource = other.resource;
            other.resource = nullptr;
        }
        return *this;
    }
    
    // Prevent copying
    RAIIWrapper(const RAIIWrapper&) = delete;
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;
    
    // Access
    ResourceType* get() { return resource; }
    ResourceType& operator*() { return *resource; }
    ResourceType* operator->() { return resource; }
    
private:
    void cleanup() {
        if (resource) {
            delete resource;
            resource = nullptr;
        }
    }
};
```

---

### **RAII in Standard Library:**

| Class | Resource Managed | Acquisition | Release |
|-------|------------------|-------------|----------|
| **unique_ptr** | Dynamic memory | Constructor | Destructor |
| **shared_ptr** | Dynamic memory | Constructor | Destructor (last owner) |
| **vector** | Dynamic array | Constructor/resize | Destructor |
| **string** | Character array | Constructor | Destructor |
| **fstream** | File handle | Constructor/open() | Destructor/close() |
| **lock_guard** | Mutex | Constructor (locks) | Destructor (unlocks) |
| **unique_lock** | Mutex | Constructor (locks) | Destructor (unlocks) |
| **thread** | Thread handle | Constructor | Destructor (must join/detach) |

---

### **RAII vs Manual Management:**

```cpp
// ❌ Manual Management - 5 ways to leak
void manualManagement() {
    Resource* res = acquire();
    
    if (error1) return;        // Leak #1: Early return
    if (error2) throw Ex();    // Leak #2: Exception
    
    process(res);              // Leak #3: Exception in process
    
    if (error3) goto cleanup;  // Leak #4: Forgot goto label
    
    release(res);              // Leak #5: Never reached
cleanup:
    release(res);
}

// ✅ RAII - Impossible to leak
void raiiManagement() {
    unique_ptr<Resource> res(acquire());
    
    if (error1) return;        // ✅ Destructor called
    if (error2) throw Ex();    // ✅ Destructor called
    
    process(res.get());        // ✅ Destructor called on exception
    
    if (error3) return;        // ✅ Destructor called
    
}  // ✅ Destructor always called
```

---

### **Common RAII Mistakes:**

#### **1. Creating Temporary:**
```cpp
// ❌ WRONG: Creates temporary, destroyed immediately
lock_guard<mutex>(mtx);  // Lock acquired and released instantly!
sharedData++;  // Not protected!

// ✅ CORRECT: Named object lives until scope end
lock_guard<mutex> lock(mtx);
sharedData++;  // Protected
```

#### **2. Forgetting to Transfer Ownership:**
```cpp
// ❌ WRONG: Local object destroyed, double delete
unique_ptr<int> getPointer() {
    int* raw = new int(42);
    unique_ptr<int> ptr(raw);
    return unique_ptr<int>(raw);  // ❌ Two owners!
}

// ✅ CORRECT: Move ownership
unique_ptr<int> getPointer() {
    return make_unique<int>(42);  // Move ownership
}
```

---

### **Interview Points:**

**Definition:**
- RAII = Resource Acquisition Is Initialization
- Resources acquired in constructor, released in destructor
- Resource lifetime = Object lifetime

**Benefits:**
1. **Automatic cleanup** - No manual delete/close/free
2. **Exception safe** - Destructor always called during stack unwinding
3. **Prevents leaks** - Impossible to forget cleanup
4. **Prevents dangling pointers** - Memory freed at right time
5. **Thread-safe** - Works with locks (lock_guard)

**Move Semantics + RAII (Preventing Dangling Pointers):**
- **Ownership transfer:** Move transfers resource ownership from source to destination
- **Source nullification:** Source object's pointer immediately set to `nullptr` after move
- **Safe source destruction:** Source destructor checks `nullptr` and skips deallocation
- **Destination ownership:** Only destination object owns the resource
- **Final cleanup:** Destination destructor eventually deallocates the resource
- **Guarantee:** Exactly one deallocation, no double deletes, no dangling pointers
- **Key insight:** At any time, exactly one object owns the resource (enforced by `nullptr` in source)

**Standard Library Examples:**
- Smart pointers: `unique_ptr`, `shared_ptr`
- Containers: `vector`, `string`, `map`
- I/O: `ifstream`, `ofstream`
- Threading: `lock_guard`, `unique_lock`, `thread`

**Best Practice:**
```cpp
// ❌ Old C++ (pre-C++11)
Resource* res = new Resource();
try {
    use(res);
} catch(...) {
    delete res;
    throw;
}
delete res;

// ✅ Modern C++ (RAII)
auto res = make_unique<Resource>();
use(res.get());  // Exception-safe automatically!
```

**Golden Rule:**
> In modern C++, you should **almost never** write `new` or `delete` in application code. Let RAII classes (smart pointers, containers) handle it.

---

**Next Files:**
- [Modern C++ Keywords](3_Modern_Cpp_Keywords.md)
- [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md)
- [STL Internals](5_STL_Internals.md)
- [Multithreading Deep Dive](6_Multithreading_Deep_Dive.md)
- [Performance Optimization](7_Performance_Optimization.md)
