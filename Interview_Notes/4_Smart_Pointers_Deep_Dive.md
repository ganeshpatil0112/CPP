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
#include <iostream>
#include <utility>   // std::move
using namespace std;

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

// ---------- Helper class to observe construction/destruction ----------
class Resource {
    int id;
public:
    Resource(int i) : id(i) {
        cout << "  Resource(" << id << ") constructed\n";
    }
    ~Resource() {
        cout << "  Resource(" << id << ") destroyed\n";
    }
    void hello() const {
        cout << "  Resource(" << id << ") says hello\n";
    }
};

// Factory: demonstrates returning UniquePtr (move on return)
UniquePtr<Resource> makeResource(int id) {
    return UniquePtr<Resource>(new Resource(id));
}

// ---------- Test driver ----------
int main() {
    cout << "=== 1. Basic construction & dereference ===\n";
    UniquePtr<Resource> p1(new Resource(1));
    p1->hello();              // operator->
    (*p1).hello();            // operator*
    cout << "  get() = " << p1.get() << "\n\n";

    cout << "=== 2. Copy is deleted (these lines won't compile) ===\n";
    // UniquePtr<Resource> bad1 = p1;       // ERROR: copy ctor deleted
    // UniquePtr<Resource> bad2; bad2 = p1; // ERROR: copy assign deleted
    cout << "  (copy/assign correctly disabled)\n\n";

    cout << "=== 3. Move constructor transfers ownership ===\n";
    UniquePtr<Resource> p2 = std::move(p1);
    cout << "  p1.get() after move = " << p1.get() << " (nullptr)\n";
    cout << "  p2.get() after move = " << p2.get() << " (owns Resource(1))\n";
    p2->hello();
    cout << "\n";

    cout << "=== 4. Move assignment ===\n";
    UniquePtr<Resource> p3(new Resource(2));   // p3 owns Resource(2)
    p3 = std::move(p2);                         // Resource(2) deleted, p3 takes Resource(1)
    cout << "  p2.get() after move = " << p2.get() << " (nullptr)\n";
    p3->hello();
    cout << "\n";

    cout << "=== 5. release() — give up ownership without delete ===\n";
    Resource* raw = p3.release();
    cout << "  p3.get() after release = " << p3.get() << " (nullptr)\n";
    raw->hello();
    delete raw;   // caller now owns it
    cout << "\n";

    cout << "=== 6. reset() — replace the managed object ===\n";
    UniquePtr<Resource> p4(new Resource(10));
    p4.reset(new Resource(20));   // Resource(10) deleted, p4 takes Resource(20)
    p4->hello();
    p4.reset();                   // Resource(20) deleted, p4 = nullptr
    cout << "  p4.get() after reset() = " << p4.get() << " (nullptr)\n\n";

    cout << "=== 7. Factory function returning UniquePtr ===\n";
    UniquePtr<Resource> p5 = makeResource(99);
    p5->hello();
    cout << "\n";

    cout << "=== 8. Automatic cleanup at end of scope ===\n";
    // p5's destructor runs here -> Resource(99) destroyed
    return 0;
}

/* Expected output:

=== 1. Basic construction & dereference ===
  Resource(1) constructed
  Resource(1) says hello
  Resource(1) says hello
  get() = 0x...

=== 2. Copy is deleted (these lines won't compile) ===
  (copy/assign correctly disabled)

=== 3. Move constructor transfers ownership ===
  p1.get() after move = 0 (nullptr)
  p2.get() after move = 0x... (owns Resource(1))
  Resource(1) says hello

=== 4. Move assignment ===
  Resource(2) constructed
  Resource(2) destroyed
  p2.get() after move = 0 (nullptr)
  Resource(1) says hello

=== 5. release() — give up ownership without delete ===
  p3.get() after release = 0 (nullptr)
  Resource(1) says hello
  Resource(1) destroyed

=== 6. reset() — replace the managed object ===
  Resource(10) constructed
  Resource(20) constructed
  Resource(10) destroyed
  Resource(20) says hello
  Resource(20) destroyed
  p4.get() after reset() = 0 (nullptr)

=== 7. Factory function returning UniquePtr ===
  Resource(99) constructed
  Resource(99) says hello

=== 8. Automatic cleanup at end of scope ===
  Resource(99) destroyed
*/
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

### **Use Cases of `unique_ptr`:**

1. **Default smart pointer / sole ownership of a heap object**
   - Whenever a single piece of code is logically responsible for a resource's
     lifetime, `unique_ptr` is the right default. It costs nothing extra over
     a raw pointer but guarantees cleanup.
   ```cpp
   auto cfg = std::make_unique<Config>("settings.json");
   ```

2. **Factory functions returning a heap-allocated object**
   - Returning `unique_ptr` makes the ownership transfer explicit and
     exception-safe; the caller decides what to do (keep it, move into
     a `shared_ptr`, store in a container, etc.).
   ```cpp
   std::unique_ptr<Shape> createShape(ShapeType t);
   ```

3. **PIMPL (Pointer to IMPLementation) idiom**
   - The header holds `std::unique_ptr<Impl>`; the `.cpp` defines `Impl`.
     Hides implementation details, reduces compile-time coupling, and the
     `unique_ptr` cleans up the impl automatically.
   ```cpp
   // header
   class Widget { struct Impl; std::unique_ptr<Impl> p; };
   ```

4. **Polymorphic ownership / storing derived objects via base pointer**
   - Containers of `unique_ptr<Base>` are the modern replacement for
     `vector<Base*>` with manual `delete` loops.
   ```cpp
   std::vector<std::unique_ptr<Animal>> zoo;
   zoo.push_back(std::make_unique<Dog>());
   ```

5. **RAII for non-memory resources (with custom deleter)**
   - File handles, sockets, OS handles, C-API resources — anything with a
     paired "create / destroy" pair. The custom deleter is part of the type.
   ```cpp
   std::unique_ptr<FILE, decltype(&std::fclose)> f(std::fopen("a.txt","r"), &std::fclose);
   ```

6. **Strict tree-style ownership (parent owns children)**
   - In trees / ASTs / scene graphs where each node has exactly one owner,
     `unique_ptr` enforces that invariant at compile time.

7. **Convertible to `shared_ptr` later**
   - You can `std::move` a `unique_ptr` into a `shared_ptr` if/when shared
     ownership becomes necessary — start strict, relax later. The reverse
     is not possible.

**Rule of thumb:** *Always start with `unique_ptr`.* Promote to `shared_ptr`
only when you genuinely need shared ownership.

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

### **Use Cases of `shared_ptr`:**

1. **Genuinely shared ownership**
   - When multiple independent parts of the code legitimately need to keep
     an object alive and you can't predict which one will be the last to
     release it (e.g., a config object held by many subsystems).

2. **Asynchronous / callback-based code**
   - In Asio, networking libraries, thread pools, etc. an async operation
     must keep its target alive until completion. Capturing
     `shared_ptr<Self>` (often via `enable_shared_from_this` →
     `shared_from_this()`) is the standard idiom.
   ```cpp
   socket.async_read(buf, [self = shared_from_this()](auto ec, auto n) { ... });
   ```

3. **Containers of polymorphic objects shared between systems**
   - Same object referenced from multiple containers/observers (e.g., a
     `Player` referenced by render list, physics list, and AI list).
   ```cpp
   std::vector<std::shared_ptr<GameObject>> renderList, physicsList;
   ```

4. **Caches / lookup tables that hand out references**
   - The cache and the consumers all share ownership; the object lives as
     long as someone holds a `shared_ptr`. (Pair with `weak_ptr` for
     auto-eviction — see weak_ptr §4.)

5. **Multi-threaded shared state**
   - Reference counting is **atomic / thread-safe** (the count itself —
     not the pointee). Safe to copy a `shared_ptr` across threads; you
     still need a mutex/atomic for the *object's* data.

6. **Custom deleters with shared semantics**
   - Unlike `unique_ptr`, the deleter is **type-erased** and not part of
     `shared_ptr<T>`'s type, so different `shared_ptr<T>` instances with
     different deleters are interchangeable. Useful for managing C-API
     handles that need shared ownership.
   ```cpp
   std::shared_ptr<sqlite3> db(raw, sqlite3_close);
   ```

7. **Aliasing constructor — share a sub-object's lifetime**
   - `shared_ptr<Member>` that keeps the *enclosing* object alive:
   ```cpp
   auto owner = std::make_shared<Big>();
   std::shared_ptr<Field> f(owner, &owner->field);   // shares ownership w/ owner
   ```
   - Used when you need to hand out a pointer to a member while ensuring
     the parent object isn't destroyed.

8. **Type-erased, copyable handle**
   - `shared_ptr<void>` is occasionally used as a generic, copyable,
     auto-cleaning handle for opaque resources (e.g., plugin systems).

**When NOT to use `shared_ptr`:**
- Single-owner scenarios → use `unique_ptr` (cheaper, clearer intent).
- Tight inner loops / performance-critical paths → atomic refcount has cost.
- Cyclic references → use `weak_ptr` to break the cycle.
- As a substitute for thinking about ownership — `shared_ptr` everywhere
  is a common anti-pattern that hides design problems.

**Rule of thumb:** Use `shared_ptr` only when *true shared ownership* exists
and a single owner cannot be identified.

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

### **Use Cases of `weak_ptr`:**

1. **Break circular references (primary use case)**
   - Two `shared_ptr`s referring to each other create a cycle → reference count never reaches 0 → **memory leak**.
   - Replacing one side with a `weak_ptr` breaks the cycle so destructors run normally.
   - Common in: Parent ↔ Child, Node ↔ Node (graphs/trees), Subject ↔ Observer.

2. **Parent → Child uses `shared_ptr`, Child → Parent uses `weak_ptr`**
   - The parent owns the child; the child only *observes* the parent.
   - Prevents the child from extending the parent's lifetime.
   ```cpp
   class Parent {
       std::vector<std::shared_ptr<Child>> children;   // owns children
   };
   class Child {
       std::weak_ptr<Parent> parent;                   // does NOT own parent
   };
   ```

3. **Observer pattern**
   - The subject keeps `weak_ptr`s to observers so observers can be destroyed
     independently. Subject calls `lock()` before notifying — if it returns
     empty, the observer is gone and can be skipped/cleaned up.

4. **Caches**
   - Cache stores `weak_ptr<T>` keyed by id. As long as a real owner (`shared_ptr`)
     exists somewhere, the cache returns it via `lock()`. When all owners go away,
     the entry expires automatically and memory is freed → no manual eviction needed.
   ```cpp
   std::unordered_map<int, std::weak_ptr<Resource>> cache;
   std::shared_ptr<Resource> get(int id) {
       if (auto sp = cache[id].lock()) return sp;        // still alive
       auto sp = load(id);
       cache[id] = sp;                                   // weak reference
       return sp;
   }
   ```

5. **Safe access to objects with unknown lifetime**
   - When a class stores a pointer to an object it does NOT own (e.g., a callback
     target, a "back-pointer" in async code), `weak_ptr` lets you check via
     `lock()` whether the object is still alive before using it — avoiding
     dangling-pointer UB that a raw pointer would cause.

6. **`std::enable_shared_from_this`**
   - Internally holds a `weak_ptr<T>` so a class can hand out additional
     `shared_ptr`s to itself without creating a second control block.
     Foundational for asynchronous code (e.g., Boost.Asio handlers) where
     `shared_from_this()` extends the object's lifetime for the duration
     of an async operation.

7. **Trees / Graphs / Doubly-linked lists**
   - Forward edges = `shared_ptr` (ownership), back edges / cross edges =
     `weak_ptr` (observation). Eliminates cycles in arbitrary graph structures.

**Rule of thumb:** Use `shared_ptr` for *ownership*, `weak_ptr` for
*observation*. If the relationship is "I need to know about it but I don't
control its lifetime," reach for `weak_ptr`.

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
- [Multithreading Deep Dive](6_Multithreading_Deep_Dive.md)
- [Performance Optimization](7_Performance_Optimization.md)
