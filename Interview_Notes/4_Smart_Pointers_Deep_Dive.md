# Smart Pointers Deep Dive - Interview Notes

## Table of Contents
1. [unique_ptr](#unique_ptr)
2. [shared_ptr](#shared_ptr)
3. [weak_ptr](#weak_ptr)
4. [Custom Deleters](#custom-deleters)
5. [make_unique and make_shared](#make_unique-and-make_shared)

---

## unique_ptr

### **Q: How Does unique_ptr Work Internally?**

**Answer:**
`unique_ptr` enforces **exclusive ownership** by **deleting copy operations** and only allowing **move operations**. Internally, it stores a raw pointer and automatically deletes it in the destructor.

### **Simplified Implementation:**

```cpp
template<typename T>
class UniquePtr {
    T* ptr;
    
public:
    // Constructor
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}
    
    // Destructor - automatic cleanup
    ~UniquePtr() {
        delete ptr;
    }
    
    // Copy operations DELETED - prevents copying
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    // Move constructor - transfers ownership
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;  // Source loses ownership
    }
    
    // Move assignment - transfers ownership
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;  // Clean up current resource
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    // Dereference operators
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    
    // Release ownership without deleting
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }
    
    // Replace managed object
    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }
};
```

### **Why Copy is Deleted:**

```cpp
void example() {
    unique_ptr<int> ptr1(new int(42));
    
    // ❌ Won't compile - copy constructor deleted
    // unique_ptr<int> ptr2 = ptr1;
    
    // ✅ Must use move - transfers ownership
    unique_ptr<int> ptr2 = std::move(ptr1);
    
    // ptr1 is now nullptr
    if (!ptr1) {
        cout << "ptr1 is empty\n";
    }
}
```

### **Complete Usage Example:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Resource {
    int value;
public:
    Resource(int v) : value(v) {
        cout << "Resource(" << value << ") created\n";
    }
    
    ~Resource() {
        cout << "Resource(" << value << ") destroyed\n";
    }
    
    void display() {
        cout << "Value: " << value << "\n";
    }
};

unique_ptr<Resource> createResource(int val) {
    return unique_ptr<Resource>(new Resource(val));  // Move on return
}

int main() {
    // 1. Basic usage
    unique_ptr<Resource> ptr1(new Resource(10));
    ptr1->display();
    
    // 2. Transfer ownership
    unique_ptr<Resource> ptr2 = std::move(ptr1);
    // ptr1 is now nullptr
    
    // 3. Factory pattern
    auto ptr3 = createResource(20);
    
    // 4. Array version
    unique_ptr<int[]> arr(new int[5]);
    arr[0] = 100;
    
    // Automatic cleanup when going out of scope
    return 0;
}
/* Output:
Resource(10) created
Value: 10
Resource(20) created
Resource(10) destroyed
Resource(20) destroyed
*/
```

### **Interview Points:**
- Zero overhead - same as raw pointer
- Copy constructor and copy assignment are `= delete`
- Move operations transfer ownership
- Automatic cleanup in destructor
- Can't have multiple owners (enforced at compile-time)

---

## shared_ptr

### **Q: How Does shared_ptr Implement Reference Counting?**

**Answer:**
`shared_ptr` uses a **control block** that stores the **reference count** and **deleter**. Multiple `shared_ptr` objects point to the **same control block**. When count reaches zero, resource is deleted.

### **Internal Structure:**

```cpp
// Simplified shared_ptr implementation
template<typename T>
class SharedPtr {
    T* ptr;                    // Pointer to managed object
    ControlBlock* control;     // Pointer to control block
    
    struct ControlBlock {
        size_t refCount;       // Number of shared_ptrs
        size_t weakCount;      // Number of weak_ptrs
        
        ControlBlock() : refCount(1), weakCount(0) {}
    };
    
public:
    // Constructor
    explicit SharedPtr(T* p = nullptr) 
        : ptr(p), control(p ? new ControlBlock() : nullptr) {}
    
    // Destructor
    ~SharedPtr() {
        if (control) {
            control->refCount--;
            if (control->refCount == 0) {
                delete ptr;  // Delete managed object
                if (control->weakCount == 0) {
                    delete control;  // Delete control block
                }
            }
        }
    }
    
    // Copy constructor - increases reference count
    SharedPtr(const SharedPtr& other) 
        : ptr(other.ptr), control(other.control) {
        if (control) {
            control->refCount++;
        }
    }
    
    // Copy assignment
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // Decrement current reference
            this->~SharedPtr();
            
            // Copy and increment new reference
            ptr = other.ptr;
            control = other.control;
            if (control) {
                control->refCount++;
            }
        }
        return *this;
    }
    
    // Move constructor - no reference count change
    SharedPtr(SharedPtr&& other) noexcept 
        : ptr(other.ptr), control(other.control) {
        other.ptr = nullptr;
        other.control = nullptr;
    }
    
    // Operators
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    
    size_t use_count() const {
        return control ? control->refCount : 0;
    }
};
```

### **Reference Counting in Action:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Data {
    int value;
public:
    Data(int v) : value(v) {
        cout << "Data(" << value << ") created\n";
    }
    
    ~Data() {
        cout << "Data(" << value << ") destroyed\n";
    }
    
    void display() {
        cout << "Value: " << value << "\n";
    }
};

int main() {
    shared_ptr<Data> ptr1(new Data(100));
    cout << "Count: " << ptr1.use_count() << "\n";  // 1
    
    {
        shared_ptr<Data> ptr2 = ptr1;  // Copy - increases count
        cout << "Count: " << ptr1.use_count() << "\n";  // 2
        
        shared_ptr<Data> ptr3 = ptr1;  // Copy again
        cout << "Count: " << ptr1.use_count() << "\n";  // 3
        
        ptr2->display();
        // ptr2 and ptr3 destroyed here, count goes to 1
    }
    
    cout << "Count: " << ptr1.use_count() << "\n";  // 1
    
    // ptr1 destroyed here, count goes to 0, Data deleted
    return 0;
}
/* Output:
Data(100) created
Count: 1
Count: 2
Count: 3
Value: 100
Count: 1
Data(100) destroyed
*/
```

### **Memory Layout:**

```
Stack:                          Heap:
┌──────────┐                   ┌──────────────┐
│  ptr1    │──────────┬───────>│   Data obj   │
│    ptr   │          │        │  value = 100 │
│  control │──┐       │        └──────────────┘
└──────────┘  │       │               ↑
              │       │               │
┌──────────┐  │       │               │
│  ptr2    │  │       └───────────────┘
│    ptr   │──┘                       │
│  control │──┬───────────────────────┘
└──────────┘  │                       
              │        ┌─────────────────┐
              └───────>│ Control Block   │
                       │  refCount = 2   │
                       │  weakCount = 0  │
                       └─────────────────┘
```

### **Copy vs Move:**

```cpp
shared_ptr<Data> ptr1(new Data(1));

// COPY - increases reference count
shared_ptr<Data> ptr2 = ptr1;  // refCount: 1 → 2

// MOVE - transfers ownership, no count change
shared_ptr<Data> ptr3 = std::move(ptr1);  // refCount stays 2
// ptr1 is now empty
```

### **Interview Points:**
- Control block stores reference count
- Copy increases count; destructor decreases count
- When count reaches 0, object deleted
- Thread-safe reference counting (atomic operations)
- Overhead: 2 heap allocations (object + control block)

---

## weak_ptr

### **Q: What is weak_ptr and Why is it Needed?**

**Answer:**
`weak_ptr` is a **non-owning reference** to an object managed by `shared_ptr`. It **doesn't increase reference count**, so it can't keep object alive. Used to break **circular references** (cycles).

### **The Circular Reference Problem:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class B;  // Forward declaration

class A {
public:
    shared_ptr<B> ptrB;
    ~A() { cout << "A destroyed\n"; }
};

class B {
public:
    shared_ptr<A> ptrA;  // ❌ PROBLEM: Creates cycle
    ~B() { cout << "B destroyed\n"; }
};

int main() {
    {
        shared_ptr<A> a(new A());
        shared_ptr<B> b(new B());
        
        a->ptrB = b;  // A → B
        b->ptrA = a;  // B → A (cycle!)
        
        // a.refCount = 2 (main + B::ptrA)
        // b.refCount = 2 (main + A::ptrB)
    }
    // Scope ends: main's references destroyed
    // But a.refCount = 1, b.refCount = 1 (still pointing to each other)
    // MEMORY LEAK! Neither A nor B destroyed
    
    return 0;
}
/* Output:
(Nothing! No destructors called - memory leak)
*/
```

### **Solution With weak_ptr:**

```cpp
class B;

class A {
public:
    shared_ptr<B> ptrB;
    ~A() { cout << "A destroyed\n"; }
};

class B {
public:
    weak_ptr<A> ptrA;  // ✅ SOLUTION: Use weak_ptr
    ~B() { cout << "B destroyed\n"; }
};

int main() {
    {
        shared_ptr<A> a(new A());
        shared_ptr<B> b(new B());
        
        a->ptrB = b;  // A → B (shared_ptr)
        b->ptrA = a;  // B → A (weak_ptr - doesn't increase count!)
        
        // a.refCount = 1 (only main)
        // b.refCount = 2 (main + A::ptrB)
    }
    // Scope ends: a.refCount → 0, A destroyed
    // A destroyed → ptrB released → b.refCount → 0, B destroyed
    
    return 0;
}
/* Output:
A destroyed
B destroyed
*/
```

### **Using weak_ptr:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    weak_ptr<int> weakPtr;
    
    {
        shared_ptr<int> sharedPtr = make_shared<int>(42);
        weakPtr = sharedPtr;  // weak_ptr observes
        
        cout << "Shared count: " << sharedPtr.use_count() << "\n";  // 1
        cout << "Weak count: " << weakPtr.use_count() << "\n";      // 1
        
        // Access through weak_ptr
        if (auto locked = weakPtr.lock()) {  // Converts to shared_ptr
            cout << "Value: " << *locked << "\n";  // 42
        }
        
        // sharedPtr destroyed here
    }
    
    // Object is destroyed, weak_ptr is expired
    if (weakPtr.expired()) {
        cout << "Object no longer exists\n";
    }
    
    // Try to access - returns empty shared_ptr
    if (auto locked = weakPtr.lock()) {
        cout << "This won't print\n";
    } else {
        cout << "weak_ptr couldn't create shared_ptr\n";
    }
    
    return 0;
}
/* Output:
Shared count: 1
Weak count: 1
Value: 42
Object no longer exists
weak_ptr couldn't create shared_ptr
*/
```

### **weak_ptr API:**

```cpp
weak_ptr<int> wp;

// Check if object still exists
if (wp.expired()) {
    // Object destroyed
}

// Try to access (safe)
if (auto sp = wp.lock()) {
    // sp is shared_ptr - safe to use
    *sp = 100;
}

// Get count
size_t count = wp.use_count();  // Number of shared_ptrs
```

### **Complete Example - Observer Pattern:**

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Subject;

class Observer {
    weak_ptr<Subject> subject;  // Non-owning reference
public:
    void setSubject(shared_ptr<Subject> s) {
        subject = s;
    }
    
    void update() {
        if (auto s = subject.lock()) {
            cout << "Subject still exists\n";
        } else {
            cout << "Subject destroyed\n";
        }
    }
};

class Subject {
    vector<shared_ptr<Observer>> observers;  // Owns observers
public:
    void attach(shared_ptr<Observer> obs) {
        observers.push_back(obs);
        obs->setSubject(/* this as shared_ptr */);
    }
    
    ~Subject() {
        cout << "Subject destroyed\n";
    }
};

int main() {
    auto observer = make_shared<Observer>();
    
    {
        auto subject = make_shared<Subject>();
        observer->setSubject(subject);
        observer->update();  // "Subject still exists"
    }
    
    observer->update();  // "Subject destroyed"
    return 0;
}
```

### **Interview Points:**
- Non-owning reference to shared_ptr managed object
- Doesn't increase reference count
- Use `lock()` to safely access object (returns shared_ptr)
- Use `expired()` to check if object still exists
- Breaks circular references
- Use in observer pattern, caches, parent-child relationships

---

## Custom Deleters

### **Q: How to Use Custom Deleters with Smart Pointers?**

**Answer:**
Custom deleters allow you to specify **custom cleanup logic** beyond simple `delete`. Useful for arrays, file handles, C APIs, custom allocators.

### **unique_ptr with Custom Deleter:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

// Custom deleter as function
void arrayDeleter(int* ptr) {
    cout << "Custom delete[]\n";
    delete[] ptr;
}

// Custom deleter as lambda
auto fileDeleter = [](FILE* file) {
    if (file) {
        cout << "Closing file\n";
        fclose(file);
    }
};

int main() {
    // Array with custom deleter
    unique_ptr<int[], decltype(&arrayDeleter)> arr(
        new int[10], 
        arrayDeleter
    );
    
    // File with lambda deleter
    unique_ptr<FILE, decltype(fileDeleter)> file(
        fopen("test.txt", "w"),
        fileDeleter
    );
    
    if (file) {
        fprintf(file.get(), "Hello\n");
    }
    
    // Automatic cleanup with custom deleters
    return 0;
}
```

### **shared_ptr with Custom Deleter:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Resource {
public:
    Resource() { cout << "Resource created\n"; }
    ~Resource() { cout << "Resource destroyed\n"; }
};

void customDeleter(Resource* ptr) {
    cout << "Custom deletion logic\n";
    delete ptr;
}

int main() {
    // Deleter as function pointer
    shared_ptr<Resource> ptr1(new Resource(), customDeleter);
    
    // Deleter as lambda
    shared_ptr<Resource> ptr2(new Resource(), [](Resource* p) {
        cout << "Lambda deletion\n";
        delete p;
    });
    
    // Array with custom deleter
    shared_ptr<int> arr(new int[10], [](int* p) {
        delete[] p;
    });
    
    return 0;
}
/* Output:
Resource created
Resource created
Resource destroyed
Lambda deletion
Resource destroyed
Custom deletion logic
*/
```

### **Real-World Examples:**

```cpp
// 1. C API Resource
shared_ptr<SDL_Window> window(
    SDL_CreateWindow("Title", 0, 0, 800, 600, 0),
    [](SDL_Window* w) { SDL_DestroyWindow(w); }
);

// 2. Custom Allocator
shared_ptr<MyClass> obj(
    static_cast<MyClass*>(custom_malloc(sizeof(MyClass))),
    [](MyClass* p) { custom_free(p); }
);

// 3. Socket Handle
shared_ptr<int> socket(
    new int(createSocket()),
    [](int* s) {
        closeSocket(*s);
        delete s;
    }
);

// 4. Database Connection
shared_ptr<Connection> conn(
    createConnection(),
    [](Connection* c) {
        c->commit();  // Commit before closing
        closeConnection(c);
    }
);
```

### **Interview Points:**
- Custom cleanup logic beyond `delete`
- Used for C APIs, arrays, custom allocators
- shared_ptr: deleter in constructor
- unique_ptr: deleter as template parameter
- Lambda deleters extremely common

---

## make_unique and make_shared

### **Q: Why Prefer make_unique/make_shared Over new?**

**Answer:**
- **Exception safety**: No resource leak if exception thrown
- **Efficiency** (make_shared): Single allocation instead of two
- **Less typing**: No need to repeat type

### **Exception Safety Problem:**

```cpp
void process(unique_ptr<Widget> w, int value);

// ❌ POTENTIAL LEAK
process(unique_ptr<Widget>(new Widget()), computeValue());

// Problem: Order of evaluation undefined:
// 1. new Widget()
// 2. computeValue() - might throw!
// 3. unique_ptr constructor
// If computeValue() throws, Widget leaked!

// ✅ SAFE
process(make_unique<Widget>(), computeValue());
// make_unique is atomic - no leak possible
```

### **make_shared Efficiency:**

```cpp
// ❌ TWO allocations
shared_ptr<Widget> ptr1(new Widget());
// Allocation 1: new Widget()
// Allocation 2: Control block

// ✅ ONE allocation
shared_ptr<Widget> ptr2 = make_shared<Widget>();
// Single allocation: Widget + Control block together
```

**Memory Layout Comparison:**

```
shared_ptr<T>(new T()):          make_shared<T>():
┌─────────┐   ┌──────────┐      ┌──────────┬───────────────┐
│ ptr → │───>│ T object │      │ T object │ Control Block │
│ control│─┐ └──────────┘      └──────────┴───────────────┘
└─────────┘ │                   (Single allocation!)
            │  ┌─────────────┐
            └─>│Control Block│
               └─────────────┘
(Two allocations)
```

### **Complete Usage:**

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Data {
    int value;
public:
    Data(int v) : value(v) {
        cout << "Data(" << value << ") created\n";
    }
    
    ~Data() {
        cout << "Data(" << value << ") destroyed\n";
    }
};

int main() {
    // make_unique (C++14)
    auto ptr1 = make_unique<Data>(10);
    
    // make_unique with array
    auto arr = make_unique<int[]>(5);
    arr[0] = 100;
    
    // make_shared
    auto ptr2 = make_shared<Data>(20);
    
    // Preferred over:
    // unique_ptr<Data> ptr1(new Data(10));
    // shared_ptr<Data> ptr2(new Data(20));
    
    return 0;
}
```

### **Interview Points:**
- make_unique/make_shared are safer and more efficient
- Exception-safe: atomic operation
- make_shared: one allocation vs two
- Less verbose, harder to make mistakes
- Always prefer them in modern C++

---

**Summary - Smart Pointer Decision Tree:**

```
Need smart pointer?
│
├─ Single owner?
│  └─> Use unique_ptr
│
├─ Multiple owners?
│  └─> Use shared_ptr
│
└─ Just observing shared_ptr?
   └─> Use weak_ptr (plus lock())
```

**Key Differences:**

| Feature | unique_ptr | shared_ptr | weak_ptr |
|---------|------------|------------|----------|
| **Ownership** | Exclusive | Shared | None |
| **Copyable** | No (deleted) | Yes | Yes |
| **Overhead** | Zero | Reference counting | Reference counting |
| **Use case** | Single owner | Multiple owners | Break cycles |

---

**Next Files:**
- [STL Internals](5_STL_Internals.md)
- [Performance Optimization](6_Performance_Optimization.md)
