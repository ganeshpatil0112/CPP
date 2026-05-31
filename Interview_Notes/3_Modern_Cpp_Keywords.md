# Modern C++ Keywords - Interview Notes

## Table of Contents
1. [static](#static)
2. [noexcept](#noexcept)
3. [override](#override)
4. [explicit](#explicit)
5. [= default](#default)
6. [= delete](#delete)
7. [constexpr](#constexpr)
8. [mutable](#mutable)

---

## static

### **Q: What is the `static` Keyword?**

**Answer:**
`static` has **several distinct meanings** depending on context. It controls **storage duration**, **linkage**, or **class-vs-instance membership**.

| Context | What `static` does |
|---|---|
| **Local variable inside a function** | Lives for the whole program; initialized once on first call |
| **Global variable / free function** | Gives it **internal linkage** (visible only in this translation unit) |
| **Class data member** | Single shared variable for all instances (class-level, not per-object) |
| **Class member function** | Class-level function; no `this` pointer |
| **Local variable inside a class method** | Same as static local — one shared instance across all calls |

All `static` storage lives in the **data/BSS segment**, not on the stack or heap.

---

### **1. Static Local Variable (inside a function)**

```cpp
void counter() {
    static int count = 0;   // Initialized ONCE, persists across calls
    ++count;
    cout << count << "\n";
}

int main() {
    counter();  // 1
    counter();  // 2
    counter();  // 3
}
```

**Key properties:**
- Initialized exactly once (thread-safe since C++11 — "magic statics").
- Retains its value between calls.
- Destroyed at program exit (in reverse order of construction).

**Use cases:** call counters, lazy-initialized singletons, expensive one-time setup, memoization caches.

---

### **2. Static Global Variable / Free Function (file scope)**

```cpp
// file: util.cpp
static int helperCount = 0;          // Visible only in util.cpp
static void logHelper() { ... }      // Visible only in util.cpp
```

**Effect:** **internal linkage** — these names won't collide with same-named symbols in other `.cpp` files. C++ also recommends **unnamed (anonymous) namespaces** for the same purpose:

```cpp
namespace {
    int helperCount = 0;
    void logHelper() { /*...*/ }
}
```

**Use cases:** private utilities inside a translation unit; avoiding ODR violations.

---

### **3. Static Class Data Member**

A **single variable shared by all instances** of the class.

```cpp
class Widget {
public:
    static int totalCount;    // Declaration (inside class)
    Widget()  { ++totalCount; }
    ~Widget() { --totalCount; }
};

int Widget::totalCount = 0;   // Definition (outside class, in .cpp)

int main() {
    Widget a, b, c;
    cout << Widget::totalCount;   // 3 — accessed via class name
}
```

**C++17 inline static** lets you skip the out-of-class definition:

```cpp
class Widget {
public:
    inline static int totalCount = 0;   // declaration + definition in one place
};
```

**Use cases:** instance counters, shared configuration, singleton state, lookup tables.

---

### **4. Static Class Member Function**

A function that belongs to the **class itself**, not to any instance.

```cpp
class MathUtils {
public:
    static int square(int x) { return x * x; }
};

int main() {
    int r = MathUtils::square(5);   // Called via class name — no object needed
}
```

**Key properties:**
- No `this` pointer.
- Can be called without an object.
- Cannot be `const`, `virtual`, or `volatile` (those qualify `this`, which doesn't exist here).

---

### **Q: Can a static member function access only static variables?**

**Answer: Yes — it can directly access only static members.** Because a static function has **no `this` pointer**, it has no implicit "current object" to read non-static members from.

```cpp
class Demo {
    int instanceVar = 10;       // non-static
    static int classVar;        // static

public:
    static void show() {
        cout << classVar;       // ✅ OK — static
        // cout << instanceVar; // ❌ ERROR — needs an object
    }
};
int Demo::classVar = 99;
```

**But there's a workaround** — if you give the static function an object (as a parameter, or it creates one), it can access non-static members **through that object**:

```cpp
class Demo {
    int instanceVar = 10;
public:
    static void show(const Demo& d) {
        cout << d.instanceVar;   // ✅ OK — accessed through the object 'd'
    }
};
```

So the precise rule is:

> A static member function **cannot implicitly access non-static members**, but it **can access them through an explicit object reference/pointer**.

---

### **5. Static Local Variable in a Class Method**

Behaves like a regular static local — **one shared instance across all calls from all objects**, not per-instance.

```cpp
class Logger {
public:
    void log() {
        static int callCount = 0;   // Shared across ALL Logger objects
        cout << ++callCount << "\n";
    }
};

Logger a, b;
a.log();   // 1
b.log();   // 2  (NOT 1 — shared!)
```

---

### **Advantages of `static`**

| Benefit | How `static` provides it |
|---|---|
| **Encapsulation at file scope** | Free functions/variables marked `static` don't leak across translation units. |
| **Memory efficiency** | A single static class member uses memory once, not per-object. |
| **Lifetime control** | Static locals survive the function call without heap allocation. |
| **Thread-safe lazy init** | Magic statics initialize exactly once even with concurrent first-callers. |
| **Class-level utilities** | Static methods act as namespaced helpers (math, factories, converters). |
| **Singleton implementation** | Static local inside `getInstance()` is the canonical thread-safe singleton. |
| **Reduced ODR/link conflicts** | `static` in `.cpp` files prevents duplicate-symbol link errors. |

---

### **When to Use `static`**

| Use case | Pattern |
|---|---|
| Count instances of a class | `static int count;` member |
| Helper not exposed outside `.cpp` | `static` free function (or anonymous namespace) |
| Factory / utility methods that don't need state | `static` member function |
| Memoization / cache local to a function | `static` map inside the function |
| Lazy-initialized singleton (Meyers singleton) | `static Foo& instance() { static Foo f; return f; }` |
| Constants shared across all instances | `static constexpr int kMax = 100;` |
| Logging counters, IDs | `static int nextId = 0;` |

---

### **Common Pitfalls**

1. **Static initialization order fiasco** — globals (incl. static class members) across different translation units have undefined initialization order. Prefer **function-local statics** for safe lazy init.
2. **Hidden global state** — static class members are effectively globals. Hard to test, not thread-safe by default.
3. **Multithreading on static data** — magic statics are init-safe, but subsequent reads/writes still need synchronization.
4. **`static` inside header** (pre-C++17) — each translation unit gets its own copy. Use `inline static` (C++17) or `constexpr` instead.

---

### **`static` vs related keywords**

| Keyword | Storage | Linkage | Per-object? |
|---|---|---|---|
| `static` (local) | static (data segment) | none (local) | n/a |
| `static` (global) | static | internal | n/a |
| `static` (class member) | static | external | **No** (shared) |
| (non-static member) | per-object | n/a | Yes |
| `extern` | static | external | n/a |
| `thread_local` | thread-static | as-declared | one per thread |

---

### **Interview Points**
- `static` is **context-sensitive** — five distinct meanings.
- Static members are **class-scoped**, not object-scoped.
- Static member functions have **no `this`** → can't implicitly touch non-static members.
- They **can** touch non-static members **through an explicit object argument**.
- Function-local `static` is the safest singleton (Meyers, thread-safe since C++11).
- Use `inline static` (C++17) to define class-static members in the header.
- Prefer anonymous namespaces over file-scope `static` for free functions in modern C++ (though both work).

---

## noexcept

### **Q: What is `noexcept` and Why Use It?**

**Answer:**
`noexcept` specifies that a function will **not throw exceptions**. If exception occurs, program calls `std::terminate()`. Enables **compiler optimizations** and is **critical for STL containers**.

### **Syntax:**
```cpp
void function() noexcept;              // Won't throw
void function() noexcept(true);        // Same
void function() noexcept(false);       // May throw (default)
```

### **Basic Example:**

```cpp
int add(int a, int b) noexcept {
    return a + b;  // Simple, won't throw
}

int divide(int a, int b) {  // May throw
    if(b == 0) throw runtime_error("Division by zero");
    return a / b;
}
```

###**Performance:** Vector Resize Optimization

**Theory:** STL containers provide **strong exception guarantee** - if operation fails, container unchanged. Achieves this by choosing copy (safe) vs move (fast) based on `noexcept`.

```cpp
class Element {
    int* data;
public:
    // Move WITHOUT noexcept
    Element(Element&& other) {
        data = other.data;
        other.data = nullptr;
    }
    // Problem: Vector uses COPY (slow) for safety
};

class OptimizedElement {
    int* data;
public:
    // Move WITH noexcept
    Element(Element&& other) noexcept {
        data = other.data;
        other.data = nullptr;
    }
    // Benefit: Vector uses MOVE (fast)
};
```

**Why This Matters:**

Without `noexcept`: Vector must use copy during resize (expensive)
```cpp
// Vector internals (simplified)
if constexpr (is_nothrow_move_constructible<T>) {
    // Use MOVE - fast
    ::new(&newMemory[i]) T(std::move(oldMemory[i]));
} else {
    // Use COPY - safe but slow
    ::new(&newMemory[i]) T(oldMemory[i]);
}
```

### **Critical Use Cases:**

#### **1. Move Constructors/Assignment:**
```cpp
class MyClass {
public:
    MyClass(MyClass&& other) noexcept {  // MUST be noexcept
        // Move logic
    }
    
    MyClass& operator=(MyClass&& other) noexcept {
        // Move assignment
        return *this;
    }
};
```

#### **2. Swap Functions:**
```cpp
class MyClass {
public:
    void swap(MyClass& other) noexcept {
        using std::swap;
        swap(data, other.data);
    }
};
```

#### **3. Destructors (Implicit noexcept):**
```cpp
class MyClass {
public:
    ~MyClass() noexcept {  // Implicit, but can be explicit
        // Cleanup - MUST NOT throw
    }
};
```

### **Interview Points:**
- Enables compiler optimizations (no exception handling code)
- Required for STL container move optimization
- Destructors are implicitly `noexcept` (C++11+)
- Zero runtime cost - compile-time feature

---

### **`noexcept` — Performance Use Cases in the STL and Beyond**

The STL uses a compile-time check called **`std::is_nothrow_move_constructible<T>` / `is_nothrow_move_assignable<T>`** (and similar) to decide between two paths:

1. **Fast path** → uses `std::move` (move ctor / move assign).
2. **Safe path** → uses copy (preserves the **strong exception guarantee**).

If your type's move operations are `noexcept`, you get the fast path. If they aren't, the STL silently falls back to copies — which on large or non-trivial types can be **10× to 1000× slower**.

This single fact is the most important practical reason to mark moves `noexcept`.

#### **1. `std::vector::push_back` / `emplace_back` (geometric reallocation)**

When the vector's capacity is exhausted, it allocates a new buffer and must transfer existing elements. The transfer helper is `std::move_if_noexcept`:

```cpp
// Conceptual STL implementation
for (size_t i = 0; i < size_; ++i) {
    if constexpr (std::is_nothrow_move_constructible_v<T>)
        ::new (&newBuf[i]) T(std::move(oldBuf[i]));   // MOVE — fast
    else
        ::new (&newBuf[i]) T(oldBuf[i]);              // COPY — slow but safe
}
```

**Why copies?** If a move *could* throw halfway through, the vector would be left in a corrupted state (some moved, some not) → violates the strong exception guarantee. So without `noexcept`, the vector pays the safety tax.

**Impact:** A `vector<MyType>` with 1M elements reallocating — `noexcept` move turns this from O(N) deep-copies into O(N) pointer swaps.

#### **2. `std::vector::resize` / `reserve` / `shrink_to_fit`**

Same `move_if_noexcept` mechanism. Without `noexcept`, every resize copies the entire buffer.

#### **3. `std::vector::insert` / `erase` (mid-buffer shifts)**

These shift trailing elements to fill or open a gap. With `noexcept` moves, elements are *moved* into their new slots. Without it, they are *copied*. For containers of strings, vectors-of-vectors, or any heap-owning type, the difference is enormous.

#### **4. `std::deque`, `std::list`, `std::forward_list`**

Node-based containers also call `move_if_noexcept` when moving elements (e.g., `splice` operations, allocator-aware moves, copy-on-write fallbacks). `noexcept` removes the safe-but-slow path.

#### **5. Associative containers — `std::map`, `set`, `multimap`, `multiset`, `unordered_*`**

- **Rehashing (`unordered_map`/`unordered_set`)** — when load factor is exceeded, the bucket array is reallocated and every node is reinserted. `noexcept` move ctors on the key/value types let the rehash use moves instead of copies.
- **`insert(node_handle)` / `extract` (C++17 node splicing)** — uses moves internally; benefits from `noexcept`.
- **Tree balancing in `std::map`** — internal pointer manipulation; if the key type's move/swap is `noexcept`, balancing avoids exception-rollback bookkeeping.

#### **6. `std::sort`, `std::stable_sort`, `std::partial_sort`, `std::nth_element`**

These all internally call `std::swap`. `std::swap<T>` is itself `noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>)`. When this is true:
- The sort is `noexcept` → compiler omits exception-handler tables for the inner loop.
- Introsort/quicksort partitions become tight pointer-swap loops.
- For `std::stable_sort` (which allocates a temp buffer), elements can be *moved* into the buffer instead of copied.

**Benchmark reality**: sorting `vector<string>` with `noexcept` moves is typically **2-5×** faster than the same code where move ctors aren't marked `noexcept`.

#### **7. `std::rotate`, `std::reverse`, `std::shuffle`, `std::next_permutation`**

All swap-heavy algorithms. Same story — `noexcept` swap means the inner loop is a sequence of pointer/integer moves with no exception-bookkeeping.

#### **8. `std::remove_if`, `std::unique`, `std::partition`**

These use **move-assignment** to shift elements into place. With `noexcept` move assignment, the elements are moved; otherwise copied.

#### **9. `std::swap` and member `swap()`**

Idiomatic STL classes provide a `noexcept` member `swap`:
```cpp
class MyVec {
public:
    void swap(MyVec& o) noexcept {            // never throws
        using std::swap;
        swap(ptr_,  o.ptr_);
        swap(size_, o.size_);
        swap(cap_,  o.cap_);
    }
};
inline void swap(MyVec& a, MyVec& b) noexcept { a.swap(b); }
```

**Why critical?**
- Copy-and-swap idiom relies on `swap` being `noexcept` to give *strong exception safety* in assignment operators.
- STL algorithms (sort, reverse, rotate…) all go through `swap`.
- `unordered_*` containers swap buckets internally.

#### **10. `std::string` (Small-String Optimization + grow)**

`std::string` itself has a `noexcept` move ctor in every modern stdlib. When you put `std::string` inside a `vector`, the vector can `move_if_noexcept` strings during reallocation — turning N heap-allocations into N pointer-grabs.

If you *wrap* a string in a class without forwarding the `noexcept`, you destroy this optimization. Always mark your wrapper's move `noexcept`.

#### **11. Smart pointers — `unique_ptr`, `shared_ptr`**

Both have **`noexcept` move ctor and move assignment** by spec. That's why:
- `vector<unique_ptr<T>>` resizes by moving raw pointers (just a memcpy of a pointer).
- `vector<shared_ptr<T>>` resizes without bumping atomic refcounts — moves are essentially free.
- Stuffing them in STL containers is cheap precisely because of `noexcept`.

#### **12. `std::thread` and `std::jthread` move**

Both move constructors are `noexcept`. This is what lets you store threads in a `vector<std::thread>` and have `push_back` move them without copies (threads aren't copyable at all).

#### **13. Exception specification on destructors**

Destructors are **implicitly `noexcept`** since C++11. This is what makes:
- RAII bullet-proof (destructors run during stack unwinding without `std::terminate`).
- STL containers safe — destroying half-constructed buffers during exception unwinding cannot throw again.

### **Practical Checklist — When to mark `noexcept`**

| Function | Mark `noexcept`? | Why |
|---|---|---|
| Move constructor | ✅ **Always**, if you can | STL container speed |
| Move assignment | ✅ **Always**, if you can | STL container speed |
| `swap` (member + non-member) | ✅ **Always** | Algorithms, copy-and-swap |
| Destructor | ✅ (Implicit since C++11) | Stack unwinding safety |
| Getters that just return a member | ✅ Usually | Free perf, doc intent |
| Functions called from `noexcept` paths | ✅ Yes | Avoid downgrading callers |
| Copy constructor / copy assignment | ❌ Usually no | Often allocates → may throw |
| Anything that calls `new`, `malloc`, `throw`, or unknown callbacks | ❌ No | Lying breaks `std::terminate` invariants |

### **Quick "Before/After" benchmark intuition**

| Operation on `vector<MyType>` (N = 1M) | Move `noexcept`? | Cost |
|---|---|---|
| `push_back` triggering reallocation | ❌ No | N **copies** (heap allocs if `MyType` has owning members) |
| `push_back` triggering reallocation | ✅ Yes | N **pointer moves** — essentially memcpy |
| `std::sort` | ❌ No | swap = move+move+move → copies under the hood |
| `std::sort` | ✅ Yes | swap = three pointer assignments |
| `unordered_map` rehash | ❌ No | Every node copied to new bucket array |
| `unordered_map` rehash | ✅ Yes | Every node moved (pointer-grab) |

### **Golden Rule**

> Mark every move constructor, move assignment, `swap`, and destructor `noexcept` **unless** you can prove it can throw. Doing so unlocks every STL fast-path silently, with **zero runtime cost** for the keyword itself.

---

## override

### **Q: What is `override` Keyword?**

**Answer:**
`override` explicitly indicates a function overrides a base class virtual function. It's a **compile-time safety check** that catches errors like typos, signature mismatches, and missing `virtual` in base.

### **The Problem Without `override`:**

```cpp
class Base {
public:
    virtual void display() const {
        cout << "Base\n";
    }
};

class Derived : public Base {
public:
    // Typo: "displya" - creates NEW function, doesn't override!
    void displya() const {
        cout << "Derived\n";
    }
};

int main() {
    Base* ptr = new Derived();
    ptr->display();  // Calls Base::display (NOT what you wanted!)
}
```

### **The Solution With `override`:**

```cpp
class Derived : public Base {
public:
    // ❌ Compile ERROR: no function named 'displya' to override
    // void displya() const override {}
    
    // ✅ Correct
    void display() const override {
        cout << "Derived\n";
    }
};
```

### **Errors `override` Catches:**

#### **1. Typo in Function Name:**
```cpp
class Derived : public Base {
    void proces() override {}  // ERROR: 'proces' doesn't override
    // Fixed:
    void process() override {}  //  Works
};
```

#### **2. Wrong Parameter Types:**
```cpp
class Derived : public Base {
    void setValue(double v) override {}  // ERROR: parameter mismatch
    // Fixed:
    void setValue(int v) override {}     // ✅ Works
};
```

#### **3. Missing `const`:**
```cpp
class Derived : public Base {
    int getValue() override {}        // ERROR: missing const
    // Fixed:
    int getValue() const override {}  // ✅ Works
};
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class Shape {
public:
    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() const override {
        cout << "Drawing Circle\n";
    }
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() const override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() const override {
        return width * height;
    }
};
```

### **`override` vs `virtual`:**

| Keyword | Where | Purpose |
|---------|-------|---------|
| **`virtual`** | Base class | Enables overriding |
| **`override`** | Derived class | Confirms overriding |

```cpp
class Base {
    virtual void func();  // 'virtual' enables overriding
};

class Derived : public Base {
    void func() override;  // 'override' confirms it
    // No need for 'virtual' here
};
```

### **Interview Points:**
- Compile-time safety check for correct overriding
- Catches typos, signature mismatches, const-correctness
- Makes refactoring safer
- Zero runtime cost
- Always use when overriding virtual functions

---

## explicit

### **Q: What is `explicit` Keyword?**

**Answer:**
`explicit` prevents **implicit type conversions** and **copy-initialization**. Used with constructors and conversion operators to avoid unexpected conversions.

### **The Problem Without `explicit`:**

```cpp
class MyString {
    char* data;
public:
    MyString(int size) {  // Implicit conversion constructor
        data = new char[size];
    }
};

void process(MyString str) {
    // ...
}

int main() {
    process(100);  // ⚠️ Implicitly converts 100 to MyString!
                   // Probably not what you wanted
}
```

### **The Solution With `explicit`:**

```cpp
class MyString {
    char* data;
public:
    explicit MyString(int size) {  // Prevents implicit conversion
        data = new char[size];
    }
};

void process(MyString str) {}

int main() {
    // process(100);  // ❌ Compile ERROR
    process(MyString(100));  // ✅ Explicit construction required
}
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class SmartPointer {
    int* ptr;
public:
    // Explicit constructor
    explicit SmartPointer(int* p) : ptr(p) {
        cout << "SmartPointer created\n";
    }
    
    ~SmartPointer() {
        delete ptr;
    }
};

void usePointer(SmartPointer sp) {
    cout << "Using smart pointer\n";
}

int main() {
    int* raw = new int(42);
    
    // ❌ Won't compile - explicit prevents implicit conversion
    // usePointer(raw);
    
    // ✅ Must explicitly construct
    usePointer(SmartPointer(raw));
    
    return 0;
}
```

### **When to Use `explicit`:**

```cpp
class Array {
public:
    explicit Array(int size);        // ✅ Prevent Array arr = 10;
    Array(std::initializer_list<int>);  // ❌ Allow Array arr = {1,2,3};
};

class Complex {
public:
    explicit Complex(double real);   // ✅ Prevent Complex c = 3.14;
    Complex(double real, double imag);  // OK without explicit
};
```

### **Interview Points:**
- Prevents unexpected implicit conversions
- Use for single-parameter constructors
- Makes code intent explicit and safer
- Zero runtime cost - compile-time check

---

## = default

### **Q: What Does `= default` Mean?**

**Answer:**
`= default` explicitly requests compiler to generate the **default implementation** of a special member function. Used when you've defined other constructors but still want the default one.

### **The Problem:**

```cpp
class MyClass {
    int value;
public:
    MyClass(int v) : value(v) {}
    
    // Problem: Default constructor NO LONGER auto-generated!
};

int main() {
    MyClass obj1(42);  // ✅ OK
    MyClass obj2;      // ❌ ERROR - no default constructor
}
```

### **The Solution:**

```cpp
class MyClass {
    int value;
public:
    MyClass(int v) : value(v) {}
    
    MyClass() = default;  // Explicitly request default constructor
};

int main() {
    MyClass obj1(42);  // ✅ OK
    MyClass obj2;      // ✅ OK - uses default constructor
}
```

### **Common Uses:**

```cpp
class MyClass {
public:
    // Default constructor
    MyClass() = default;
    
    // Destructor (especially virtual)
    virtual ~MyClass() = default;
    
    // Copy constructor
    MyClass(const MyClass&) = default;
    
    // Copy assignment
    MyClass& operator=(const MyClass&) = default;
    
    // Move constructor
    MyClass(MyClass&&) = default;
    
    // Move assignment
    MyClass& operator=(MyClass&&) = default;
};
```

### **Virtual Destructor Example:**

```cpp
class Base {
public:
    virtual ~Base() = default;  // Virtual + default implementation
};

class Derived : public Base {
    unique_ptr<int> data;
public:
    ~Derived() = default;  // Smart pointer cleans up automatically
};
```

### **Interview Points:**
- Explicitly requests compiler-generated implementation
- Use when other constructors defined but want default too
- Better than empty implementation (compiler can optimize better)
- Makes intent clear

---

## = delete

### **Q: What Does `= delete` Mean?**

**Answer:**
`= delete` explicitly **disables** a function, making it impossible to call. Generates **compile-time error** if used. Commonly used to prevent copying or specific type conversions.

### **Preventing Copying:**

```cpp
class Singleton {
    static Singleton* instance;
    
    Singleton() {}  // Private constructor
    
public:
    // Delete copy operations
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    static Singleton* getInstance();
};

int main() {
    Singleton* s1 = Singleton::getInstance();
    
    // ❌ Won't compile
    // Singleton s2 = *s1;  // ERROR: copy constructor deleted
}
```

### **Preventing Type Conversions:**

```cpp
class MyClass {
public:
    void process(int value) {
        cout << "Processing int: " << value << "\n";
    }
    
    // Delete other overloads to prevent implicit conversions
    void process(double) = delete;
    void process(float) = delete;
    void process(char) = delete;
};

int main() {
    MyClass obj;
    obj.process(42);      // ✅ OK
    // obj.process(3.14);   // ❌ ERROR: double version deleted
    // obj.process(3.14f);  // ❌ ERROR: float version deleted
}
```

### **Unique Ptr Implementation:**

```cpp
template<typename T>
class UniquePtr {
    T* ptr;
public:
    explicit UniquePtr(T* p) : ptr(p) {}
    ~UniquePtr() { delete ptr; }
    
    // Delete copy to enforce unique ownership
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    // Allow move (transfer ownership)
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
};
```

### **`= delete` vs Private:**

| Approach | Error Type | Can Call from Member/Friend? |
|----------|------------|------------------------------|
| **Private** | Link error | ✅ Yes (bad!) |
| **`= delete`** | Compile error | ❌ No (good!) |

### **Interview Points:**
- Explicitly disables functions
- Compile-time error (better than linker error)
- Use to prevent copying, conversion, or specific overloads
- Part of modern C++ (C++11+)

---

## constexpr

### **Q: What is `constexpr`?**

**Answer:**
`constexpr` indicates that a value/function **can be evaluated at compile-time**. Enables compile-time computation, improving performance and enabling template metaprogramming.

### **constexpr Variables:**

```cpp
constexpr int square(int x) {
    return x * x;
}

int main() {
    constexpr int result = square(5);  // Computed at compile-time
    int arr[result];  // Can use in array size (compile-time constant)
}
```

### **constexpr Functions:**

```cpp
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

int main() {
    constexpr int fact5 = factorial(5);  // Computed at compile-time: 120
    
    int n;
    cin >> n;
    int factN = factorial(n);  // Computed at runtime (n not constexpr)
}
```

### **constexpr vs const:**

| Feature | `const` | `constexpr` |
|---------|---------|-------------|
| **Meaning** | Value doesn't change | Evaluated at compile-time |
| **Initialization** | Runtime or compile-time | Must be compile-time |
| **Functions** | Not applicable | Can be constexpr |

```cpp
const int a = getValue();      // Runtime initialization OK
constexpr int b = getValue();  // ERROR if getValue() not constexpr
```

### **Interview Points:**
- Enables compile-time computation
- Can reduce runtime overhead
- Required for template non-type parameters
- More flexible than `const`

---

## mutable

### **Q: What is `mutable` Keyword?**

**Answer:**
`mutable` allows a member variable to be **modified even in const member functions**. Used for implementation details that don't affect logical constness (e.g., caching, mutexes).

### **Basic Example:**

```cpp
class Counter {
    mutable int accessCount;  // Can modify in const functions
    int value;
    
public:
    Counter(int v) : value(v), accessCount(0) {}
    
    int getValue() const {
        accessCount++;  // ✅ OK - mutable member
        return value;
    }
    
    int getAccessCount() const {
        return accessCount;
    }
};

int main() {
    const Counter c(42);
    cout << c.getValue() << "\n";  // Modifies accessCount
    cout << "Accessed: " << c.getAccessCount() << " times\n";
}
```

### **Caching Example:**

```cpp
class ExpensiveCalculation {
    int value;
    mutable bool cached;
    mutable double cachedResult;
    
public:
    double compute() const {
        if (!cached) {
            // Expensive computation
            cachedResult = /* ... */;
            cached = true;
        }
        return cachedResult;
    }
};
```

### **Mutex Example:**

```cpp
class ThreadSafe {
    mutable std::mutex mtx;  // Must be mutable for const functions
   int data;
    
public:
    int getData() const {
        std::lock_guard<std::mutex> lock(mtx);  // Locks mutex in const function
        return data;
    }
};
```

### **Interview Points:**
- Allows modification in const member functions
- Used for implementation details (caching, logging, locking)
- Doesn't affect logical constness
- Common with mutexes in thread-safe classes

---

**Summary - Quick Reference:**

|Keyword | Purpose | When to Use |
|--------|---------|-------------|
| **`noexcept`** | No exceptions | Move constructors, swap, destructors |
| **`override`** | Confirm override | All virtual function overrides |
| **`explicit`** | Prevent implicit conversion | Single-param constructors |
| **`= default`** | Use compiler default | When others defined, want default too |
| **`= delete`** | Disable function | Prevent copying, conversions |
| **`constexpr`** | Compile-time evaluation | Constants, template params |
| **`mutable`** | Modify in const | Caching, mutexes, counters |
| **`static`** | Storage / linkage / class-level | Singletons, counters, utilities, file-private helpers |

---

## C++ Standards — Feature Timeline

A quick reference of the major language/library features introduced in each C++ standard.

### **C++98 / C++03 (baseline)**
- Classes, inheritance, polymorphism, templates, exceptions
- STL: `vector`, `list`, `deque`, `map`, `set`, `string`, iterators, algorithms
- RTTI: `typeid`, `dynamic_cast`
- Casts: `static_cast`, `const_cast`, `reinterpret_cast`
- Namespaces
- `bool` type
- `mutable`
- I/O streams (`iostream`, `fstream`, `sstream`)
- C++03: value initialization fix, minor library tweaks (no new language features)

---

### **C++11** (the "modern C++" turning point)

**Language**
- `auto` type deduction
- `decltype`
- Range-based `for` loop
- Lambda expressions
- Rvalue references & **move semantics** (`&&`, `std::move`)
- `nullptr`
- `nullptr_t`
- Strongly typed `enum class`
- Uniform initialization (`{}` braces)
- `constexpr` (functions & variables)
- `noexcept`
- `override`, `final`
- `= default`, `= delete`
- Delegating & inheriting constructors
- Variadic templates
- Template aliases (`using` for templates)
- `static_assert`
- Raw string literals (`R"(...)"`)
- Unicode string literals (`u8`, `u`, `U`)
- User-defined literals (`operator""`)
- `alignas`, `alignof`
- `thread_local`
- Trailing return type (`auto f() -> int`)
- `[[noreturn]]`, `[[carries_dependency]]` attributes
- Defaulted/deleted special members

**Library**
- Smart pointers: `unique_ptr`, `shared_ptr`, `weak_ptr`
- `std::thread`, `std::mutex`, `std::condition_variable`, `std::future`, `std::promise`, `std::async`
- `std::atomic`
- `std::array`, `std::forward_list`, `std::unordered_map`, `std::unordered_set`
- `std::tuple`, `std::tie`
- `std::function`, `std::bind`
- `std::chrono`
- `std::regex`
- `std::random` (engines + distributions)
- `std::initializer_list`
- `std::move`, `std::forward`
- `std::move_iterator`, `std::ref`, `std::cref`
- Type traits (`<type_traits>`)
- Hash-based containers

---

### **C++14** (incremental polish on C++11)

**Language**
- Generic lambdas (`[](auto x){…}`)
- Lambda init-captures (`[x = expr]`)
- Relaxed `constexpr` (loops, multiple returns)
- Variable templates
- Return type deduction for normal functions (`auto f()`)
- Binary literals (`0b1010`)
- Digit separators (`1'000'000`)
- `[[deprecated]]` attribute

**Library**
- `std::make_unique`
- `std::shared_lock`, `std::shared_timed_mutex`
- `std::integer_sequence`
- `std::exchange`
- `std::quoted`
- Heterogeneous lookup in associative containers (transparent comparators)
- `std::tuple` element access by type
- User-defined literals for `chrono` and `string` (`"hi"s`, `42ms`)

---

### **C++17**

**Language**
- Structured bindings (`auto [a, b] = pair`)
- `if`/`switch` with initializer (`if (auto x = f(); x > 0)`)
- `constexpr if` (`if constexpr (...)`)
- Inline variables (`inline static`)
- Fold expressions
- Class template argument deduction (CTAD)
- Guaranteed copy elision
- `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]` attributes
- `__has_include`
- Hexadecimal floating-point literals
- Nested namespace definition (`namespace a::b::c`)
- UTF-8 character literals (`u8'x'`)
- Removed: `std::auto_ptr`, trigraphs, `register`, dynamic exception specs

**Library**
- `std::optional`
- `std::variant`
- `std::any`
- `std::string_view`
- `std::filesystem`
- `std::byte`
- Parallel algorithms (execution policies: `seq`, `par`, `par_unseq`)
- `std::invoke`, `std::apply`
- `std::not_fn`
- `std::scoped_lock`, `std::shared_mutex`
- Mathematical special functions
- `std::sample`
- Map/set **node handle splicing** (`extract`, `insert(node)`, `merge`)
- `try_emplace`, `insert_or_assign`
- `std::clamp`, `std::gcd`, `std::lcm`
- `std::to_chars`, `std::from_chars`

---

### **C++20** (the second "big" release after C++11)

**Language**
- **Concepts & requires clauses**
- **Modules**
- **Coroutines** (`co_await`, `co_yield`, `co_return`)
- Three-way comparison `<=>` (spaceship operator) + auto-generated comparisons
- Designated initializers (`{.x = 1, .y = 2}`)
- `consteval`, `constinit`
- Expanded `constexpr` (virtual functions, dynamic_cast, try-catch, allocations)
- Lambda improvements: template parameters, `[=, this]`, capture of `*this`, lambdas in unevaluated contexts
- `[[likely]]`, `[[unlikely]]`, `[[no_unique_address]]` attributes
- `char8_t`
- `using enum`
- Aggregate init with `()` parens
- Range-based for with init-statement
- `__VA_OPT__`
- Conditional `explicit`

**Library**
- **Ranges library** (`std::ranges`, views, pipelines)
- **`std::format`** (Python-style formatting)
- **`std::span`**
- **Coroutine support**: `std::coroutine_handle`
- Calendar & time-zone library (`std::chrono::year`, `month`, `zoned_time`)
- `std::jthread` (cooperative cancellation + auto-join)
- `std::stop_token`, `std::stop_source`
- `std::latch`, `std::barrier`, `std::counting_semaphore`, `std::binary_semaphore`
- `std::atomic_ref`, atomic smart pointers
- `std::bit_cast`, `<bit>` header (`popcount`, `countl_zero`, etc.)
- `std::source_location`
- `std::span`, `std::endian`
- `starts_with`, `ends_with`, `contains` on strings/maps
- `std::erase`, `std::erase_if` (uniform erase)
- `std::midpoint`, `std::lerp`
- `<numbers>` (math constants)

---

### **C++23**

**Language**
- Deducing `this` (explicit object parameter)
- `if consteval`
- Multidimensional subscript `operator[](a, b)`
- `static operator()` and `static operator[]`
- `auto(x)` / `auto{x}` decay copy
- Labels in compound statements
- `#elifdef`, `#elifndef`
- Attributes on lambdas
- Literal suffixes for size_t (`123uz`, `123z`)
- Relaxed structured bindings as condition

**Library**
- `std::expected<T, E>`
- `std::mdspan` (multi-dimensional span)
- `std::flat_map`, `std::flat_set`, `std::flat_multimap`, `std::flat_multiset`
- `std::generator` (coroutine-based)
- `std::print`, `std::println` (formatted I/O)
- Stacktrace library (`std::stacktrace`)
- `std::stride_view`, `zip`, `adjacent`, `chunk`, `slide`, `join_with`, `enumerate` and more range views
- `std::ranges::to<Container>(...)`
- `std::byteswap`
- `std::move_only_function`
- `std::out_ptr`, `std::inout_ptr`
- `std::basic_string::contains`
- `std::optional` monadic ops (`and_then`, `transform`, `or_else`)
- `<stdfloat>` fixed-width floating-point types

---

### **C++26** (in progress — features tentatively voted in)

**Language**
- **Reflection** (static reflection, `^^` operator, `std::meta`)
- **Contracts** (`pre`, `post`, `contract_assert`)
- Pack indexing (`Args...[N]`)
- `=delete("reason")` with diagnostic string
- Structured bindings as condition (full support)
- `constexpr` placement new
- Member-function pointers to non-static member functions in unevaluated contexts
- `#embed` (binary resource embedding)
- Trivial relocatability (`trivially_relocatable`)
- Allow `void{}` and `void()` expressions

**Library**
- **`std::execution`** / **Senders & Receivers** (async framework, P2300)
- **Linear algebra library** (`std::linalg`, BLAS-style)
- Hazard pointers, RCU
- `std::inplace_vector`
- `std::function_ref`
- `std::hive` (formerly `colony`)
- `std::submdspan`
- `std::dims`, `std::extents` improvements
- `std::simd` (data-parallel types)
- `std::indirect`, `std::polymorphic` (value-type wrappers for polymorphism)
- `std::text_encoding`
- `std::debugging` (`is_debugger_present`, `breakpoint`)
- Range concatenation (`views::concat`)
- More monadic operations on `std::expected`
- Saturating arithmetic (`add_sat`, `sub_sat`, etc.)

> ⚠️ C++26 is not yet finalized as of this writing — features may shift before official ratification.

---

### **Quick "if you remember nothing else"**

| Standard | Headline features |
|---|---|
| **C++98** | Classes, templates, STL, exceptions |
| **C++11** | Move semantics, lambdas, smart pointers, `auto`, threads, `constexpr` |
| **C++14** | Generic lambdas, `make_unique`, relaxed `constexpr` |
| **C++17** | Structured bindings, `optional`/`variant`/`any`, `string_view`, `filesystem`, parallel STL |
| **C++20** | Concepts, modules, coroutines, ranges, `format`, `span`, `jthread` |
| **C++23** | `expected`, `mdspan`, `print`, flat containers, stacktrace |
| **C++26** | Reflection, contracts, senders/receivers, linear algebra, `simd` |

---

**Next Files:**
- [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md)
- [STL Internals](5_STL_Internals.md)
- [Multithreading Deep Dive](6_Multithreading_Deep_Dive.md)
- [Performance Optimization](7_Performance_Optimization.md)
